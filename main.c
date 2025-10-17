#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "util.c"

#define STRUCT_NAME OtherStruct
#define STRUCT_FIELDS \
    X(int, i) \
    X(char *, string) \
    X(char, c)
#include "fmtgen.h"

#define STRUCT_NAME MyStruct
#define STRUCT_FIELDS \
    X(long, l) \
    X(double, d) \
    X(int, i)
#include "fmtgen.h"

#define STRUCT_NAME BigStruct
#define STRUCT_FIELDS \
    X(long, l) \
    X(OtherStruct, other_struct) \
    X(MyStruct, my_struct)
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

typedef union Types {
    char tchar;
    short tshort;
    int tint;
    long tlong;
    uchar tuchar;
    ushort tushort;
    uint tuint;
    ulong tulong;
    float tfloat;
    double tdouble;
    char * tstring;
} Types;

typedef struct Value {
    Types tval;
    char *fmt;
} Value;

#define STRUCT_PRINT(NAME, NESTED) \
    _Generic((NAME), \
    BigStruct : struct_print(&BigStruct_fmt, #NAME, &NAME, NESTED), \
    MyStruct : struct_print(&MyStruct_fmt, #NAME, &NAME, NESTED), \
    OtherStruct : struct_print(&OtherStruct_fmt, #NAME, &NAME, NESTED) \
)

void
struct_print(struct struct_fmt *fmt, const char *name, void *structure, int nested)
{
#define MATCH(X, fmt) \
    if(!strcmp(type, #X)) { \
        printf(fmt, *(X *) pointer); \
        continue; \
    }
#define MATCH_S(X) \
    if(!strcmp(type, #X)) { \
        struct_print(&X##_fmt, fmt->names[i], pointer, ++nested); \
        if (nested) \
            nested -= 1; \
        continue; \
    } \

    if (!nested)
        printf("%s %s:", fmt->struct_name, name);
    printf("\n");

    for (size_t i = 0; i < fmt->num_members; i++) {
        const char *type = fmt->types[i];
        void *pointer = ((unsigned char*)structure)+fmt->offsets[i];
        for (int j = 0; j < nested; j += 1)
            printf("\t");
        printf("\t %s %s: &%zu [%zu] = ",
               fmt->types[i], fmt->names[i], fmt->offsets[i], fmt->sizes[i]);

        MATCH(char, "%c\n");
        MATCH(uchar, "%c\n");
        MATCH(int, "%d\n");
        MATCH(uint, "%d\n");
        MATCH(long, "%ld\n");
        MATCH(ulong, "%ld\n");
        MATCH(char *, "%s\n");
        MATCH(float, "%f\n");
        MATCH(double, "%f\n");
        MATCH(int8, "%d\n");
        MATCH(int16, "%d\n");
        MATCH(int32, "%d\n");
        MATCH(int64, "%ld\n");
        MATCH(uint8, "%u\n");
        MATCH(uint16, "%u\n");
        MATCH(uint32, "%u\n");
        MATCH(uint64, "%lu\n");
        MATCH(usize, "%zu\n");
        MATCH(isize, "%zu\n");
        MATCH_S(OtherStruct);
        MATCH_S(MyStruct);

        error("Missing printf for type %s.\n", type);
        exit(EXIT_FAILURE);

#undef MATCH
#undef MATCH_S
        /* print_buffer(((unsigned char*)structure)+fmt->offsets[i], fmt->sizes[i]); */
        printf("\n");
    }
}


int
main(int argc, char **argv)
{
    OtherStruct other = {.i = 50, .string = "superstring", .c = 'a'};
    MyStruct mine = {.l = 100, .d = 100.0, .i = 2000};
    BigStruct big = {.l = 100, .other_struct = other };

    unsigned char tbuff[OtherStruct_fmt.packed_size];
    struct_pack(&OtherStruct_fmt, &other, tbuff);
    printf("t packed:\n\t");
    print_buffer(tbuff, sizeof(tbuff));
    printf("\n");

    unsigned char sbuff[MyStruct_fmt.packed_size];
    struct_pack(&MyStruct_fmt, &mine, sbuff);
    printf("s packed:\n\t");
    print_buffer(sbuff, sizeof(tbuff));
    printf("\n");

    STRUCT_PRINT(other, 0);
    STRUCT_PRINT(mine, 0);
    STRUCT_PRINT(big, 0);

    OtherStruct tst2;
    MyStruct sst2;

    struct_unpack(&OtherStruct_fmt, tbuff, &tst2); 
    struct_unpack(&MyStruct_fmt, sbuff, &sst2); 

    STRUCT_PRINT(tst2, 0);
    STRUCT_PRINT(sst2, 0);

    return 0;
}
