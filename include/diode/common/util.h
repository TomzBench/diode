/* SPDX-License-Identifier: MIT */
#ifndef DIODE_COMMON_UTIL_H
#define DIODE_COMMON_UTIL_H

/**
 * DIODE_NUM_VA_ARGS - count the number of variadic arguments
 *
 * Supports 1-16 arguments. Does not work with 0 arguments.
 */
#define DIODE_NUM_VA_ARGS(...)                                                 \
  DIODE_NUM_VA_ARGS_IMPL(                                                      \
      __VA_ARGS__, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define DIODE_NUM_VA_ARGS_IMPL(_1,                                             \
                               _2,                                             \
                               _3,                                             \
                               _4,                                             \
                               _5,                                             \
                               _6,                                             \
                               _7,                                             \
                               _8,                                             \
                               _9,                                             \
                               _10,                                            \
                               _11,                                            \
                               _12,                                            \
                               _13,                                            \
                               _14,                                            \
                               _15,                                            \
                               _16,                                            \
                               N,                                              \
                               ...)                                            \
  N

#endif /* DIODE_COMMON_UTIL_H */
