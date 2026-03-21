#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#if !defined(INTEGERS)
#define INTEGERS
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
typedef long long llong;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
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
} StructFormat;

static void
print_buffer(uchar *buffer, size_t size) {
    for (size_t j = 0; j < size; j += 1) {
        printf(" %02x", buffer[j]);
    }
    return;
}

static void
dispatch_print(void *pointer, char *type, char *name, int32 nested);

#define PRIMITIVE_PRINT(TYPE, FMT, CAST) \
    if (strcmp(type, #TYPE) == 0 || strcmp(type, "signed " #TYPE) == 0) { \
        printf(FMT, *(CAST *)pointer); \
        return; \
    }

static void
print_primitive(void *pointer, char *type) {
    PRIMITIVE_PRINT(char, "%c\n", char)
    PRIMITIVE_PRINT(uchar, "%c\n", uchar)
    PRIMITIVE_PRINT(short, "%d\n", short)
    PRIMITIVE_PRINT(ushort, "%u\n", ushort)
    PRIMITIVE_PRINT(int, "%d\n", int)
    PRIMITIVE_PRINT(uint, "%u\n", uint)
    PRIMITIVE_PRINT(long, "%ld\n", long)
    PRIMITIVE_PRINT(ulong, "%lu\n", ulong)
    PRIMITIVE_PRINT(char *, "%s\n", char *)
    PRIMITIVE_PRINT(float, "%f\n", float)
    PRIMITIVE_PRINT(double, "%f\n", double)
    PRIMITIVE_PRINT(long double, "%Lf\n", long double)
    PRIMITIVE_PRINT(int8, "%d\n", int8)
    PRIMITIVE_PRINT(int16, "%d\n", int16)
    PRIMITIVE_PRINT(int32, "%d\n", int32)
    PRIMITIVE_PRINT(int64, "%ld\n", int64)
    PRIMITIVE_PRINT(uint8, "%u\n", uint8)
    PRIMITIVE_PRINT(uint16, "%u\n", uint16)
    PRIMITIVE_PRINT(uint32, "%u\n", uint32)
    PRIMITIVE_PRINT(uint64, "%lu\n", uint64)
    PRIMITIVE_PRINT(void *, "%p\n", void *)

    fprintf(stderr, "Missing printf for type " RED "%s" RESET ".\n", type);
    exit(EXIT_FAILURE);
}
#endif

#if !defined(__INCLUDE_LEVEL__) || (__INCLUDE_LEVEL__ >= 1)
  #if !defined(STRUCT_NAME)
    #error "STRUCT_NAME is not defined."
  #endif
  #if !defined(STRUCT_FIELDS)
    #error "STRUCT_FIELDS is not defined."
  #endif
#endif

#define STR_EXPAND(A) #A
#define STR(A) STR_EXPAND(A)
#define CAT_EXPAND(A, B) A ## B
#define CAT(A, B) CAT_EXPAND(A, B)

typedef struct STRUCT_NAME {
    #define X(L, R) L R;
    STRUCT_FIELDS
    #undef X
} STRUCT_NAME;

static StructFormat CAT(STRUCT_NAME, _fmt) = {
    .struct_name = STR(STRUCT_NAME),
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
};

static void
CAT(STRUCT_NAME, _print)(STRUCT_NAME *structure, char *name, int32 nested) {
    if (structure == NULL) {
        printf("NULL\n");
        return;
    }
    if (nested == 0) {
        printf(GREEN STR(STRUCT_NAME) RESET " %s = ", name);
    }
    printf("{\n");

    #define X(L, R) \
    for (int32 j = 0; j <= nested; j += 1) { \
        printf("\t"); \
    } \
    printf(GREEN #L RESET " " #R " = "); \
    dispatch_print(&structure->R, #L, #R, nested + 1);
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
#undef STR_EXPAND
#undef STR
#undef CAT_EXPAND
#undef CAT
