VERSION = 4.2.1


# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/X11R6/include
X11LIB = /usr/X11R6/lib

# Xinerama, comment if you don't want it
XINERAMALIBS  = -lXinerama
XINERAMAFLAGS = -DXINERAMA

# Xft, comment if you don't want it
XFTINC = /usr/include/freetype2
XFTLIBS  = -lXft -lXrender -lfreetype -lz -lfontconfig

#lxcb libraries
LXCBLIBS = -lxcb -lxcb-icccm -lxcb-ewmh

#libnotify
LIBNOT = -lnotify -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0
INCNOT = -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include





# TO COMPILE WITHOUT LIBNOTIFY SUPPORT
#    1. remove ${INCNOT} from INCS
#    2. remove ${LIBNOT} from LIBS
#    3. remove -DLIBNOTIFY from CPPFLAGS
# includes and libs
INCS = -I${X11INC} -I${XFTINC} ${INCNOT}
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${XFTLIBS} ${LXCBLIBS} ${LIBNOT} -lpthread -ldl

# FLAGS: C-Pre-Processor, Compile Flags, Linked Flags
CPPFLAGS = -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS} -DLIBNOTIFY
CFLAGS   = -std=c11 -pedantic -rdynamic -Wextra -Wall ${INCS} ${CPPFLAGS} -g
LDFLAGS  = ${LIBS}

# compiler and linker
CC = gcc
