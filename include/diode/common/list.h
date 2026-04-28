/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Circular doubly linked list implementation.
 *
 * Adapted from the Linux kernel (include/linux/list.h) for userspace use.
 * Original authors: Linux kernel developers
 */
#ifndef DIODE_COMMON_LIST_H
#define DIODE_COMMON_LIST_H

#include <stddef.h>

struct diode_list_head {
  struct diode_list_head *next, *prev;
};

/*
 * Circular doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

/**
 * DIODE_LIST_HEAD_INIT - initialize a list_head's links to point to itself
 * @name: name of the diode_list_head
 */
#define DIODE_LIST_HEAD_INIT(name) {&(name), &(name)}

/**
 * DIODE_LIST_HEAD - definition of a diode_list_head with initialization values
 * @name: name of the diode_list_head
 */
#define DIODE_LIST_HEAD(name)                                                  \
  struct diode_list_head name = DIODE_LIST_HEAD_INIT(name)

/**
 * diode_list_init - Initialize a diode_list_head structure
 * @list: diode_list_head structure to be initialized.
 *
 * Initializes the diode_list_head to point to itself. If it is a list header,
 * the result is an empty list.
 */
static inline void
diode_list_init(struct diode_list_head *list)
{
  list->next = list;
  list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void
__diode_list_add(struct diode_list_head *new,
                 struct diode_list_head *prev,
                 struct diode_list_head *next)
{
  next->prev = new;
  new->next = next;
  new->prev = prev;
  prev->next = new;
}

/**
 * diode_list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void
diode_list_add(struct diode_list_head *new, struct diode_list_head *head)
{
  __diode_list_add(new, head, head->next);
}

/**
 * diode_list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void
diode_list_add_tail(struct diode_list_head *new, struct diode_list_head *head)
{
  __diode_list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void
__diode_list_del(struct diode_list_head *prev, struct diode_list_head *next)
{
  next->prev = prev;
  prev->next = next;
}

static inline void
__diode_list_del_entry(struct diode_list_head *entry)
{
  __diode_list_del(entry->prev, entry->next);
}

/**
 * diode_list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: diode_list_empty() on entry does not return true after this, the entry
 * is in an undefined state.
 */
static inline void
diode_list_del(struct diode_list_head *entry)
{
  __diode_list_del_entry(entry);
  entry->next = NULL;
  entry->prev = NULL;
}

/**
 * diode_list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void
diode_list_replace(struct diode_list_head *old, struct diode_list_head *new)
{
  new->next = old->next;
  new->next->prev = new;
  new->prev = old->prev;
  new->prev->next = new;
}

/**
 * diode_list_replace_init - replace old entry by new one and initialize the old
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void
diode_list_replace_init(struct diode_list_head *old,
                        struct diode_list_head *new)
{
  diode_list_replace(old, new);
  diode_list_init(old);
}

/**
 * diode_list_swap - replace entry1 with entry2 and re-add entry1 at entry2's
 *                   position
 * @entry1: the location to place entry2
 * @entry2: the location to place entry1
 */
static inline void
diode_list_swap(struct diode_list_head *entry1, struct diode_list_head *entry2)
{
  struct diode_list_head *pos = entry2->prev;

  diode_list_del(entry2);
  diode_list_replace(entry1, entry2);
  if (pos == entry1)
    pos = entry2;
  diode_list_add(entry1, pos);
}

/**
 * diode_list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void
diode_list_del_init(struct diode_list_head *entry)
{
  __diode_list_del_entry(entry);
  diode_list_init(entry);
}

/**
 * diode_list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void
diode_list_move(struct diode_list_head *list, struct diode_list_head *head)
{
  __diode_list_del_entry(list);
  diode_list_add(list, head);
}

/**
 * diode_list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void
diode_list_move_tail(struct diode_list_head *list, struct diode_list_head *head)
{
  __diode_list_del_entry(list);
  diode_list_add_tail(list, head);
}

/**
 * diode_list_bulk_move_tail - move a subsection of a list to its tail
 * @head: the head that will follow our entry
 * @first: first entry to move
 * @last: last entry to move, can be the same as first
 *
 * Move all entries between @first and including @last before @head.
 * All three entries must belong to the same linked list.
 */
static inline void
diode_list_bulk_move_tail(struct diode_list_head *head,
                          struct diode_list_head *first,
                          struct diode_list_head *last)
{
  first->prev->next = last->next;
  last->next->prev = first->prev;

  head->prev->next = first;
  first->prev = head->prev;

  last->next = head;
  head->prev = last;
}

/**
 * diode_list_is_first -- tests whether @list is the first entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int
diode_list_is_first(const struct diode_list_head *list,
                    const struct diode_list_head *head)
{
  return list->prev == head;
}

/**
 * diode_list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int
diode_list_is_last(const struct diode_list_head *list,
                   const struct diode_list_head *head)
{
  return list->next == head;
}

/**
 * diode_list_is_head - tests whether @list is the list @head
 * @list: the entry to test
 * @head: the head of the list
 */
static inline int
diode_list_is_head(const struct diode_list_head *list,
                   const struct diode_list_head *head)
{
  return list == head;
}

/**
 * diode_list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int
diode_list_empty(const struct diode_list_head *head)
{
  return head->next == head;
}

/**
 * diode_list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
static inline void
diode_list_rotate_left(struct diode_list_head *head)
{
  struct diode_list_head *first;

  if (!diode_list_empty(head)) {
    first = head->next;
    diode_list_move_tail(first, head);
  }
}

/**
 * diode_list_rotate_to_front() - Rotate list to specific item.
 * @list: The desired new front of the list.
 * @head: The head of the list.
 *
 * Rotates list so that @list becomes the new front of the list.
 */
static inline void
diode_list_rotate_to_front(struct diode_list_head *list,
                           struct diode_list_head *head)
{
  diode_list_move_tail(head, list);
}

/**
 * diode_list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
static inline int
diode_list_is_singular(const struct diode_list_head *head)
{
  return !diode_list_empty(head) && (head->next == head->prev);
}

static inline void
__diode_list_cut_position(struct diode_list_head *list,
                          struct diode_list_head *head,
                          struct diode_list_head *entry)
{
  struct diode_list_head *new_first = entry->next;
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry;
  entry->next = list;
  head->next = new_first;
  new_first->prev = head;
}

/**
 * diode_list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *         and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 */
static inline void
diode_list_cut_position(struct diode_list_head *list,
                        struct diode_list_head *head,
                        struct diode_list_head *entry)
{
  if (diode_list_empty(head))
    return;
  if (diode_list_is_singular(head) && !diode_list_is_head(entry, head) &&
      (entry != head->next))
    return;
  if (diode_list_is_head(entry, head))
    diode_list_init(list);
  else
    __diode_list_cut_position(list, head, entry);
}

/**
 * diode_list_cut_before - cut a list into two, before given entry
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *
 * This helper moves the initial part of @head, up to but
 * excluding @entry, from @head to @list. You should pass
 * in @entry an element you know is on @head. @list should
 * be an empty list or a list you do not care about losing
 * its data.
 * If @entry == @head, all entries on @head are moved to @list.
 */
static inline void
diode_list_cut_before(struct diode_list_head *list,
                      struct diode_list_head *head,
                      struct diode_list_head *entry)
{
  if (head->next == entry) {
    diode_list_init(list);
    return;
  }
  list->next = head->next;
  list->next->prev = list;
  list->prev = entry->prev;
  list->prev->next = list;
  head->next = entry;
  entry->prev = head;
}

static inline void
__diode_list_splice(const struct diode_list_head *list,
                    struct diode_list_head *prev,
                    struct diode_list_head *next)
{
  struct diode_list_head *first = list->next;
  struct diode_list_head *last = list->prev;

  first->prev = prev;
  prev->next = first;

  last->next = next;
  next->prev = last;
}

/**
 * diode_list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void
diode_list_splice(const struct diode_list_head *list,
                  struct diode_list_head *head)
{
  if (!diode_list_empty(list))
    __diode_list_splice(list, head, head->next);
}

/**
 * diode_list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void
diode_list_splice_tail(struct diode_list_head *list,
                       struct diode_list_head *head)
{
  if (!diode_list_empty(list))
    __diode_list_splice(list, head->prev, head);
}

/**
 * diode_list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void
diode_list_splice_init(struct diode_list_head *list,
                       struct diode_list_head *head)
{
  if (!diode_list_empty(list)) {
    __diode_list_splice(list, head, head->next);
    diode_list_init(list);
  }
}

/**
 * diode_list_splice_tail_init - join two lists and reinitialise the emptied
 *                               list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void
diode_list_splice_tail_init(struct diode_list_head *list,
                            struct diode_list_head *head)
{
  if (!diode_list_empty(list)) {
    __diode_list_splice(list, head->prev, head);
    diode_list_init(list);
  }
}

/**
 * diode_list_entry - get the struct for this entry
 * @ptr:    the &struct diode_list_head pointer.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the diode_list_head within the struct.
 */
#define diode_list_entry(ptr, type, member)                                    \
  ((type *)((char *)(ptr) - offsetof(type, member)))

/**
 * diode_list_first_entry - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the diode_list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define diode_list_first_entry(ptr, type, member)                              \
  diode_list_entry((ptr)->next, type, member)

/**
 * diode_list_last_entry - get the last element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the diode_list_head within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define diode_list_last_entry(ptr, type, member)                               \
  diode_list_entry((ptr)->prev, type, member)

/**
 * diode_list_first_entry_or_null - get the first element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the diode_list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define diode_list_first_entry_or_null(ptr, type, member)                      \
  ({                                                                           \
    struct diode_list_head *head__ = (ptr);                                    \
    struct diode_list_head *pos__ = head__->next;                              \
    pos__ != head__ ? diode_list_entry(pos__, type, member) : NULL;            \
  })

/**
 * diode_list_last_entry_or_null - get the last element from a list
 * @ptr:    the list head to take the element from.
 * @type:   the type of the struct this is embedded in.
 * @member: the name of the diode_list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define diode_list_last_entry_or_null(ptr, type, member)                       \
  ({                                                                           \
    struct diode_list_head *head__ = (ptr);                                    \
    struct diode_list_head *pos__ = head__->prev;                              \
    pos__ != head__ ? diode_list_entry(pos__, type, member) : NULL;            \
  })

/**
 * diode_list_next_entry - get the next element in list
 * @pos:    the type * to cursor
 * @member: the name of the diode_list_head within the struct.
 */
#define diode_list_next_entry(pos, member)                                     \
  diode_list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * diode_list_next_entry_circular - get the next element in list
 * @pos:    the type * to cursor.
 * @head:   the list head to take the element from.
 * @member: the name of the diode_list_head within the struct.
 *
 * Wraparound if pos is the last element (return the first element).
 * Note, that list is expected to be not empty.
 */
#define diode_list_next_entry_circular(pos, head, member)                      \
  (diode_list_is_last(&(pos)->member, head)                                    \
       ? diode_list_first_entry(head, typeof(*(pos)), member)                  \
       : diode_list_next_entry(pos, member))

/**
 * diode_list_prev_entry - get the prev element in list
 * @pos:    the type * to cursor
 * @member: the name of the diode_list_head within the struct.
 */
#define diode_list_prev_entry(pos, member)                                     \
  diode_list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * diode_list_prev_entry_circular - get the prev element in list
 * @pos:    the type * to cursor.
 * @head:   the list head to take the element from.
 * @member: the name of the diode_list_head within the struct.
 *
 * Wraparound if pos is the first element (return the last element).
 * Note, that list is expected to be not empty.
 */
#define diode_list_prev_entry_circular(pos, head, member)                      \
  (diode_list_is_first(&(pos)->member, head)                                   \
       ? diode_list_last_entry(head, typeof(*(pos)), member)                   \
       : diode_list_prev_entry(pos, member))

/**
 * diode_list_for_each - iterate over a list
 * @pos:  the &struct diode_list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define diode_list_for_each(pos, head)                                         \
  for (pos = (head)->next; !diode_list_is_head(pos, (head)); pos = pos->next)

/**
 * diode_list_for_each_continue - continue iteration over a list
 * @pos:  the &struct diode_list_head to use as a loop cursor.
 * @head: the head for your list.
 *
 * Continue to iterate over a list, continuing after the current position.
 */
#define diode_list_for_each_continue(pos, head)                                \
  for (pos = pos->next; !diode_list_is_head(pos, (head)); pos = pos->next)

/**
 * diode_list_for_each_prev - iterate over a list backwards
 * @pos:  the &struct diode_list_head to use as a loop cursor.
 * @head: the head for your list.
 */
#define diode_list_for_each_prev(pos, head)                                    \
  for (pos = (head)->prev; !diode_list_is_head(pos, (head)); pos = pos->prev)

/**
 * diode_list_for_each_safe - iterate over a list safe against removal of entry
 * @pos:  the &struct diode_list_head to use as a loop cursor.
 * @n:    another &struct diode_list_head to use as temporary storage
 * @head: the head for your list.
 */
#define diode_list_for_each_safe(pos, n, head)                                 \
  for (pos = (head)->next, n = pos->next; !diode_list_is_head(pos, (head));    \
       pos = n, n = pos->next)

/**
 * diode_list_for_each_prev_safe - iterate over a list backwards safe against
 *                                 removal of list entry
 * @pos:  the &struct diode_list_head to use as a loop cursor.
 * @n:    another &struct diode_list_head to use as temporary storage
 * @head: the head for your list.
 */
#define diode_list_for_each_prev_safe(pos, n, head)                            \
  for (pos = (head)->prev, n = pos->prev; !diode_list_is_head(pos, (head));    \
       pos = n, n = pos->prev)

/**
 * diode_list_count_nodes - count nodes in the list
 * @head: the head for your list.
 */
static inline size_t
diode_list_count_nodes(struct diode_list_head *head)
{
  struct diode_list_head *pos;
  size_t count = 0;

  diode_list_for_each(pos, head) count++;

  return count;
}

/**
 * diode_list_entry_is_head - test if the entry points to the head of the list
 * @pos:    the type * to cursor
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 */
#define diode_list_entry_is_head(pos, head, member)                            \
  diode_list_is_head(&(pos)->member, (head))

/**
 * diode_list_for_each_entry - iterate over list of given type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 */
#define diode_list_for_each_entry(pos, head, member)                           \
  for (pos = diode_list_first_entry(head, typeof(*pos), member);               \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = diode_list_next_entry(pos, member))

/**
 * diode_list_for_each_entry_reverse - iterate backwards over list of given
 *                                     type.
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 */
#define diode_list_for_each_entry_reverse(pos, head, member)                   \
  for (pos = diode_list_last_entry(head, typeof(*pos), member);                \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = diode_list_prev_entry(pos, member))

/**
 * diode_list_prepare_entry - prepare a pos entry for use in
 *                            diode_list_for_each_entry_continue()
 * @pos:    the type * to use as a start point
 * @head:   the head of the list
 * @member: the name of the diode_list_head within the struct.
 *
 * Prepares a pos entry for use as a start point in
 * diode_list_for_each_entry_continue().
 */
#define diode_list_prepare_entry(pos, head, member)                            \
  ((pos) ?: diode_list_entry(head, typeof(*pos), member))

/**
 * diode_list_for_each_entry_continue - continue iteration over list of given
 *                                      type
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define diode_list_for_each_entry_continue(pos, head, member)                  \
  for (pos = diode_list_next_entry(pos, member);                               \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = diode_list_next_entry(pos, member))

/**
 * diode_list_for_each_entry_continue_reverse - iterate backwards from given
 *                                              point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define diode_list_for_each_entry_continue_reverse(pos, head, member)          \
  for (pos = diode_list_prev_entry(pos, member);                               \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = diode_list_prev_entry(pos, member))

/**
 * diode_list_for_each_entry_from - iterate over list of given type from the
 *                                  current point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define diode_list_for_each_entry_from(pos, head, member)                      \
  for (; !diode_list_entry_is_head(pos, head, member);                         \
       pos = diode_list_next_entry(pos, member))

/**
 * diode_list_for_each_entry_from_reverse - iterate backwards over list of given
 *                                          type from the current point
 * @pos:    the type * to use as a loop cursor.
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 *
 * Iterate backwards over list of given type, continuing from current position.
 */
#define diode_list_for_each_entry_from_reverse(pos, head, member)              \
  for (; !diode_list_entry_is_head(pos, head, member);                         \
       pos = diode_list_prev_entry(pos, member))

/**
 * diode_list_for_each_entry_safe - iterate over list of given type safe against
 *                                  removal of list entry
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 */
#define diode_list_for_each_entry_safe(pos, n, head, member)                   \
  for (pos = diode_list_first_entry(head, typeof(*pos), member),               \
      n = diode_list_next_entry(pos, member);                                  \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = n, n = diode_list_next_entry(n, member))

/**
 * diode_list_for_each_entry_safe_continue - continue list iteration safe
 *                                           against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define diode_list_for_each_entry_safe_continue(pos, n, head, member)          \
  for (pos = diode_list_next_entry(pos, member),                               \
      n = diode_list_next_entry(pos, member);                                  \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = n, n = diode_list_next_entry(n, member))

/**
 * diode_list_for_each_entry_safe_from - iterate over list from current point
 *                                       safe against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define diode_list_for_each_entry_safe_from(pos, n, head, member)              \
  for (n = diode_list_next_entry(pos, member);                                 \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = n, n = diode_list_next_entry(n, member))

/**
 * diode_list_for_each_entry_safe_reverse - iterate backwards over list safe
 *                                          against removal
 * @pos:    the type * to use as a loop cursor.
 * @n:      another type * to use as temporary storage
 * @head:   the head for your list.
 * @member: the name of the diode_list_head within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define diode_list_for_each_entry_safe_reverse(pos, n, head, member)           \
  for (pos = diode_list_last_entry(head, typeof(*pos), member),                \
      n = diode_list_prev_entry(pos, member);                                  \
       !diode_list_entry_is_head(pos, head, member);                           \
       pos = n, n = diode_list_prev_entry(n, member))

/**
 * diode_list_safe_reset_next - reset a stale diode_list_for_each_entry_safe
 *                              loop
 * @pos:    the loop cursor used in the diode_list_for_each_entry_safe loop
 * @n:      temporary storage used in diode_list_for_each_entry_safe
 * @member: the name of the diode_list_head within the struct.
 *
 * diode_list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and diode_list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define diode_list_safe_reset_next(pos, n, member)                             \
  n = diode_list_next_entry(pos, member)

#endif /* DIODE_COMMON_LIST_H */
