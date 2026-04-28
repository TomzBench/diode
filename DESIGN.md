# Diode v2 — Design Notes

**Diode** = **D**eterministic **IO** **D**eclared **E**fficiently. A test
framework for mocking platform I/O calls on top of CMock + Unity, targeted at
both Linux host builds and Zephyr targets.

This document captures the design for a rebuild of diode in a standalone
repository, lifted out of `qwiet/platform/testing/diode/`. It records what we
learned from v1, the design principles we want to hold to, and the concrete
mechanisms (expectation channels, stack-resident expectations, macro discipline)
that fall out of those principles.

## Goals

- Single-line declarative expectations for platform I/O syscalls
  (`pal_net_socket_poll`, `libevdev_next_event`, `pal_net_recv`, ...).
- Deterministic, reproducible tests with no ordering surprises across unrelated
  handles.
- Zero heap dependency. Builds and runs on Zephyr without
  `CONFIG_HEAP_MEM_POOL_SIZE` or `CONFIG_COMMON_LIBC_MALLOC`.
- Wraps real syscalls via the linker `--wrap` mechanism that CMock already
  generates. Integration with CMock/Unity stays.

## Non-goals

- Replacing CMock/Unity. Diode is a layer over them, not a competitor.
- Behavioral fakes. Diode validates inputs and feeds outputs; it does not try to
  emulate the OS.
- General-purpose I/O virtualization. The library is purpose-built for unit
  tests where every event is spec'd up front.

## Lessons from v1

Three distinct kinds of brittleness emerged in v1, all conflated under "tests
get hard to write as the number of handles grows":

### 1. Single global FIFO across all handles

In v1, every poll expectation lands in one `__list`. If production code ever
changes the order it polls subsystem A vs. subsystem B (e.g., stylus before
touch), unrelated tests break for non-semantic reasons. There is no way to say
"these two streams are independent."

### 2. Positional encodings that drift out of sync

The flagship example:

```c
EXPECT_NET_POLL_OK(PAL_MSEC(100), "xxio", "xxeo", 1, 2, 3, 4);
```

This is three parallel sequences (events string, revents string, varargs FDs)
coupled by index. Adding a fifth FD requires editing all three in lockstep. A
typo in the event string silently degrades to `events = 0`. v1's own unit test
for the macro had to introspect 24 fields just to confirm the encoding expanded
correctly — a tell that the encoding is too clever.

### 3. Composed macros leak event counts into tests

`EXPECT_STYLUS_DOWN` queues 6 events. Tests then have hard-coded
`for (int i = 0; i < 6; i++) libevdev_next_event(...)` loops to drain them.
Adding one synthetic event to the gesture silently leaves a dangling
expectation. The high-level macro is the right abstraction; the test draining at
the low-level is the wrong one.

## Design principles

1. **Tests are static specifications.** Every expectation is known up front,
   declared in the test body. There is no incoming stream of bytes that needs to
   be caught at unknown times. This drives the lifetime model — every
   expectation lives exactly as long as the test function it appears in.
2. **Storage follows lifetime.** Since expectations live for one test, they live
   on the test's stack frame. No heap, no arena, no allocator.
3. **One handle, one channel.** Independent handles get independent expectation
   streams. The test framework does not impose a cross-handle ordering that
   production code did not actually require.
4. **Self-describing expectation literals.** Designated initializers, no
   positional encodings.
5. **Compile-time fail-fast.** Misuse should be a compile error or static
   assertion, not a silent zero or a dangling pointer.

## Core mechanism: stack-resident expectations

A test function's stack frame is alive for the entire test. C99 compound
literals at block scope have automatic storage with the lifetime of the
enclosing block. Combine the two and the expectation lives exactly as long as it
needs to.

```c
struct poll_expectation {
  struct pal_list_head node;
  pal_timeout_t timeout;
  int ret;
  int nfds;
  struct pollfd watching[DIODE_POLL_MAX_FDS];
  struct pollfd ready[DIODE_POLL_MAX_FDS];
};

void diode_poll_register(struct poll_expectation *e);

#define EXPECT_NET_POLL(_timeout, _ret, ...)                    \
  diode_poll_register(&(struct poll_expectation){               \
    .timeout = (_timeout),                                      \
    .ret     = (_ret),                                          \
    .nfds    = PAL_NUM_VA_ARGS(__VA_ARGS__),                    \
    .watching = { __VA_ARGS__ },                                \
  })
```

Used:

```c
void test_poll_two_streams(void) {
  EXPECT_NET_POLL(PAL_MSEC(100), 1, {.fd = 3, .events = POLLIN});
  EXPECT_NET_POLL(PAL_MSEC(100), 1, {.fd = 4, .events = POLLOUT});
  pal_net_socket_poll(...);
}
```

