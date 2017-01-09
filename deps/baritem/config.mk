exported := C/baritem.h
project := baritem

dep_libxft := -I/usr/include/freetype2 # needed for xftutils dep
CFLAGS_PRINT := -std=c11 -pedantic -Wextra -Wall
CFLAGS := ${CFLAGS_PRINT} ${dep_libxft}
