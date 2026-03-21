#include <stdio.h>
#include <string.h>

#if !defined(CAT) || !defined(CAT3)
  #define CAT_(a, b)     a##b
  #define CAT3_(a, b, c) a##b##c
  #define CAT(a, b)      CAT_(a, b)
  #define CAT3(a, b, c)  CAT3_(a, b, c)
#endif

#define NUM_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, n, ...) n
#define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, x)
#define SELECT_ON_NUM_ARGS(macro, ...) \
    CAT(macro, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#if !defined(QUOTE)
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)
#endif

#if !defined(EXPAND_STRUCTS)
#error "EXPAND_STRUCTS is undefined"
#endif

static void
dispatch_print(void *pointer, char *type, char *name, int32 nested) {
#define STRUCT(TYPE) \
    if (strcmp(type, #TYPE) == 0) { \
        CAT(TYPE, _print)((TYPE *)pointer, name, nested); \
        return; \
    } \
    if (strcmp(type, #TYPE " *") == 0) { \
        TYPE **ptr = (TYPE **)pointer; \
        CAT(TYPE, _print)(*ptr, name, nested); \
        return; \
    }
    
    EXPAND_STRUCTS
#undef STRUCT

    print_primitive(pointer, type);
    return;
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
