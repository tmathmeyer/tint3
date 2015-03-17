#ifndef _T3_TINT3_H_
#define _T3_TINT3_H_

//=========================//
// global access variables //
//=========================//


// maybe make configurable?
extern const char *font;

// definitely make configurable
extern int topbar;

// the root window of the system
extern Window root;







//=========================//
// global access functions //
//=========================//

// redraw the bar (why did I call it menu?)
void drawmenu(void);

#endif