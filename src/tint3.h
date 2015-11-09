#ifndef _T3_TINT3_H_
#define _T3_TINT3_H_

#include <X11/Xft/Xft.h>
#include "dlist.h"

//=========================//
// structs //
//=========================//

typedef struct ColorSet{
    unsigned long FG;
    unsigned long BG;
    XftColor FG_xft;
} ColorSet;

#define UNDERLINE_TEXT 0x01
typedef struct stylized_text_element {
    unsigned long attributes;
    char *text;
    ColorSet *color;
    unsigned int length;
} text_element;

typedef struct colored_graph_element {
    int *xys;
    unsigned int xy_count;
    ColorSet *color;
} graph_element;

typedef struct generic_element {
    union {
        graph_element *graph;
        text_element *text;
    };
    unsigned int opt;
    unsigned int length;
} element;

typedef struct baritem{
    //recalculated often
    unsigned int length;
    unsigned int xstart;
    
    // text elements
    dlist *elements;

    // brought in from config file
    ColorSet *default_colors;
    dlist *options;
    char *format;
    char *source;
    char *shell;
    
    // update listeners
    dlist *(* update)(struct baritem *);
    void (* click)(struct baritem *, int xpos);
} baritem;

typedef struct bar_layout {
    dlist *left;
    short leftlen;
    dlist *right;
    short rightlen;
    dlist *center;
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
void update_nba(baritem *item);

// get a property 
int get_x11_cardinal_property(Atom at, Atom type);
dlist *get_x11_cpp_property(Atom at);

// get an item by X
baritem *item_by_coord(unsigned int x);

char *get_baritem_option(char *opt_name, baritem* item);

ColorSet *make_baritem_colours(char *fg, char *bg);
void free_stylized(void *ste_v);

//================//
// cardinals used //
//================//

Atom NET_NUMBER_DESKTOPS
    ,NET_CURRENT_DESKTOP
    ,NET_DESKTOP_NAMES
    ;

Atom _CARDINAL_
    ,_UTF8_STRING_
    ;

#endif
