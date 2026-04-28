/* SPDX-License-Identifier: Apache-2.0 */
/**
 * listify.h — DIODE_LISTIFY(N, F, sep, ...). Port from Zephyr RTOS
 *
 * Repeats F(idx, ...) N times with separator, supporting 0..64.
 *
 * Usage:
 *   #define POINT(idx, _) {.x = idx, .y = idx * 10}
 *   struct point arr[4] = { DIODE_LISTIFY(4, POINT, (, )) };
 *
 *   #define ROW(idx, _) VLA(3, idx*3+1, idx*3+2, idx*3+3)
 *   struct vla mat = VLA(4, DIODE_LISTIFY(4, ROW, (, )));
 */
#ifndef DIODE_COMMON_LISTIFY_H
#define DIODE_COMMON_LISTIFY_H

#define DIODE_CAT(a, b) a##b
#define DIODE_LISTIFY(N, F, sep, ...)                                          \
  DIODE_CAT(DIODE_LISTIFY_, N)(F, sep, __VA_ARGS__)

// clang-format off
#define DIODE_LISTIFY_0(F, sep, ...)
#define DIODE_LISTIFY_1(F, sep, ...) \
    F(0, __VA_ARGS__)
#define DIODE_LISTIFY_2(F, sep, ...) \
    DIODE_LISTIFY_1(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(1, __VA_ARGS__)
#define DIODE_LISTIFY_3(F, sep, ...) \
    DIODE_LISTIFY_2(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(2, __VA_ARGS__)
#define DIODE_LISTIFY_4(F, sep, ...) \
    DIODE_LISTIFY_3(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(3, __VA_ARGS__)
#define DIODE_LISTIFY_5(F, sep, ...) \
    DIODE_LISTIFY_4(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(4, __VA_ARGS__)
#define DIODE_LISTIFY_6(F, sep, ...) \
    DIODE_LISTIFY_5(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(5, __VA_ARGS__)
#define DIODE_LISTIFY_7(F, sep, ...) \
    DIODE_LISTIFY_6(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(6, __VA_ARGS__)
#define DIODE_LISTIFY_8(F, sep, ...) \
    DIODE_LISTIFY_7(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(7, __VA_ARGS__)
#define DIODE_LISTIFY_9(F, sep, ...) \
    DIODE_LISTIFY_8(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(8, __VA_ARGS__)
#define DIODE_LISTIFY_10(F, sep, ...) \
    DIODE_LISTIFY_9(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(9, __VA_ARGS__)
#define DIODE_LISTIFY_11(F, sep, ...) \
    DIODE_LISTIFY_10(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(10, __VA_ARGS__)
#define DIODE_LISTIFY_12(F, sep, ...) \
    DIODE_LISTIFY_11(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(11, __VA_ARGS__)
#define DIODE_LISTIFY_13(F, sep, ...) \
    DIODE_LISTIFY_12(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(12, __VA_ARGS__)
#define DIODE_LISTIFY_14(F, sep, ...) \
    DIODE_LISTIFY_13(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(13, __VA_ARGS__)
#define DIODE_LISTIFY_15(F, sep, ...) \
    DIODE_LISTIFY_14(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(14, __VA_ARGS__)
#define DIODE_LISTIFY_16(F, sep, ...) \
    DIODE_LISTIFY_15(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(15, __VA_ARGS__)
#define DIODE_LISTIFY_17(F, sep, ...) \
    DIODE_LISTIFY_16(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(16, __VA_ARGS__)
#define DIODE_LISTIFY_18(F, sep, ...) \
    DIODE_LISTIFY_17(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(17, __VA_ARGS__)
#define DIODE_LISTIFY_19(F, sep, ...) \
    DIODE_LISTIFY_18(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(18, __VA_ARGS__)
#define DIODE_LISTIFY_20(F, sep, ...) \
    DIODE_LISTIFY_19(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(19, __VA_ARGS__)
#define DIODE_LISTIFY_21(F, sep, ...) \
    DIODE_LISTIFY_20(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(20, __VA_ARGS__)
#define DIODE_LISTIFY_22(F, sep, ...) \
    DIODE_LISTIFY_21(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(21, __VA_ARGS__)
#define DIODE_LISTIFY_23(F, sep, ...) \
    DIODE_LISTIFY_22(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(22, __VA_ARGS__)
#define DIODE_LISTIFY_24(F, sep, ...) \
    DIODE_LISTIFY_23(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(23, __VA_ARGS__)
#define DIODE_LISTIFY_25(F, sep, ...) \
    DIODE_LISTIFY_24(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(24, __VA_ARGS__)
#define DIODE_LISTIFY_26(F, sep, ...) \
    DIODE_LISTIFY_25(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(25, __VA_ARGS__)
#define DIODE_LISTIFY_27(F, sep, ...) \
    DIODE_LISTIFY_26(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(26, __VA_ARGS__)
#define DIODE_LISTIFY_28(F, sep, ...) \
    DIODE_LISTIFY_27(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(27, __VA_ARGS__)
#define DIODE_LISTIFY_29(F, sep, ...) \
    DIODE_LISTIFY_28(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(28, __VA_ARGS__)
#define DIODE_LISTIFY_30(F, sep, ...) \
    DIODE_LISTIFY_29(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(29, __VA_ARGS__)
#define DIODE_LISTIFY_31(F, sep, ...) \
    DIODE_LISTIFY_30(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(30, __VA_ARGS__)
#define DIODE_LISTIFY_32(F, sep, ...) \
    DIODE_LISTIFY_31(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(31, __VA_ARGS__)
#define DIODE_LISTIFY_33(F, sep, ...) \
    DIODE_LISTIFY_32(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(32, __VA_ARGS__)
#define DIODE_LISTIFY_34(F, sep, ...) \
    DIODE_LISTIFY_33(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(33, __VA_ARGS__)
#define DIODE_LISTIFY_35(F, sep, ...) \
    DIODE_LISTIFY_34(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(34, __VA_ARGS__)
#define DIODE_LISTIFY_36(F, sep, ...) \
    DIODE_LISTIFY_35(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(35, __VA_ARGS__)
#define DIODE_LISTIFY_37(F, sep, ...) \
    DIODE_LISTIFY_36(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(36, __VA_ARGS__)
#define DIODE_LISTIFY_38(F, sep, ...) \
    DIODE_LISTIFY_37(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(37, __VA_ARGS__)
#define DIODE_LISTIFY_39(F, sep, ...) \
    DIODE_LISTIFY_38(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(38, __VA_ARGS__)
#define DIODE_LISTIFY_40(F, sep, ...) \
    DIODE_LISTIFY_39(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(39, __VA_ARGS__)
#define DIODE_LISTIFY_41(F, sep, ...) \
    DIODE_LISTIFY_40(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(40, __VA_ARGS__)
#define DIODE_LISTIFY_42(F, sep, ...) \
    DIODE_LISTIFY_41(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(41, __VA_ARGS__)
#define DIODE_LISTIFY_43(F, sep, ...) \
    DIODE_LISTIFY_42(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(42, __VA_ARGS__)
#define DIODE_LISTIFY_44(F, sep, ...) \
    DIODE_LISTIFY_43(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(43, __VA_ARGS__)
#define DIODE_LISTIFY_45(F, sep, ...) \
    DIODE_LISTIFY_44(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(44, __VA_ARGS__)
#define DIODE_LISTIFY_46(F, sep, ...) \
    DIODE_LISTIFY_45(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(45, __VA_ARGS__)
#define DIODE_LISTIFY_47(F, sep, ...) \
    DIODE_LISTIFY_46(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(46, __VA_ARGS__)
#define DIODE_LISTIFY_48(F, sep, ...) \
    DIODE_LISTIFY_47(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(47, __VA_ARGS__)
#define DIODE_LISTIFY_49(F, sep, ...) \
    DIODE_LISTIFY_48(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(48, __VA_ARGS__)
#define DIODE_LISTIFY_50(F, sep, ...) \
    DIODE_LISTIFY_49(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(49, __VA_ARGS__)
#define DIODE_LISTIFY_51(F, sep, ...) \
    DIODE_LISTIFY_50(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(50, __VA_ARGS__)
#define DIODE_LISTIFY_52(F, sep, ...) \
    DIODE_LISTIFY_51(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(51, __VA_ARGS__)
#define DIODE_LISTIFY_53(F, sep, ...) \
    DIODE_LISTIFY_52(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(52, __VA_ARGS__)
#define DIODE_LISTIFY_54(F, sep, ...) \
    DIODE_LISTIFY_53(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(53, __VA_ARGS__)
#define DIODE_LISTIFY_55(F, sep, ...) \
    DIODE_LISTIFY_54(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(54, __VA_ARGS__)
#define DIODE_LISTIFY_56(F, sep, ...) \
    DIODE_LISTIFY_55(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(55, __VA_ARGS__)
#define DIODE_LISTIFY_57(F, sep, ...) \
    DIODE_LISTIFY_56(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(56, __VA_ARGS__)
#define DIODE_LISTIFY_58(F, sep, ...) \
    DIODE_LISTIFY_57(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(57, __VA_ARGS__)
#define DIODE_LISTIFY_59(F, sep, ...) \
    DIODE_LISTIFY_58(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(58, __VA_ARGS__)
#define DIODE_LISTIFY_60(F, sep, ...) \
    DIODE_LISTIFY_59(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(59, __VA_ARGS__)
#define DIODE_LISTIFY_61(F, sep, ...) \
    DIODE_LISTIFY_60(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(60, __VA_ARGS__)
#define DIODE_LISTIFY_62(F, sep, ...) \
    DIODE_LISTIFY_61(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(61, __VA_ARGS__)
#define DIODE_LISTIFY_63(F, sep, ...) \
    DIODE_LISTIFY_62(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(62, __VA_ARGS__)
#define DIODE_LISTIFY_64(F, sep, ...) \
    DIODE_LISTIFY_63(F, sep, __VA_ARGS__) DIODE_DEBRACKET sep \
    F(63, __VA_ARGS__)
// clang-format on

/* Strip parentheses from separator: DIODE_DEBRACKET (, ) => , */
#define DIODE_DEBRACKET(...) __VA_ARGS__

#endif /* DIODE_COMMON_LISTIFY_H */
