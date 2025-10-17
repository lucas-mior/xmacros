#ifndef EXPAND_STRUCTS
#error "EXPAND_STRUCTS is undefined"
#endif

void
struct_print(struct struct_fmt *fmt, const char *name, void *structure, int nested)
{
#define PRIMITIVE(TYPE, fmt) \
if (!strcmp(type, #TYPE)) {  \
    printf(fmt, *(TYPE *) pointer);  \
    continue;  \
}
#define STRUCT(TYPE) \
if (!strcmp(type, #TYPE)) {  \
    printf("{"); \
    struct_print(&TYPE##_fmt, fmt->names[i], pointer, ++nested);  \
    if (nested) { \
        nested -= 1;  \
    } \
    for (int j = 0; j < nested; j += 1) \
        printf("\t"); \
    printf("}"); \
    continue;  \
}  \
if(!strcmp(type, #TYPE " *")) {  \
    TYPE **tmp = pointer;  \
    printf("{"); \
    struct_print(&TYPE##_fmt, fmt->names[i], *tmp, ++nested);  \
    if (nested) { \
        nested -= 1;  \
    } \
    for (int j = 0; j < nested; j += 1) \
        printf("\t"); \
    printf("}"); \
    continue;  \
}

#define GREEN "\x1b[32m"
#define RED "\x1b[31m"
#define RESET "\x1b[0m"

    if (!nested)
        printf(GREEN"%s"RESET" %s:", fmt->struct_name, name);
    printf("\n{");

    for (size_t i = 0; i < fmt->num_members; i++) {
        const char *type = fmt->types[i];
        void *pointer = ((unsigned char*)structure)+fmt->offsets[i];
        for (int j = 0; j < nested; j += 1)
            printf("\t");
        printf("\t "GREEN"%s"RESET" %s = ", fmt->types[i], fmt->names[i]);

        PRIMITIVE(char, "%c\n");
        PRIMITIVE(uchar, "%c\n");
        PRIMITIVE(short, "%d\n");
        PRIMITIVE(ushort, "%u\n");
        PRIMITIVE(int, "%d\n");
        PRIMITIVE(uint, "%u\n");
        PRIMITIVE(long, "%ld\n");
        PRIMITIVE(ulong, "%lu\n");
        PRIMITIVE(char *, "%s\n");
        PRIMITIVE(float, "%f\n");
        PRIMITIVE(double, "%f\n");
        PRIMITIVE(int8, "%d\n");
        PRIMITIVE(int16, "%d\n");
        PRIMITIVE(int32, "%d\n");
        PRIMITIVE(int64, "%ld\n");
        PRIMITIVE(uint8, "%u\n");
        PRIMITIVE(uint16, "%u\n");
        PRIMITIVE(uint32, "%u\n");
        PRIMITIVE(uint64, "%lu\n");
        PRIMITIVE(usize, "%zu\n");
        PRIMITIVE(isize, "%zu\n");
        PRIMITIVE(void *, "%p\n");

        EXPAND_STRUCTS

        printf("\n");
        error("Missing printf for type "RED"%s"RESET".\n", type);
        exit(EXIT_FAILURE);

#undef PRIMITIVE
#undef STRUCT
        /* print_buffer(((unsigned char*)structure)+fmt->offsets[i], fmt->sizes[i]); */
    }
    printf("}\n");
}

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

