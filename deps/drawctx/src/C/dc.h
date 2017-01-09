#ifndef _T3_DC_H_
#define _T3_DC_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

typedef struct {
    int x, y, w, h;
    int text_offset_y;
    int color_border_pixels;
    int border_width;
    XSetWindowAttributes wa;
    Display *dpy;
    GC gc;
    Pixmap canvas;
    Pixmap empty;
    XftDraw *xftdraw;
    struct {
        int ascent;
        int descent;
        int height;
        XFontSet set;
        XFontStruct *xfont;
        XftFont *xft_font;
    } font;
} DC;

DC *getdc();

#endif
