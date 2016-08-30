exported := 
project := tint3_tests


link_libnodify := -lnotify -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
link_libx11 := -lX11 -lXinerama -lXft -lXrender
link_libfreetype := -lfreetype -lz -lfontconfig
link_libxcb := -lxcb -lxcb-icccm -lxcb-ewmh
link_libpthread := -lpthread
link_libdl := -ldl
link_libs := ${link_libnotify} ${link_libx11} ${link_libfreetype} ${link_libxcb} ${link_libpthread} ${link_libdl}

dep_libx11 := -I/usr/X11R6/include
dep_libxft := -I/usr/include/freetype2
dep_libnotify := -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include

CFLAGS_PRINT := -std=c11 -pedantic -Wextra -Wall
CFLAGS := ${CFLAGS_PRINT} ${dep_libnotify} ${dep_libx11} ${dep_libxft}
