exported := C/dc.h
project := drawctx

dep_libxft := -I/usr/include/freetype2
CFLAGS_PRINT := -std=c11 -pedantic -Wextra -Wall
CFLAGS := ${CFLAGS_PRINT} ${dep_libxft}
