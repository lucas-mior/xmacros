#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define STRUCT_NAME t
#define STRUCT_FIELDS \
    X(int, a) \
    X(float, b) \
    X(char, c)
#include "fmtgen.h"

#define STRUCT_NAME s
#define STRUCT_FIELDS \
    X(long, l) \
    X(double, g) \
    X(unsigned long long, f) \
    X(int, x)
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

void
struct_print(struct struct_fmt *fmt, void *structure)
{
    printf("%s:\n", fmt->struct_name);
    for (size_t i = 0; i < fmt->num_members; i++) {
        printf("\t%s: %zu %zu =", fmt->names[i], fmt->offsets[i], fmt->sizes[i]);
        print_buffer(((unsigned char*)structure)+fmt->offsets[i], fmt->sizes[i]);
        printf("\n");
    }
}


int
main(int argc, char **argv)
{
    struct t tst = {.a = 5, .b = 0.0f, .c = 'a'};
    struct s sst = {.l = 1024, .g = 0.0, .f = 2048};

    unsigned char tbuff[t_fmt.packed_size];
    struct_pack(&t_fmt, &tst, tbuff);
    printf("t packed:\n\t");
    print_buffer(tbuff, sizeof(tbuff));
    printf("\n");

    unsigned char sbuff[s_fmt.packed_size];
    struct_pack(&s_fmt, &sst, sbuff);
    printf("s packed:\n\t");
    print_buffer(sbuff, sizeof(tbuff));
    printf("\n");

    struct_print(&t_fmt, &tst);
    struct_print(&s_fmt, &sst);

    struct t tst2;
    struct s sst2;

    struct_unpack(&t_fmt, tbuff, &tst2); 
    struct_unpack(&s_fmt, sbuff, &sst2); 

    struct_print(&t_fmt, &tst2);
    struct_print(&s_fmt, &sst2);

    return 0;
}
