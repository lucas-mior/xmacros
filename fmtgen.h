#ifndef STRUCT_FMT_DEF
    /* One time only definitons */
    #define STRUCT_FMT_DEF
    struct struct_fmt {
        char const *struct_name;
        size_t num_members;
        size_t struct_size;
        size_t packed_size;
        size_t *offsets;
        size_t *sizes;
        char const **names;
    };
#endif

/* Error Checking */

#ifndef STRUCT_NAME
    #error "Did not define STRUCT_NAME before including fmtgen.h"
#endif

#ifndef STRUCT_FIELDS
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

struct struct_fmt CAT(STRUCT_NAME, _fmt) = {

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
        #define X(L, R) #L ":" #R,
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
