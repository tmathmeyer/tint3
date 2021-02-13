#ifndef _T3_TINT3_H_
#define _T3_TINT3_H_

#include <X11/Xft/Xft.h>
#include "dlist/dlist.h"
#include "baritem/baritem.h"
#include "xftutils/colorset.h"
#include "drawctx/dc.h"

#define is_digit(x) ((x)<='9' && (x)>='0')


#define UNDERLINE_TEXT 0x01
typedef struct bar_layout {
    dlist *left;
    short leftlen;
    dlist *right;
    short rightlen;
    dlist *center;
    short centerlen;
} bar_layout;


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
void shell_exec(baritem *item, int xpos);
void set_timeout(baritem *ipl);

// get a property 
int get_x11_cardinal_property(Atom at, Atom type);
dlist *get_x11_cpp_property(Atom at);

// get an item by X
baritem *item_by_coord(unsigned int x);

ColorSet *make_baritem_colours(char *fg, char *bg);
void free_stylized(void *ste_v);
void setup(void);
void set_dc(DC *);
void set_root(Window w);
void drawlock(void);
void drawunlock(void);

//================//
// cardinals used //
//================//

extern Atom NET_NUMBER_DESKTOPS
    ,NET_CURRENT_DESKTOP
    ,NET_DESKTOP_NAMES
    ;

extern Atom _CARDINAL_
    ,_UTF8_STRING_
    ;

#endif
