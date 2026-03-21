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
