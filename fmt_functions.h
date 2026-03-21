#include <stdio.h>
#include <string.h>

#if !defined(__INCLUDE_LEVEL__) || (__INCLUDE_LEVEL__ >= 1)
#if !defined(EXPAND_STRUCTS)
#error "EXPAND_STRUCTS is undefined"
#endif
#endif

static void
dispatch_print(void *pointer, int32 type_id, char *type_name, char *field_name, int32 nested) {
    /* If it is a primitive, use the tagged printer instead of string guessing */
    if (type_id != TYPE_OTHER) {
        print_primitive(pointer, type_id);
        return;
    }

#undef STRUCT
#define STRUCT(TYPE) \
    if (strcmp(type_name, #TYPE) == 0) { \
        CAT(TYPE, _print)((TYPE *)pointer, field_name, nested); \
        return; \
    } \
    if (strcmp(type_name, #TYPE " *") == 0) { \
        TYPE **ptr = (TYPE **)pointer; \
        CAT(TYPE, _print)(*ptr, field_name, nested); \
        return; \
    }
    
    EXPAND_STRUCTS
#undef STRUCT

    fprintf(stderr, "Unknown struct type: %s\n", type_name);
    exit(EXIT_FAILURE);
}

#define STRUCT(TYPE) TYPE *: CAT(TYPE, _print)((TYPE *)_v, _n, 0),

#define STRUCT_PRINT(VAR) \
    do { \
        void *_v = (VAR); \
        char *_n = #VAR; \
        _Generic((VAR), \
            EXPAND_STRUCTS \
            default: (void)0 \
        ); \
    } while (0)
