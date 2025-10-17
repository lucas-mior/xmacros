#!/bin/sh

ctags --kinds-C=+l+d ./*.h ./*.c 2> /dev/null || true
vtags.sed tags > .tags.vim 2> /dev/null || true

CC="${CC:-cc}"

$CC $CFLAGS main.c -o ./xmacros && ./xmacros