The compound literal's enclosing block is `test_poll_two_streams`'s body
(because the macro expands to a single function-call expression — no nested
`{}`). Both literals stay alive until the closing `}`. The intrusive
`pal_list_head node` lives inside the literal, so the channel list holds
pointers into still-valid stack memory. No allocation, anywhere.

This is the same pattern the Linux kernel uses for stack-allocated waiters and
timers: intrusive list nodes embedded in caller-owned storage.

## Core mechanism: per-handle channels

Replace the v1 single global `__list` with one list per handle:

```c
typedef int diode_handle_t;

struct diode_channel {
  struct pal_list_head expectations;
  /* per-handle metadata: kind tag, debug name, etc. */
};

static struct diode_channel channels[DIODE_MAX_HANDLES];
```

Handles are dense small ints issued by diode itself, not borrowed from the OS. A
static array indexed by handle is heapless, O(1), and avoids the "what hash
function?" bikeshed. If we ever need sparse keys (e.g., real OS fds from
production code), we drop in a kernel-style intrusive hash table
(`hlist_head[1 << bits]`, `DECLARE_HASHTABLE`) — same shape, same heaplessness.

A `pal_net_socket_poll(pfds, nfds, timeout)` call walks each fd's channel,
returns ready ones in `revents`, and pops the matched expectations. Cross-handle
ordering does not matter. Tests express what each handle produces, not the
syscall transcript.

## Core mechanism: intrusive lists

We keep the existing `pal_list_head` (kernel-style circular doubly-linked list).
It is the perfect substrate for this design — it stores nothing of its own, just
threads through caller-provided storage. No allocator required, no per-channel
memory pool, no fixed-size queue depth.

If we later want a map (sparse keys, hash buckets), the analogues are
`hlist_head` / `hlist_node` and `DECLARE_HASHTABLE`. Both are header-only,
heapless, and port cleanly from the kernel.

## Macro discipline

### Single-expression rule

The macro **must not** introduce a block. This breaks lifetime:

```c
/* WRONG — compound literal dies at the inner } */
#define EXPECT_NET_POLL(...) do {                            \
  struct poll_expectation _e = (struct poll_expectation){... }; \
  diode_poll_register(&_e);                                  \
} while (0)
```

The literal `_e` lives until the inner `}`, after which `__list` holds a
dangling pointer. Use a single function-call expression instead, so the compound
literal's enclosing block is the _caller's_ block. Add a comment in the header
that locks this in. A unit test that registers an expectation and then tries to
read it back catches accidental reintroduction of the do-while form.

### The loop footgun and `LISTIFY`

This silently breaks:

```c
for (int i = 0; i < 5; i++)
  EXPECT_NET_POLL(...);
```

Each iteration reuses the same stack slot. The list ends up with five pointers,
all aliased to the last value. Compilers are allowed to do this and they do.

We accept this as a known footgun and address it two ways:

1. **Document loudly.** Every `EXPECT_*` macro's docstring states "do not call
   inside a loop; use `EXPECT_LISTIFY(N, ...)` for repetition."
2. **Provide a `LISTIFY` form** (Zephyr-style preprocessor expansion). Each call
   site becomes a separate source location → separate compound literal →
   separate stack slot:

   ```c
   #define _DIODE_POLL_ONE(i, _) EXPECT_NET_POLL(PAL_MSEC(100), 1, {.fd=i+3, .events=POLLIN});
   LISTIFY(5, _DIODE_POLL_ONE, (;));
   ```

   This expands to five distinct `EXPECT_NET_POLL` invocations at the
   preprocessor level, each producing its own compound literal in the caller's
   frame.

If a test legitimately needs runtime repetition (e.g., count comes from a
runtime variable), the escape hatch is a function-scope array:

```c
struct poll_expectation e[5];
for (int i = 0; i < 5; i++) {
  e[i] = (struct poll_expectation){...};
  diode_poll_register(&e[i]);
}
```

Still heapless, just less ergonomic.

### Variable payload cap

Compound literals cannot have flexible array members. Variable-length
expectations need a compile-time max:

```c
#define DIODE_POLL_MAX_FDS 16
```

A test that polls more than 16 fds is doing something unusual and a
`static_assert` catches it. The waste at typical sizes (4–8 fds per test) is
tolerable for a test framework.

If a specific use case needs unbounded payloads, the escape is a two-literal
split:

```c
EXPECT_NET_POLL(PAL_MSEC(100), 1,
                /* watching */ (struct pollfd[]){ ... },
                /* count    */ N);
```

Both literals live in the same caller block. Fine, slightly more pointer
chasing.

## Memory model

Diode v2 has no allocator dependency.

- **Expectations:** caller's stack via compound literals.
- **Channel array:** static `.bss`, sized at compile time.
- **List nodes:** intrusive, embedded in expectations.
- **Strings, FD arrays, etc.:** all part of the expectation literal.

