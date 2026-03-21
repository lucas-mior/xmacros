#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

#if !defined(CAT) || !defined(CAT3)
  #define CAT_(a, b)     a##b
  #define CAT3_(a, b, c) a##b##c
  #define CAT(a, b)      CAT_(a, b)
  #define CAT3(a, b, c)  CAT3_(a, b, c)
#endif

#if !defined(SELECT_ON_NUM_ARGS)
  #define NUM_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, n, ...) n
  #define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, x)
  #define SELECT_ON_NUM_ARGS(macro, ...) \
      CAT(macro, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
#endif

#if !defined(QUOTE)
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)
#endif

#define STRUCT_NAME NumberStruct
#define STRUCT_FIELDS                                                          \
    X(char, ic)                                                                \
    X(uchar, uc)                                                               \
    X(signed short, is)                                                        \
    X(ushort, us)                                                              \
    X(int, ii)                                                                 \
    X(uint, ui)                                                                \
    X(long, il)                                                                \
    X(ulong, ul)                                                               \
    X(float, f)                                                                \
    X(double, d)                                                               \
    X(long double, ld)
#include "xstructs.h"

#define STRUCT_NAME SmallStruct
#define STRUCT_FIELDS                                                          \
    X(char *, string)                                                          \
    X(NumberStruct, number_struct)
#include "xstructs.h"

#define STRUCT_NAME BigStruct
#define STRUCT_FIELDS                                                          \
    X(long, l)                                                                 \
    X(SmallStruct *, small_struct)
#include "xstructs.h"

#define EXPAND_STRUCTS                                                         \
    STRUCT(SmallStruct)                                                        \
    STRUCT(NumberStruct)                                                       \
    STRUCT(BigStruct)
#include "fmt_functions.h"

#define ENUM_NAME WeekDay
#define ENUM_PREFIX_ WEEK_DAY_
#define ENUM_FIELDS       \
    X(SUNDAY, 0)      \
    X(MONDAY)         \
    X(TUESDAY, 10)    \
    X(WEDNESDAY)      \
    X(THURSDAY)       \
    X(FRIDAY, 5)      \
    X(SATURDAY, 20)
#include "xenums.h"

#define ENUM_NAME PowerOfTwo
#define ENUM_PREFIX_ POWER_OF2_
#define ENUM_IS_FLAGS 1
#define ENUM_FIELDS \
    X(ONE,     1 << 0)             \
    X(TWO,     1 << 1)             \
    X(FOUR,    1 << 2)             \
    X(EIGHT,   1 << 3)             \
    X(SIXTEEN, 1 << 4)             \
    X(THIRTY2, 1 << 5)             \
    X(SIXTY4,  1 << 6)             \

#include "xenums.h"

int
main(int argc, char **argv) {
    NumberStruct mine = {
        .ic = 'c',
        .uc = 'd',
        .is = SHRT_MAX,
        .us = USHRT_MAX,
        .ii = INT_MAX,
        .ui = UINT_MAX,
        .il = LONG_MAX,
        .ul = ULONG_MAX,
        .f = 0.5f,
        .d = 0.5,
        .ld = 0.5
    };

    SmallStruct small = {
        .string = "superstring",
        .number_struct = mine
    };

    BigStruct big = {
        .l = 100,
        .small_struct = &small
    };

    BigStruct *pbig = &big;

    (void)argc;
    (void)argv;

    for (enum WeekDay day = WEEK_DAY_SUNDAY; day < WEEK_DAY_LAST; day += 1) {
        char *s = WEEK_DAY_str(day);
        if (strcmp(s, "Unknown value") != 0) {
            printf("day[%u] = %s\n", day, s);
        }
    }

    for (enum PowerOfTwo x = 0; x < POWER_OF2_LAST; x += 1) {
        char *value_name = POWER_OF2_str(x);
        printf("enum[%d] = %s\n", x, value_name);
    }
    exit(0);

    STRUCT_PRINT(&small);
    STRUCT_PRINT(&mine);
    STRUCT_PRINT(&big);
    STRUCT_PRINT(pbig);

    {
        SmallStruct small_from_buf;
        NumberStruct num_from_buf;
        uchar *s_buf;
        uchar *n_buf;

        if ((s_buf = malloc(SmallStruct_fmt.packed_size)) == NULL) {
            return EXIT_FAILURE;
        }
        if ((n_buf = malloc(NumberStruct_fmt.packed_size)) == NULL) {
            return EXIT_FAILURE;
        }

        SmallStruct_pack(&small, s_buf);
        printf("SmallStruct packed:\n\t");
        print_buffer(s_buf, SmallStruct_fmt.packed_size);
        printf("\n");

        NumberStruct_pack(&mine, n_buf);
        printf("NumberStruct packed:\n\t");
        print_buffer(n_buf, NumberStruct_fmt.packed_size);
        printf("\n");

        SmallStruct_unpack(s_buf, &small_from_buf);
        NumberStruct_unpack(n_buf, &num_from_buf);

        STRUCT_PRINT(&small_from_buf);
        STRUCT_PRINT(&num_from_buf);
    }

    return EXIT_SUCCESS;
}
