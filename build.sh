#!/bin/sh

ctags --kinds-C=+l+d ./*.h ./*.c 2> /dev/null || true
vtags.sed tags > .tags.vim 2> /dev/null || true

CC="${CC:-cc}"

CFLAGS="$CFLAGS -D_DEFAULT_SOURCE"

CFLAGS="$CFLAGS -Wall -Wextra"
CFLAGS="$CFLAGS -Wfatal-errors"
CFLAGS="$CFLAGS -Wno-unused-function"
CFLAGS="$CFLAGS -Wno-unused-variable"
CFLAGS="$CFLAGS -Wno-unused-macros"
CFLAGS="$CFLAGS -Werror"

if [ $CC = "clang" ]; then
    CFLAGS="$CFLAGS -Weverything"
    CFLAGS="$CFLAGS -Wno-unsafe-buffer-usage"
    CFLAGS="$CFLAGS -Wno-format-nonliteral"
    CFLAGS="$CFLAGS -Wno-unused-function"
    CFLAGS="$CFLAGS -Wno-pre-c11-compat"
    CFLAGS="$CFLAGS -Wno-double-promotion"
    CFLAGS="$CFLAGS -Wno-constant-logical-operand"
    CFLAGS="$CFLAGS -Wno-padded"
    CFLAGS="$CFLAGS -Wno-implicit-void-ptr-cast"
    CFLAGS="$CFLAGS -Wno-covered-switch-default"
    CFLAGS="$CFLAGS -Wno-implicit-int-enum-cast"
    CFLAGS="$CFLAGS -Wno-c++-keyword"
    CFLAGS="$CFLAGS -Wno-float-equal"

    # there is a portable (slower to compile)
    # callback when not using gcc nor clang, see generic.c
    CFLAGS="$CFLAGS -Wno-gnu-union-cast"

    # this is needed to work with bit flags (enum values are powers of 2)
    CFLAGS="$CFLAGS -Wno-assign-enum"
fi

$CC $CFLAGS main.c -o ./xmacros