`diode_init()` and `diode_destroy()` walk the channel array to reset list heads.
There is no allocator hook to wire up per platform.

This works identically on:

- Linux host builds (no libc heap dependency, but it would be available).
- Zephyr targets without any of `CONFIG_HEAP_MEM_POOL_SIZE`,
  `CONFIG_COMMON_LIBC_MALLOC`, or `K_HEAP_DEFINE`.
- Picolibc / minimal libc configurations where `malloc` may not even link.

## API sketch

### Net poll

```c
EXPECT_NET_POLL(PAL_MSEC(100), 1,
                {.fd = 3, .events = POLLIN,  .revents = POLLERR},
                {.fd = 4, .events = POLLOUT, .revents = POLLOUT});
```

Designated initializers throughout. No positional event strings. `revents` is
part of each `pollfd` record; the validator copies it into the caller's output
array on match.

### Net recv

```c
EXPECT_NET_RECV(.socket = 3, .buf = "hello", .len = 5, .ret = 5);
```

Simple pattern. Wraps Unity's `ExpectAndReturn`. No diode-managed list needed,
but the macro shape stays consistent with the heap-pattern macros.

### Evdev (high-level gestures)

```c
EXPECT_STYLUS_DOWN(.x = 500, .y = 300, .pressure = 2000);
EXPECT_STYLUS_MOVE(.x = 510, .y = 310, .pressure = 2100);
EXPECT_STYLUS_UP();
```

Each high-level macro queues N low-level events (each its own stack-resident
expectation). Tests do not loop to drain them; they call `libevdev_next_event`
until it returns `-EAGAIN`. The low-level event count inside
`EXPECT_STYLUS_DOWN` becomes an implementation detail of the macro, not a
contract with the test.

## Open questions and future work

### Virtual clock + scenario layer

For UI-style tests (stylus strokes, multi-finger touches, debounce windows), a
per-syscall transcript is the wrong abstraction. The right shape is:

```c
diode_at(PAL_MSEC(0),  stylus_down(500, 300, 2000));
diode_at(PAL_MSEC(20), stylus_move(510, 310, 2100));
diode_at(PAL_MSEC(40), stylus_up());
diode_run_until(PAL_MSEC(50));
```

A virtual clock drives `pal_net_socket_poll`'s timeout: when the SUT calls poll,
diode advances time to the next scheduled event and marks the appropriate fd
ready. This decouples tests from the structure of the production loop and makes
timing-dependent bugs (debounce, idle timeout, double-tap) testable.

Tradeoff: real engineering — scheduler, virtual fd readiness, integration with
`pal_timeout_t`. Defer until there are ~10+ stroke-style tests demanding it. The
stack-resident, channel-keyed core is independent and ships first.

### Order-independent matching

Sometimes a test does not care about the order of two unrelated calls (e.g.,
"log a metric and emit a frame; either order is fine"). A future extension is a
"match any" expectation that scans the channel for the first expectation whose
predicate matches, rather than strict FIFO. This stays within the channel model;
no global change is required.

### Sparse handle keys

If diode ever needs to mock a syscall where production code uses real OS fds
(not handles diode issued), drop in a `DECLARE_HASHTABLE`-style intrusive hash
table. Same heaplessness, same intrusive-node pattern.

## Repository layout (proposed)

```
diode/
  README.md
  DESIGN.md                  # this file
  include/diode/
    diode.h                  # init/destroy/verify
    list.h                   # intrusive doubly-linked list (lifted from PAL)
    macros.h                 # PAL_NUM_VA_ARGS, LISTIFY, container_of
    net_poll.h
    net_recv.h
    input/evdev.h
  src/
    diode.c
    net_poll.c
    input/evdev.c
  tests/
    test_poll.c
    test_recv.c
    test_evdev.c
    test_lifetime.c          # guards against accidental block-scope regression
  cmake/
    diode.cmake              # CMock --wrap integration
  CMakeLists.txt
```

## Migration from v1

- v1 macros that take positional event strings (`"xxio"`) are removed. Tests
  must be rewritten using designated-initializer `EXPECT_NET_POLL`. This is
  mechanical but not automated.
- v1 tests that drained gestures with hard-coded loop counts switch to draining
  until `-EAGAIN`. Removes the implicit count coupling.
- v1's `diode_*_create_expectation` heap functions become `diode_*_register`
  registration functions. The signature changes from "allocate and link" to
  "link caller-provided storage."

## Review discipline

The v1 README's reviewer guidelines still apply:

- AI-implemented mocks ship only happy-path tests; edge cases need a human
  review of the test plan first. Correlated errors between SUT and test are the
  failure mode we are guarding against.
- Pay attention to: redundant coverage, error-path coverage, validator
  reachability, and assumptions about the function being mocked.
- New patterns (beyond simple/heap-channel) require deliberate design
  discussion. Do not improvise.
