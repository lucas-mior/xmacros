#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>

static char *program;

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
    STRUCT(SmallStruct);                                                       \
    STRUCT(NumberStruct);                                                      \
    STRUCT(BigStruct);
#include "fmt_functions.h"

#define STRUCT_PRINT(NAME)                                                     \
    _Generic((NAME),                                                           \
        BigStruct *: struct_print(&BigStruct_fmt, #NAME, NAME, 0),             \
        NumberStruct *: struct_print(&NumberStruct_fmt, #NAME, NAME, 0),       \
        SmallStruct *: struct_print(&SmallStruct_fmt, #NAME, NAME, 0))

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

int
main(int argc, char **argv) {
    NumberStruct mine = {.ic = 'c',
                         .uc = 'd',
                         .is = SHRT_MAX,
                         .us = USHRT_MAX,
                         .ii = INT_MAX,
                         .ui = UINT_MAX,
                         .il = LONG_MAX,
                         .ul = ULONG_MAX,
                         .f = 0.5f,
                         .d = 0.5,
                         .ld = 0.5};
    SmallStruct small = {.string = "superstring", .number_struct = mine};
    BigStruct big = {.l = 100, .small_struct = &small};

    BigStruct *pbig = &big;

    for (enum WeekDay day = 0; day < WEEK_DAY_LAST; day += 1) {
        printf("day[%u] = %s\n", day, WEEK_DAY_str(day));
    }

    (void)argc;
    program = argv[0];

    STRUCT_PRINT(&small);
    STRUCT_PRINT(&mine);
    STRUCT_PRINT(&big);
    STRUCT_PRINT(pbig);

    {
        SmallStruct small_struct_from_buffer;
        NumberStruct number_struct_from_buffer;

        uchar *small_struct_buffer;
        uchar *number_struct_buffer;

        small_struct_buffer = malloc(SmallStruct_fmt.packed_size);
        number_struct_buffer = malloc(NumberStruct_fmt.packed_size);

        struct_pack(&SmallStruct_fmt, &small, small_struct_buffer);
        printf("t packed:\n\t");
        print_buffer(small_struct_buffer, sizeof(small_struct_buffer));
        printf("\n");

        struct_pack(&NumberStruct_fmt, &mine, number_struct_buffer);
        printf("s packed:\n\t");
        print_buffer(number_struct_buffer, sizeof(small_struct_buffer));
        printf("\n");

        struct_unpack(&SmallStruct_fmt, small_struct_buffer,
                      &small_struct_from_buffer);
        struct_unpack(&NumberStruct_fmt, number_struct_buffer,
                      &number_struct_from_buffer);

        STRUCT_PRINT(&small_struct_from_buffer);
        STRUCT_PRINT(&number_struct_from_buffer);
    }

    exit(EXIT_SUCCESS);
}
