exported := C/dlist.h
project := dlist

CFLAGS_PRINT := -std=c11 -pedantic -Wextra -Wall
#CFLAGS := ${CFLAGS_PRINT} ${dep_libnotify} ${dep_libx11} ${dep_libxft}
CFLAGS := ${CFLAGS_PRINT}
