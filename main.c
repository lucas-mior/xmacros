#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
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

void
print_buffer(unsigned char *buffer, size_t size)
{
    for (size_t j = 0; j < size; j++) {
        printf(" %02x", buffer[j]);
    }
}

size_t
struct_pack(struct struct_fmt *fmt, void *structure, unsigned char *buffer)
{
    size_t pos = 0;

    for (size_t i = 0; i < fmt->num_members; i++) {
        memcpy(buffer+pos, ((unsigned char*)structure)+fmt->offsets[i], fmt->sizes[i]);
        pos += fmt->sizes[i];
    }

    return pos;
}

size_t
struct_unpack(struct struct_fmt *fmt, unsigned char *buffer, void *structure)
{
    size_t pos = 0;

    for (size_t i = 0; i < fmt->num_members; i++) {
        memcpy(((unsigned char*)structure)+fmt->offsets[i], buffer+pos, fmt->sizes[i]);
        pos += fmt->sizes[i];
    }

    return pos;
}

#define STRUCT_PRINT(NAME, NESTED) \
  _Generic((NAME), \
    BigStruct *:   struct_print(&BigStruct_fmt,   #NAME, NAME, NESTED), \
    NumberStruct *:struct_print(&NumberStruct_fmt,    #NAME, NAME, NESTED), \
    SmallStruct *: struct_print(&SmallStruct_fmt, #NAME, NAME, NESTED) \
  )

void
struct_print(struct struct_fmt *fmt, const char *name, void *structure, int nested)
{
#define PRIMITIVE(TYPE, fmt) if (!strcmp(type, #TYPE)) {  \
    printf(fmt, *(TYPE *) pointer);  \
    continue;  \
}
#define STRUCT(TYPE) if (!strcmp(type, #TYPE)) {  \
    struct_print(&TYPE##_fmt, fmt->names[i], pointer, ++nested);  \
    if (nested) { \
        nested -= 1;  \
    } \
    continue;  \
}  \
if(!strcmp(type, #TYPE " *")) {  \
    TYPE **tmp = pointer;  \
    struct_print(&TYPE##_fmt, fmt->names[i], *tmp, ++nested);  \
    if (nested) { \
        nested -= 1;  \
    } \
    continue;  \
}

#define GREEN "\x1b[32m"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"

    if (!nested)
        printf(GREEN"%s"RESET" %s:", fmt->struct_name, name);
    printf("\n");

    for (size_t i = 0; i < fmt->num_members; i++) {
        const char *type = fmt->types[i];
        void *pointer = ((unsigned char*)structure)+fmt->offsets[i];
        for (int j = 0; j < nested; j += 1)
            printf("\t");
        printf("\t "GREEN"%s"RESET" %s: &%zu [%zu] = ",
               fmt->types[i], fmt->names[i], fmt->offsets[i], fmt->sizes[i]);

        PRIMITIVE(char, "%c\n");
        PRIMITIVE(uchar, "%c\n");
        PRIMITIVE(short, "%d\n");
        PRIMITIVE(ushort, "%u\n");
        PRIMITIVE(int, "%d\n");
        PRIMITIVE(uint, "%u\n");
        PRIMITIVE(long, "%ld\n");
        PRIMITIVE(ulong, "%lu\n");
        PRIMITIVE(char *, "%s\n");
        PRIMITIVE(float, "%f\n");
        PRIMITIVE(double, "%f\n");
        PRIMITIVE(int8, "%d\n");
        PRIMITIVE(int16, "%d\n");
        PRIMITIVE(int32, "%d\n");
        PRIMITIVE(int64, "%ld\n");
        PRIMITIVE(uint8, "%u\n");
        PRIMITIVE(uint16, "%u\n");
        PRIMITIVE(uint32, "%u\n");
        PRIMITIVE(uint64, "%lu\n");
        PRIMITIVE(usize, "%zu\n");
        PRIMITIVE(isize, "%zu\n");
        PRIMITIVE(void *, "%p\n");

        STRUCT(SmallStruct);
        STRUCT(NumberStruct);

        printf("\n");
        error("Missing printf for type "RED"%s"RESET".\n", type);
        exit(EXIT_FAILURE);

#undef PRIMITIVE
#undef STRUCT
        /* print_buffer(((unsigned char*)structure)+fmt->offsets[i], fmt->sizes[i]); */
        printf("\n");
    }
}


int
main(int argc, char **argv)
{
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
