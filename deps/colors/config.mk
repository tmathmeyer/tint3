exported := C/colors.h
project := colors

dep_libxft := -I/usr/include/freetype2
CFLAGS_PRINT := -std=c11 -pedantic -Wextra -Wall
CFLAGS := ${CFLAGS_PRINT} ${dep_libxft}
