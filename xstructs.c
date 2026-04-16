/*
 * Copyright (C) 2025 Mior, Lucas;
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the*License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#include "generic.c"
#include "util.c"

#if !defined(error2)
#define error2(...) fprintf(stderr, __VA_ARGS__)
#endif

#if defined(__INCLUDE_LEVEL__) && (__INCLUDE_LEVEL__ == 0)
#define TESTING_xstructs 1
#elif !defined(TESTING_xstructs)
#define TESTING_xstructs 0
#endif

#if !defined(XSTRUCT_UTILS)
#define XSTRUCT_UTILS

typedef struct StructFormat {
    char *struct_name;
    int32 num_members;
    int32 struct_size;
    int32 packed_size;
    int32 *offsets;
    int32 *sizes;
    int32 *array_lens;
    char **names;
    char **types;
    enum Type *type_ids;
} StructFormat;

static void
print_buffer(uchar *buffer, int64 size) {
    for (int64 j = 0; j < size; j += 1) {
        printf(" %02x", buffer[j]);
    }
    return;
}

static void
dispatch_print(void *pointer, int32 type_id, char *type_name, char *name, int32 nested);

static void
print_primitive(void *pointer, int32 type_id) {
    switch (type_id) {
    case TYPE_BOOL:    printf("%s\n",     *(bool *)pointer ? "true" : "false"); break;
    case TYPE_CHAR:    printf("'%c'\n",   *(char *)pointer);                    break;
    case TYPE_SCHAR:   printf("%d\n",     *(schar *)pointer);                   break;
    case TYPE_UCHAR:   printf("%u\n",     *(uchar *)pointer);                   break;
    case TYPE_SHORT:   printf("%d\n",     *(short *)pointer);                   break;
    case TYPE_USHORT:  printf("%u\n",     *(ushort *)pointer);                  break;
    case TYPE_INT:     printf("%d\n",     *(int *)pointer);                     break;
    case TYPE_UINT:    printf("%u\n",     *(uint *)pointer);                    break;
    case TYPE_LONG:    printf("%ld\n",    *(long *)pointer);                    break;
    case TYPE_ULONG:   printf("%lu\n",    *(ulong *)pointer);                   break;
    case TYPE_LLONG:   printf("%lld\n",   *(llong *)pointer);                   break;
    case TYPE_ULLONG:  printf("%llu\n",   *(ullong *)pointer);                  break;
    case TYPE_FLOAT:   printf("%f\n",     (double)*(float *)pointer);           break;
    case TYPE_DOUBLE:  printf("%f\n",     *(double *)pointer);                  break;
    case TYPE_LDOUBLE: printf("%Lf\n",    *(ldouble *)pointer);                 break;
    case TYPE_CHARP:   printf("\"%s\"\n", *(char **)pointer);                   break;
    case TYPE_VOIDP:   printf("%p\n",     *(void **)pointer);                   break;
    default:
        fprintf(stderr, "Unhandled primitive type ID: %d\n", type_id);
        exit(EXIT_FAILURE);
    }
    return;
}
#endif

#if TESTING_xstructs
#define STRUCT_NAME ExampleStruct
#define STRUCT_FIELDS         \
    X(char, ic)               \
    X(uchar, uc)              \
    X(signed short, is)       \
    X(ushort, us)             \
    X(int, ii)                \
    X(uint, ui)               \
    X(long, il)               \
    X(ulong, ul)              \
    X(float, f, 10)           \
    X(double, d)              \
    X(long double, ld)
#endif

#define X_STRUCT_2(L, R)    L R;
#define X_STRUCT_3(L, R, N) L R[N];

#define X_OFF_2(L, R)     offsetof(STRUCT_NAME, R),
#define X_OFF_3(L, R, N)  offsetof(STRUCT_NAME, R),

#define X_SIZE_2(L, R)      sizeof(((STRUCT_NAME*)0)->R),
#define X_SIZE_3(L, R, N)   sizeof(((STRUCT_NAME*)0)->R),

#define X_ALEN_2(L, R)      0,
#define X_ALEN_3(L, R, N)   N,

#define X_NAME_2(L, R)      #R,
#define X_NAME_3(L, R, N)   #R,

#define X_TYPE_2(L, R)      #L,
#define X_TYPE_3(L, R, N)   #L,

#define X_TID_2(L, R)       TYPEID(((STRUCT_NAME*)0)->R),
#define X_TID_3(L, R, N)    TYPEID(((STRUCT_NAME*)0)->R[0]),

typedef struct STRUCT_NAME {
    #define X(...) SELECT_ON_NUM_ARGS(X_STRUCT_, __VA_ARGS__)
    STRUCT_FIELDS
    #undef X
} STRUCT_NAME;

static StructFormat CAT(STRUCT_NAME, _fmt) = {
    .struct_name = QUOTE(STRUCT_NAME),
    .num_members = (
        #define X_C2(L, R) 1 +
        #define X_C3(L, R, N) 1 +
        #define X(...) SELECT_ON_NUM_ARGS(X_C, __VA_ARGS__)
        STRUCT_FIELDS
        #undef X
        #undef X_C2
        #undef X_C3
        0),
    .struct_size = sizeof(STRUCT_NAME),
    .packed_size = (
        #define X_P2(L, R) sizeof(((STRUCT_NAME*)0)->R) +
        #define X_P3(L, R, N) sizeof(((STRUCT_NAME*)0)->R) +
        #define X(...) SELECT_ON_NUM_ARGS(X_P, __VA_ARGS__)
        STRUCT_FIELDS
        #undef X
        #undef X_P2
        #undef X_P3
        0),
    .offsets = (int32[]){
        #define X(...) SELECT_ON_NUM_ARGS(X_OFF_, __VA_ARGS__)
        STRUCT_FIELDS
        #undef X
    },
    .sizes = (int32[]){
        #define X(...) SELECT_ON_NUM_ARGS(X_SIZE_, __VA_ARGS__)
        STRUCT_FIELDS
        #undef X
    },
    .array_lens = (int32[]){
        #define X(...) SELECT_ON_NUM_ARGS(X_ALEN_, __VA_ARGS__)
        STRUCT_FIELDS
        #undef X
    },
    .names = (char *[]){
        #define X(...) SELECT_ON_NUM_ARGS(X_NAME_, __VA_ARGS__)
        STRUCT_FIELDS
        #undef X
    },
    .types = (char *[]){
        #define X(...) SELECT_ON_NUM_ARGS(X_TYPE_, __VA_ARGS__)
        STRUCT_FIELDS
        #undef X
    },
    .type_ids = (enum Type []){
        #define X(...) SELECT_ON_NUM_ARGS(X_TID_, __VA_ARGS__)
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
        printf(GREEN(QUOTE(STRUCT_NAME))" %s = ", name);
    }
    printf("{\n");

    #define X_PR2(L, R) \
        for (int32 j = 0; j <= nested; j += 1) { \
            printf("\t"); \
        } \
        printf(GREEN(#L)" " #R " = "); \
        dispatch_print(&structure->R, TYPEID(structure->R), #L, #R, nested + 1);

    #define X_PR3(L, R, N) \
        for (int32 i = 0; i < N; i += 1) { \
            for (int32 j = 0; j <= nested; j += 1) { \
                printf("\t"); \
            } \
            char buf[128]; \
            snprintf(buf, sizeof(buf), "%s[%d]", #R, i); \
            printf(GREEN(#L)" " #R "[%d] = ", i); \
            dispatch_print(&structure->R[i], TYPEID(structure->R[0]), #L, buf, nested + 1); \
        }

    #define X(...) SELECT_ON_NUM_ARGS(X_PR, __VA_ARGS__)
    STRUCT_FIELDS
    #undef X
    #undef X_PR2
    #undef X_PR3

    for (int32 j = 0; j < nested; j += 1) {
        printf("\t");
    }
    printf("}\n");

    if (nested == 0) {
        printf("\n");
    }
    return;
}

static int64
CAT(STRUCT_NAME, _pack)(STRUCT_NAME *structure, uchar *buffer) {
    int64 pos = 0;

    #define X_PK2(L, R) \
        memcpy64(buffer + pos, &structure->R, sizeof(structure->R)); \
        pos += sizeof(structure->R);
    #define X_PK3(L, R, N) \
        memcpy64(buffer + pos, structure->R, sizeof(structure->R)); \
        pos += sizeof(structure->R);
    #define X(...) SELECT_ON_NUM_ARGS(X_PK, __VA_ARGS__)

    STRUCT_FIELDS

    #undef X
    #undef X_PK2
    #undef X_PK3
    return pos;
}

static int64
CAT(STRUCT_NAME, _unpack)(uchar *buffer, STRUCT_NAME *structure) {
    int64 pos = 0;

    #define X_UP2(L, R) \
        memcpy64(&structure->R, buffer + pos, sizeof(structure->R)); \
        pos += sizeof(structure->R);
    #define X_UP3(L, R, N) \
        memcpy64(structure->R, buffer + pos, sizeof(structure->R)); \
        pos += sizeof(structure->R);
    #define X(...) SELECT_ON_NUM_ARGS(X_UP, __VA_ARGS__)

    STRUCT_FIELDS

    #undef X
    #undef X_UP2
    #undef X_UP3
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
    int64 packed_size;

    original.ic = 'c';
    original.uc = 'd';
    original.is = SHRT_MAX;
    original.us = USHRT_MAX;
    original.ii = INT_MAX;
    original.ui = UINT_MAX;
    original.il = LONG_MAX;
    original.ul = ULONG_MAX;
    for (int32 i = 0; i < LENGTH(original.f); i += 1) {
        original.f[i] = 0.5f;
    }
    original.d = 0.5;
    original.ld = 0.5;

    ASSERT_EQUAL(ExampleStruct_fmt.struct_name, "ExampleStruct");

    buffer = xmalloc(ExampleStruct_fmt.packed_size);
    packed_size = ExampleStruct_pack(&original, buffer);
    ASSERT(packed_size == ExampleStruct_fmt.packed_size);

    ExampleStruct_unpack(buffer, &restored);

    STRUCT_PRINT(&original);
    STRUCT_PRINT(&restored);

    ASSERT_EQUAL(original.ic, restored.ic);
    ASSERT_EQUAL(original.uc, restored.uc);
    ASSERT_EQUAL(original.is, restored.is);
    ASSERT_EQUAL(original.us, restored.us);
    ASSERT_EQUAL(original.ii, restored.ii);
    ASSERT_EQUAL(original.ui, restored.ui);
    ASSERT_EQUAL(original.il, restored.il);
    ASSERT_EQUAL(original.ul, restored.ul);
    ASSERT_EQUAL(original.d,  restored.d);
    ASSERT_EQUAL(original.ld, restored.ld);

    free2(buffer, ExampleStruct_fmt.packed_size);

    exit(EXIT_SUCCESS);
}
#endif
