#include <stdio.h>
#include <stddef.h>
#include <limits.h>
#include <stdint.h>
#include "util.c"

#define STRUCT_NAME NumberStruct
#define STRUCT_FIELDS \
    X(char, c) \
    X(short, s) \
    X(int, i) \
    X(long, l) \
    X(uchar, uc) \
    X(ushort, us) \
    X(uint, ui) \
    X(ulong, ul) \
    X(float, f) \
    X(double, d)
#include "fmtgen.h"

#define STRUCT_NAME SmallStruct
#define STRUCT_FIELDS \
    X(char *, string) \
    X(NumberStruct, number_struct)
#include "fmtgen.h"

#define STRUCT_NAME BigStruct
#define STRUCT_FIELDS \
    X(long, l) \
    X(SmallStruct *, other_struct)
#include "fmtgen.h"

#define EXPAND_STRUCTS \
    STRUCT(SmallStruct); \
    STRUCT(NumberStruct);
#include "fmt_functions.h"

#define STRUCT_PRINT(NAME, NESTED) \
  _Generic((NAME), \
    BigStruct *:   struct_print(&BigStruct_fmt,   #NAME, NAME, NESTED), \
    NumberStruct *:struct_print(&NumberStruct_fmt,    #NAME, NAME, NESTED), \
    SmallStruct *: struct_print(&SmallStruct_fmt, #NAME, NAME, NESTED) \
  )

int
main(int argc, char **argv)
{
    (void) argc;
    (void) argv;
    NumberStruct mine = {
        .c = 'c',
        .s = SHRT_MAX,
        .i = INT_MAX,
        .l = LONG_MAX,
        .uc = 'd',
        .us = USHRT_MAX,
        .ui = UINT_MAX,
        .ul = ULONG_MAX,
        .f = 0.5f,
        .d = 0.5
    };
    SmallStruct other = {
        .string = "superstring",
        .number_struct = mine
    };
    BigStruct big = {
        .l = 100,
        .other_struct = &other
    };

    BigStruct *pbig = &big;

    unsigned char tbuff[SmallStruct_fmt.packed_size];
    struct_pack(&SmallStruct_fmt, &other, tbuff);
    printf("t packed:\n\t");
    print_buffer(tbuff, sizeof(tbuff));
    printf("\n");

    unsigned char sbuff[NumberStruct_fmt.packed_size];
    struct_pack(&NumberStruct_fmt, &mine, sbuff);
    printf("s packed:\n\t");
    print_buffer(sbuff, sizeof(tbuff));
    printf("\n");

    STRUCT_PRINT(&other, 0);
    STRUCT_PRINT(&mine, 0);
    STRUCT_PRINT(&big, 0);
    STRUCT_PRINT(pbig, 0);

    /* SmallStruct tst2; */
    /* NumberStruct sst2; */

    /* struct_unpack(&SmallStruct_fmt, tbuff, &tst2); */
    /* struct_unpack(&NumberStruct_fmt, sbuff, &sst2); */

    /* STRUCT_PRINT(&tst2, 0); */
    /* STRUCT_PRINT(&sst2, 0); */

    return 0;
}
