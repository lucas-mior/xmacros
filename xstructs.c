#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "generic.c"

#define xmalloc(x) malloc(x)
#define memcpy64(a, b, c) memcpy(a, b, c)

#if !defined(error2)
#define error2(...) fprintf(stderr, __VA_ARGS__)
#endif

#if defined(__INCLUDE_LEVEL__) && (__INCLUDE_LEVEL__ == 0)
#define TESTING_xstructs 1
#elif !defined(TESTING_xstructs)
#define TESTING_xstructs 0
#endif

#if !defined(INTEGERS)
#define INTEGERS
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
typedef long long llong;
typedef unsigned long long ullong;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef long double ldouble;
#endif

#if !defined(CAT) || !defined(CAT3)
  #define CAT_(a, b)     a##b
  #define CAT3_(a, b, c) a##b##c
  #define CAT(a, b)      CAT_(a, b)
  #define CAT3(a, b, c)  CAT3_(a, b, c)
#endif

#if !defined(QUOTE)
  #define QUOTE_(x) #x
  #define QUOTE(x)  QUOTE_(x)
#endif

#if !defined(SELECT_ON_NUM_ARGS)
  #define NUM_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, n, ...) n
  #define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, x)
  #define SELECT_ON_NUM_ARGS(macro, ...) \
      CAT(macro, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
#endif

#if !defined(XSTRUCT_UTILS)
#define XSTRUCT_UTILS

#define GREEN "\x1b[32m"
#define RED   "\x1b[31m"
#define RESET "\x1b[0m"

typedef struct StructFormat {
    char *struct_name;
    size_t num_members;
    size_t struct_size;
    size_t packed_size;
    size_t *offsets;
    size_t *sizes;
    char **names;
    char **types;
    int32 *type_ids; /* Stores enum Type or TYPE_STRUCT */
} StructFormat;

static void
print_buffer(uchar *buffer, size_t size) {
    for (size_t j = 0; j < size; j += 1) {
        printf(" %02x", buffer[j]);
    }
    return;
}

static void
dispatch_print(void *pointer, int32 type_id, char *type_name, char *name, int32 nested);

static void
print_primitive(void *pointer, int32 type_id) {
    switch (type_id) {
        case TYPE_BOOL:    printf("%s\n", *(bool *)pointer ? "true" : "false"); break;
        case TYPE_CHAR:    printf("'%c'\n", *(char *)pointer); break;
        case TYPE_SCHAR:   printf("%d\n", *(schar *)pointer); break;
        case TYPE_UCHAR:   printf("%u\n", *(uchar *)pointer); break;
        case TYPE_SHORT:   printf("%d\n", *(short *)pointer); break;
        case TYPE_USHORT:  printf("%u\n", *(ushort *)pointer); break;
        case TYPE_INT:     printf("%d\n", *(int *)pointer); break;
        case TYPE_UINT:    printf("%u\n", *(uint *)pointer); break;
        case TYPE_LONG:    printf("%ld\n", *(long *)pointer); break;
        case TYPE_ULONG:   printf("%lu\n", *(ulong *)pointer); break;
        case TYPE_LLONG:   printf("%lld\n", *(llong *)pointer); break;
        case TYPE_ULLONG:  printf("%llu\n", *(ullong *)pointer); break;
        case TYPE_FLOAT:   printf("%f\n", (double)*(float *)pointer); break;
        case TYPE_DOUBLE:  printf("%f\n", *(double *)pointer); break;
        case TYPE_LDOUBLE: printf("%Lf\n", *(ldouble *)pointer); break;
        case TYPE_CHARP:   printf("\"%s\"\n", *(char **)pointer); break;
        case TYPE_VOIDP:   printf("%p\n", *(void **)pointer); break;
        default:
            fprintf(stderr, "Unhandled primitive type ID: %d\n", type_id);
            exit(EXIT_FAILURE);
    }
    return;
}
#endif

#if TESTING_xstructs
#define STRUCT_NAME ExampleStruct
#define STRUCT_FIELDS \
    X(int, x) \
    X(char *, str)
#endif

typedef struct STRUCT_NAME {
    #define X(L, R) L R;
    STRUCT_FIELDS
    #undef X
} STRUCT_NAME;

static StructFormat CAT(STRUCT_NAME, _fmt) = {
    .struct_name = QUOTE(STRUCT_NAME),
    .num_members = (
    #define X(L, R) 1 +
        STRUCT_FIELDS 
    #undef X
    0),
    .struct_size = sizeof(STRUCT_NAME),
    .packed_size = (
    #define X(L, R) sizeof(L) +
        STRUCT_FIELDS 
    #undef X 
    0),
    .offsets = (size_t[]){
    #define X(L, R) offsetof(STRUCT_NAME, R),
        STRUCT_FIELDS 
    #undef X
    },
    .sizes = (size_t[]){
    #define X(L, R) sizeof(L),
        STRUCT_FIELDS 
    #undef X
    },
    .names = (char *[]){
    #define X(L, R) #R,
        STRUCT_FIELDS 
    #undef X
    },
    .types = (char *[]){
    #define X(L, R) #L,
        STRUCT_FIELDS 
    #undef X
    },
    .type_ids = (int32[]){
    #define X(L, R) TYPEID(((STRUCT_NAME*)0)->R),
        STRUCT_FIELDS
    #undef X
    },
};

static void
CAT(STRUCT_NAME, _print)(STRUCT_NAME *structure, char *name, int32 nested) {
    if (structure == NULL) {
        printf("NULL\n");
        return;
    }
    if (nested == 0) {
        printf(GREEN QUOTE(STRUCT_NAME) RESET " %s = ", name);
    }
    printf("{\n");

    #define X(L, R) \
    for (int32 j = 0; j <= nested; j += 1) { \
        printf("\t"); \
    } \
    printf(GREEN #L RESET " " #R " = "); \
    dispatch_print(&structure->R, TYPEID(structure->R), #L, #R, nested + 1);
    STRUCT_FIELDS
    #undef X

    for (int32 j = 0; j < nested; j += 1) {
        printf("\t");
    }
    printf("}\n");

    if (nested == 0) {
        printf("\n");
    }
    return;
}

static size_t
CAT(STRUCT_NAME, _pack)(STRUCT_NAME *structure, uchar *buffer) {
    size_t pos = 0;
    #define X(L, R) \
    memcpy(buffer + pos, &structure->R, sizeof(L)); \
    pos += sizeof(L);
    STRUCT_FIELDS
    #undef X
    return pos;
}

static size_t
CAT(STRUCT_NAME, _unpack)(uchar *buffer, STRUCT_NAME *structure) {
    size_t pos = 0;
    #define X(L, R) \
    memcpy(&structure->R, buffer + pos, sizeof(L)); \
    pos += sizeof(L);
    STRUCT_FIELDS
    #undef X
    return pos;
}

#undef STRUCT_FIELDS
#undef STRUCT_NAME

#if TESTING_xstructs
#include "assert.c"
#define EXPAND_STRUCTS \
    STRUCT(ExampleStruct)
#include "fmt_functions.h"

int main(void) {
    ExampleStruct original;
    ExampleStruct restored;
    uchar *buffer;
    size_t packed_size;

    original.x = 1337;
    original.str = "Metaprogramming logic";

    ASSERT(strcmp(ExampleStruct_fmt.struct_name, "ExampleStruct") == 0);
    ASSERT(ExampleStruct_fmt.num_members == 2);
    ASSERT(ExampleStruct_fmt.struct_size == sizeof(ExampleStruct));
    ASSERT(ExampleStruct_fmt.packed_size == (sizeof(int) + sizeof(char *)));

    if ((buffer = malloc(ExampleStruct_fmt.packed_size)) == NULL) {
        return EXIT_FAILURE;
    }

    packed_size = ExampleStruct_pack(&original, buffer);
    ASSERT(packed_size == ExampleStruct_fmt.packed_size);

    ExampleStruct_unpack(buffer, &restored);
    ASSERT(restored.x == 1337);
    ASSERT(strcmp(restored.str, "Metaprogramming logic") == 0);

    STRUCT_PRINT(&original);

    free(buffer);
    return EXIT_SUCCESS;
}

#endif
