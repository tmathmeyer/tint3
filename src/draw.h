/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#include "utils.h"

typedef struct {
	int x, y, w, h;
    int text_offset_y;
    int border_width;
	Display *dpy;
	GC gc;
	Pixmap canvas;
	XftDraw *xftdraw;
	struct {
		int ascent;
		int descent;
		int height;
		XFontSet set;
		XFontStruct *xfont;
		XftFont *xft_font;
	} font;
} DC;  /* draw context */


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
void resizedc(DC *dc, unsigned int w, unsigned int h);
int textnw(DC *dc, const char *text, size_t len);
int textw(DC *dc, const char *text);

const char *progname;
