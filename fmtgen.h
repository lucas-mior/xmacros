#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(INTEGERS)
#define INTEGERS
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef size_t usize;
typedef ssize_t isize;
#endif

#if __INCLUDE_LEVEL__ && !defined(STRUCT_FMT_DEF)
#define STRUCT_FMT_DEF
struct struct_fmt {
    char const *struct_name;
    size_t num_members;
    size_t struct_size;
    size_t packed_size;
    size_t *offsets;
    size_t *sizes;
    char const **names;
    char const **types;
};
#endif

#if __INCLUDE_LEVEL__ == 0
#define STRUCT_NAME SmallStruct
#define STRUCT_FIELDS \
    X(char *, string) \
    X(int, i)
#endif

#if !defined(STRUCT_NAME)
    #error "Did not define STRUCT_NAME before including fmtgen.h"
#endif

#if !defined(STRUCT_FIELDS)
    #error "Did not define STRUCT_FIELDS before including fmtgen.h"
#endif

#define STR_NOEXPAND(A) #A
#define STR(A) STR_NOEXPAND(A)

#define CAT_NOEXPAND(A, B) A ## B
#define CAT(A, B) CAT_NOEXPAND(A, B)

typedef struct STRUCT_NAME {
    #define X(L, R) L R;
    STRUCT_FIELDS
    #undef X
} STRUCT_NAME;

static struct struct_fmt CAT(STRUCT_NAME, _fmt) = {

    .struct_name = STR(STRUCT_NAME),

    .num_members = (
        #define X(L, R) 1 +
        STRUCT_FIELDS
        #undef X
    0),

    .struct_size = sizeof(struct STRUCT_NAME),

    .packed_size = (
        #define X(L, R) sizeof(L) +
        STRUCT_FIELDS
        #undef X
    0),

    .offsets = (size_t[]){
        #define X(L, R) offsetof(struct STRUCT_NAME, R),
        STRUCT_FIELDS
        #undef X
    },

    .sizes = (size_t []){
        #define X(L, R) sizeof(L),
        STRUCT_FIELDS
        #undef X
    },

    .names = (char const *[]){
        #define X(L, R) #R,
        STRUCT_FIELDS
        #undef X
    },

    .types = (char const *[]){
        #define X(L, R) #L,
        STRUCT_FIELDS
        #undef X
    },
};

#undef STRUCT_FIELDS
#undef STRUCT_NAME
#undef STR_NOEXPAND
#undef STR
#undef CAT_NOEXPAND
#undef CAT
