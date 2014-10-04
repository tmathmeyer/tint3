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

# MPD depends on libmpdclient, comment if you don't want it
MPDLIBS = -lmpdclient
MPDDEPS = mpd.o
MPDFLAGS= -D_WITH_MPD

#lxcb libraries
LXCBLIBS = -lxcb -lxcb-icccm -lxcb-ewmh

# includes and libs
INCS = -I${X11INC} -I${XFTINC}
LIBS = -L${X11LIB} -lX11 ${XINERAMALIBS} ${XFTLIBS} ${MPDLIBS} ${LXCBLIBS}

# flags
CPPFLAGS = -D_BSD_SOURCE -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS} ${MPDFLAGS}
CFLAGS   = -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS  = -s ${LIBS}

# compiler and linker
CC = gcc
