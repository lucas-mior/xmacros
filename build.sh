#!/bin/sh

ctags --kinds-C=+l+d ./*.h ./*.c 2> /dev/null || true
vtags.sed tags > .tags.vim 2> /dev/null || true

CC="${CC:-cc}"
CC=clang

CFLAGS="$CFLAGS -D_DEFAULT_SOURCE"

CFLAGS="$CFLAGS -Wall -Wextra"
if [ $CC = "clang" ]; then
    CFLAGS="$CFLAGS -Weverything"
    CFLAGS="$CFLAGS -Wno-unsafe-buffer-usage"
    CFLAGS="$CFLAGS -Wno-format-nonliteral"
    CFLAGS="$CFLAGS -Wno-unused-function"
    CFLAGS="$CFLAGS -Wno-pre-c11-compat"
    CFLAGS="$CFLAGS -Wno-double-promotion"
    CFLAGS="$CFLAGS -Wno-constant-logical-operand"
    CFLAGS="$CFLAGS -Wno-padded"
fi

$CC $CFLAGS main.c -o ./xmacros
