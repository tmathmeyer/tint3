#ifndef _T3_TINT3_H_
#define _T3_TINT3_H_

#include <X11/Xft/Xft.h>

//=========================//
// structs //
//=========================//

typedef struct ColorSet{
    unsigned long FG;
    XftColor FG_xft;
    unsigned long BG;
} ColorSet;

typedef struct baritem{
    ColorSet * color;
    ColorSet * invert;
    unsigned int length;
    unsigned int xstart;
    unsigned char inverted;
    char * string;
    char * format;
    char * source;
    char * (* update)(struct baritem *);
} baritem;

typedef struct itemlist {
    uint length;
    baritem ** buffer;
} itemlist;

typedef struct bar_layout {
    itemlist * left;
    short leftlen;
    itemlist * right;
    short rightlen;
    itemlist * center;
    short centerlen;
} bar_layout;

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




//=========================//
// global access variables //
//=========================//

// maybe make configurable?
extern const char *font;

// definitely make configurable
extern int topbar;

// the root window of the system
extern Window root;

// the window that the bar occupies
extern Window win;




//=========================//
// global access functions //
//=========================//

// redraw the bar (why did I call it menu?)
void drawmenu(void);

// get a property 
int get_x11_property(Atom at, Atom type);

// get an item by X
baritem *item_by_coord(unsigned int x);




//================//
// cardinals used //
//================//

Atom NET_NUMBER_DESKTOPS,
     NET_CURRENT_DESKTOP;

Atom _CARDINAL_;

#endif