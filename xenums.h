#ifndef XENUM_UTILS
#define XENUM_UTILS

#ifndef CAT
#define CAT_(a, b) a##b
#define CAT(a, b)  CAT_(a, b)
#endif

#ifndef QUOTE
#define QUOTE_(x) #x
#define QUOTE(x)  QUOTE_(x)
#endif

#define NUM_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, n, ...) n
#define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, x)
#define SELECT_ON_NUM_ARGS(macro, ...) CAT(macro, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)

#endif

#if !defined(ENUM_NAME) || !defined(ENUM_PREFIX_) || !defined(ENUM_FIELDS)
#error "xenums.h requires ENUM_NAME, ENUM_PREFIX_, and ENUM_FIELDS defined."
#endif

enum ENUM_NAME {
#if !defined(ENUM_IS_FLAGS)
    #define XENUM_DEF_1(e)    CAT(ENUM_PREFIX_, e),
#endif
    #define XENUM_DEF_2(e, v) CAT(ENUM_PREFIX_, e) = v,
    #define X(...)            SELECT_ON_NUM_ARGS(XENUM_DEF_, __VA_ARGS__)
    
    ENUM_FIELDS

    #undef X
    #undef XENUM_DEF_1
    #undef XENUM_DEF_2
    CAT(ENUM_PREFIX_, LAST)
};

static char *
CAT(ENUM_PREFIX_, str)(enum ENUM_NAME val) {
#if !defined(ENUM_IS_FLAGS)
    switch (val) {
        #define XENUM_ST_1(e)    case CAT(ENUM_PREFIX_, e): return QUOTE(ENUM_PREFIX_) #e;
        #define XENUM_ST_2(e, v) case CAT(ENUM_PREFIX_, e): return QUOTE(ENUM_PREFIX_) #e;
        #define X(...)           SELECT_ON_NUM_ARGS(XENUM_ST_, __VA_ARGS__)
        
        ENUM_FIELDS

        #undef X
        #undef XENUM_ST_1
        #undef XENUM_ST_2
        case CAT(ENUM_PREFIX_, LAST):
            return QUOTE(ENUM_PREFIX_) "LAST";
        default:
            return "Unknown value";
    }
#else
    char buffer[4096];
    char *buffer_ptr = buffer;
    char *buffer_end = buffer + sizeof(buffer);
    int32 is_first = 1;
    int64 final_len;
    char *copy;

    #define XENUM_FL_1(e)
    #define XENUM_FL_2(e, v) \
        if (val & CAT(ENUM_PREFIX_, e)) { \
            char *name = QUOTE(ENUM_PREFIX_) #e; \
            int32 len = (int32)strlen32(name); \
            if (is_first == 0) { \
                *buffer_ptr = '|'; \
                buffer_ptr += 1; \
            } \
            if (buffer_ptr + len < (buffer_end - 1)) { \
                memcpy(buffer_ptr, name, len); \
                buffer_ptr += len; \
            } \
            is_first = 0; \
        }
    #define X(...) SELECT_ON_NUM_ARGS(XENUM_FL_, __VA_ARGS__)

    ENUM_FIELDS

    #undef X
    #undef XENUM_FL_1
    #undef XENUM_FL_2

    if (buffer_ptr == buffer) {
        return "NONE";
    }

    *buffer_ptr = '\0';
    final_len = (int64)(buffer_ptr - buffer) + 1;
    copy = xmalloc(final_len);
    memcpy(copy, buffer, final_len);

    return copy;
#endif
}

#undef ENUM_NAME
#undef ENUM_PREFIX_
#undef ENUM_FIELDS
#undef ENUM_IS_FLAGS
