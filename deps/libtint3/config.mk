exported := C/tint3_main.h C/tint3.h C/vdesk.h C/draw.h C/dlist.h
project := libtint3
link_libs := 

dep_libx11 := -I/usr/X11R6/include
dep_libxft := -I/usr/include/freetype2
dep_libnotify := -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

CFLAGS_PRINT := -std=c11 -pedantic -Wextra -Wall
CFLAGS := ${CFLAGS_PRINT} ${dep_libnotify} ${dep_libx11} ${dep_libxft}

