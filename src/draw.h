/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _DRAW_H_
#define _DRAW_H_

#include <stdint.h>
#include "tint3.h"

DC *dc;

unsigned long getcolor(DC *dc, const char *colstr);
void drawrect_modifier(DC *dc, int x, int y, unsigned int w, unsigned int h, Bool fill, unsigned long color);
void draw_rectangle(DC * dc, unsigned int x, unsigned int y, unsigned int w, unsigned int h, Bool fill, unsigned long color);
void drawtext(DC *dc, const char *text, ColorSet *col);
void drawtextn(DC *dc, const char *text, size_t n, ColorSet *col);
void freecol(DC *dc, ColorSet *col);
void freedc(DC *dc);
ColorSet *initcolor(DC *dc, const char *foreground, const char *background);
DC *initdc(void);
void initfont(DC *dc, const char *fontstr);
void mapdc(DC *dc, Window win, unsigned int w, unsigned int h);
void resizedc(DC *dc, unsigned int w, unsigned int h, XVisualInfo * vinfo, XSetWindowAttributes * wa);
int textnw(DC *dc, const char *text, size_t len);
int textw(DC *dc, const char *text);
ColorSet *copy_color(ColorSet *color);
XftColor get_xft_color(DC *dc, const char *colstr);
ulong alphaset(ulong color, uint8_t alpha);
const char *progname;

void drawline(DC *dc, int at_x, int npts, int *points);
#endif
