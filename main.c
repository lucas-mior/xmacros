#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "util.c"

#define STRUCT_NAME OtherStruct
#define STRUCT_FIELDS \
    X(int, i) \
    X(float, f) \
    X(char, c)
#include "fmtgen.h"

#define STRUCT_NAME MyStruct
#define STRUCT_FIELDS \
    X(long, l) \
    X(double, d) \
    X(int, i)
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

Value
formatter(const char *type, void *pointer) {
#define MATCH(X) !strcmp(type, #X)
    Value value;

    Types tval;
    char *fmt;

    if (MATCH(char)) {
        fmt = "%c";
        tval.tchar = *(char *) pointer;
    } else if (MATCH(int)) {
        fmt = "%d";
        tval.tint = *(int *) pointer;
    } else if (MATCH(long)) {
        fmt = "%ld";
        tval.tlong = *(long *) pointer;
    } else if (MATCH(char *)) {
        fmt = "%s";
        tval.tstring = *(char  **) pointer;
    } else if (MATCH(float)) {
        fmt = "%f";
        tval.tfloat = *(float *) pointer;
        error("float case: %f\n", tval.tfloat);
    } else if (MATCH(double)) {
        fmt = "%f";
        /* tval.tdouble = *(double *) pointer; */
        memcpy(&tval.tdouble, pointer, sizeof(double));
        error("double case: %f\n", tval.tdouble);
    } else {
        error("formaatter.\n");
        exit(EXIT_FAILURE);
    }

    value.fmt = fmt;
    value.tval = tval;

    return value;

#undef MATCH
}

void
struct_print(struct struct_fmt *fmt, void *structure)
{
    printf("%s:\n", fmt->struct_name);
    for (size_t i = 0; i < fmt->num_members; i++) {
        char fmt_buffer[128];
        Value value = formatter(fmt->types[i], ((unsigned char*)structure)+fmt->offsets[i]);
        SNPRINTF(fmt_buffer, "\t %%s %%s: &%%zu [%%zu] = %s", value.fmt);
        printf(fmt_buffer,
               fmt->types[i], fmt->names[i], fmt->offsets[i], fmt->sizes[i],
               value.tval);
        /* print_buffer(((unsigned char*)structure)+fmt->offsets[i], fmt->sizes[i]); */
        printf("\n");
    }
}


int
main(int argc, char **argv)
{
    OtherStruct other = {.i = 50, .f = 100.0f, .c = 'a'};
    MyStruct mine = {.l = 100, .d = 100.0, .i = 2000};

    unsigned char tbuff[OtherStruct_fmt.packed_size];
    struct_pack(&MyStruct_fmt, &other, tbuff);
    printf("t packed:\n\t");
    print_buffer(tbuff, sizeof(tbuff));
    printf("\n");

    unsigned char sbuff[MyStruct_fmt.packed_size];
    struct_pack(&MyStruct_fmt, &mine, sbuff);
    printf("s packed:\n\t");
    print_buffer(sbuff, sizeof(tbuff));
    printf("\n");

    struct_print(&OtherStruct_fmt, &other);
    struct_print(&MyStruct_fmt, &mine);

    OtherStruct tst2;
    MyStruct sst2;

    struct_unpack(&OtherStruct_fmt, tbuff, &tst2); 
    struct_unpack(&MyStruct_fmt, sbuff, &sst2); 

    struct_print(&OtherStruct_fmt, &tst2);
    struct_print(&MyStruct_fmt, &sst2);

    return 0;
}
