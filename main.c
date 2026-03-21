#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "assert.c"

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
#include "xstructs.c"

#define STRUCT_NAME SmallStruct
#define STRUCT_FIELDS                                                          \
    X(char *, string)                                                          \
    X(NumberStruct, number_struct)
#include "xstructs.c"

#define STRUCT_NAME BigStruct
#define STRUCT_FIELDS                                                          \
    X(long, l)                                                                 \
    X(SmallStruct *, small_struct)
#include "xstructs.c"

#define EXPAND_STRUCTS                                                         \
    STRUCT(SmallStruct)                                                        \
    STRUCT(NumberStruct)                                                       \
    STRUCT(BigStruct)
#include "fmt_functions.h"

#define ENUM_NAME WeekDay
#define ENUM_PREFIX_ WEEK_DAY_
#define ENUM_BITFLAGS 0
#define ENUM_FIELDS   \
    X(SUNDAY, 0)      \
    X(MONDAY)         \
    X(TUESDAY, 10)    \
    X(WEDNESDAY)      \
    X(THURSDAY)       \
    X(FRIDAY, 5)      \
    X(SATURDAY, 20)
#include "xenums.c"

#define ENUM_NAME PowerOfTwo
#define ENUM_PREFIX_ POWER_OF2_
#define ENUM_BITFLAGS 1
#define ENUM_FIELDS \
    X(ONE)          \
    X(TWO)          \
    X(FOUR)         \
    X(EIGHT)        \
    X(SIXTEEN)      \
    X(THIRTY2)      \
    X(SIXTY4)
#include "xenums.c"

int
main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    {
        char *str_ptr;

        /* Standard Enum Tests */
        ASSERT_EQUAL(WEEK_DAY_str(WEEK_DAY_SUNDAY), "WEEK_DAY_SUNDAY");
        ASSERT_EQUAL(WEEK_DAY_str(WEEK_DAY_MONDAY), "WEEK_DAY_MONDAY");
        ASSERT_EQUAL(WEEK_DAY_str(WEEK_DAY_SATURDAY), "WEEK_DAY_SATURDAY");
        ASSERT_EQUAL(WEEK_DAY_str(999), "Unknown value");

        /* Bitflag Enum Tests */
        if ((str_ptr = POWER_OF2_str(POWER_OF2_ONE))) {
            ASSERT_EQUAL(str_ptr, "POWER_OF2_ONE");
            free(str_ptr);
        }

        if ((str_ptr = POWER_OF2_str(POWER_OF2_ONE | POWER_OF2_FOUR | POWER_OF2_SIXTY4))) {
            ASSERT_EQUAL(str_ptr, "POWER_OF2_ONE|POWER_OF2_FOUR|POWER_OF2_SIXTY4");
            free(str_ptr);
        }

        ASSERT_EQUAL(POWER_OF2_str(0), "NONE");
    }

    {
        ASSERT_EQUAL(NumberStruct_fmt.num_members, 11);
        ASSERT_EQUAL(SmallStruct_fmt.num_members, 2);
        ASSERT_EQUAL(BigStruct_fmt.num_members, 2);

        ASSERT_EQUAL(NumberStruct_fmt.struct_name, "NumberStruct");
        ASSERT_EQUAL(SmallStruct_fmt.names[0], "string");
        ASSERT_EQUAL(SmallStruct_fmt.types[0], "char *");

        /* Check packed size against manual calculation */
        size_t expected_small_size = sizeof(char *) + sizeof(NumberStruct);
        ASSERT_EQUAL(SmallStruct_fmt.packed_size, expected_small_size);
    }

    {
        NumberStruct original_num = {
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
        NumberStruct restored_num;
        SmallStruct small_val = {
            .string = "reflection test",
            .number_struct = original_num
        };
        SmallStruct restored_small;
        BigStruct big_val = {
            .l = 123456789,
            .small_struct = &small_val
        };
        BigStruct *p_big = &big_val;
        uchar *n_buf;
        uchar *s_buf;

        if ((n_buf = xmalloc(NumberStruct_fmt.packed_size)) == NULL) {
            return EXIT_FAILURE;
        }
        NumberStruct_pack(&original_num, n_buf);
        NumberStruct_unpack(n_buf, &restored_num);

        ASSERT_EQUAL(original_num.ic, restored_num.ic);
        ASSERT_EQUAL(original_num.is, restored_num.is);
        ASSERT_EQUAL(original_num.ui, restored_num.ui);
        ASSERT_EQUAL(original_num.ld, restored_num.ld);

        if ((s_buf = xmalloc(SmallStruct_fmt.packed_size)) == NULL) {
            free(n_buf);
            return EXIT_FAILURE;
        }
        SmallStruct_pack(&small_val, s_buf);
        SmallStruct_unpack(s_buf, &restored_small);

        ASSERT_EQUAL(small_val.string, restored_small.string);
        ASSERT_EQUAL(small_val.number_struct.ii, restored_small.number_struct.ii);
        ASSERT_EQUAL(small_val.number_struct.f, restored_small.number_struct.f);

        printf("--- Printing BigStruct (Includes Pointer to SmallStruct) ---\n");
        STRUCT_PRINT(&big_val);

        printf("--- Printing BigStruct via Pointer ---\n");
        STRUCT_PRINT(p_big);

        printf("--- Printing Restored NumberStruct ---\n");
        STRUCT_PRINT(&restored_num);

        free(n_buf);
        free(s_buf);
    }

    printf("\nmain.c: All comprehensive tests passed.\n");
    return EXIT_SUCCESS;
}
