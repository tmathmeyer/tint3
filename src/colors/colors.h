#ifndef _T3_COLORS_H
#define _T3_COLORS_H

#include <stdint.h>
#include <stddef.h>

#include "drawctx/dc.h"

#define unlong unsigned long
// get a color from a string, and save it into the context
unlong _getcolor(DC *dc, const char *colstr);
unlong alphaset(unlong color, uint8_t alpha);
uint8_t hex(char c);
unlong getcolor(DC *dc, const char *colstr);

#endif
