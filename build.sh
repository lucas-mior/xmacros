#!/bin/sh

dir="$(readlink -f "$(dirname "$0")")"
cbase="cbase"
CPPFLAGS="$CPPFLAGS -I "$dir/$cbase""

ctags --kinds-C=+l+d ./*.h ./*.c 2> /dev/null || true
vtags.sed tags > .tags.vim 2> /dev/null || true

CC="${CC:-cc}"

CFLAGS="$CFLAGS -D_DEFAULT_SOURCE -D_XOPEN_SOURCE=700"

CFLAGS="$CFLAGS -Wall -Wextra"
CFLAGS="$CFLAGS -Wfatal-errors"
CFLAGS="$CFLAGS -Wno-unused-variable"
CFLAGS="$CFLAGS -Wno-unused-macros"
CFLAGS="$CFLAGS -Wno-unused-function"
CFLAGS="$CFLAGS -Werror"

if [ $CC = "clang" ]; then
    CFLAGS="$CFLAGS -Weverything"
    CFLAGS="$CFLAGS -Wno-unsafe-buffer-usage"
    CFLAGS="$CFLAGS -Wno-format-nonliteral"
    CFLAGS="$CFLAGS -Wno-pre-c11-compat"
    CFLAGS="$CFLAGS -Wno-double-promotion"
    CFLAGS="$CFLAGS -Wno-constant-logical-operand"
    CFLAGS="$CFLAGS -Wno-padded"
    CFLAGS="$CFLAGS -Wno-implicit-void-ptr-cast"
    CFLAGS="$CFLAGS -Wno-covered-switch-default"
    CFLAGS="$CFLAGS -Wno-implicit-int-enum-cast"
    CFLAGS="$CFLAGS -Wno-c++-keyword"
    CFLAGS="$CFLAGS -Wno-float-equal"
    CFLAGS="$CFLAGS -Wno-declaration-after-statement"
    CFLAGS="$CFLAGS -Wno-cast-qual"
    CFLAGS="$CFLAGS -Wno-c23-extensions"
    CFLAGS="$CFLAGS -Wno-cast-function-type-strict"

    # there is a portable (slower to compile)
    # callback when not using gcc nor clang, see generic.c
    CFLAGS="$CFLAGS -Wno-gnu-union-cast"

    # this is needed to work with bit flags (enum values are powers of 2)
    CFLAGS="$CFLAGS -Wno-assign-enum"
fi

set -x
target="${1:-build}"
case $target in
"build")
    $CC $CPPFLAGS $CFLAGS -O2 -flto main.c -lm -o ./xmacros
    ;;
"debug")
    $CC $CPPFLAGS $CFLAGS -g3 -DDEBUGGING=1 main.c -lm -o ./xmacros
    ;;
"test")
    ;;
"check")
    CC=gcc CFLAGS="-fanalyzer" ./build.sh

    CFLAGS="--analyze -Xanalyzer -analyzer-output=text"
    CFLAGS="$CFLAGS -Xanalyzer -analyzer-werror"
    CFLAGS="$CFLAGS -Xanalyzer -analyzer-opt-analyze-headers"
    CFLAGS="$CFLAGS -Wno-unused-command-line-argument"
    CC=clang CFLAGS="$CFLAGS" ./build.sh
    ;;
esac
