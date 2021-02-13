#ifndef _T3_COLORSET_H_
#define _T3_COLORSET_H_

#include <X11/Xft/Xft.h>

typedef struct ColorSet{
    unsigned long FG;
    unsigned long BG;
    XftColor FG_xft;
} ColorSet;

#endif
