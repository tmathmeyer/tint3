/* See LICENSE file for copyright and license details. */
#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif
#include "draw.h"

#define INRECT(x,y,rx,ry,rw,rh) ((x) >= (rx) && (x) < (rx)+(rw) && (y) >= (ry) && (y) < (ry)+(rh))
#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))
#define DEFFONT "fixed" /* xft example: "Monospace-11" */



//will be off in a config somewhere later
#define RIGHTSIDE {"bspwm","ted"}
#define RIGHTCOUNT 2



typedef enum {
    LEFT,
    RIGHT,
    CENTRE
} TextPosition;

static void drawmenu(void);
static void run(void);
static void setup(void);

static int bh, mw, mh;
static int height = 0;
static int itemcount = 0;
static int lines = 0;
static int monitor = -1;
static const char *font = "uushi-9";
static const char *normbgcolor = "#222222";
static const char *normfgcolor = "#bbbbbb";
static const char *selbgcolor  = "#005577";
static const char *selfgcolor  = "#eeeeee";

static ColorSet *normcol;
static ColorSet *selcol;

static Atom utf8;
static Bool topbar = True;
static DC *dc;
static Window root, win;

char* rightside[RIGHTCOUNT] = {"bspwm","ted"};


int main(int argc, char *argv[]) {

	dc = initdc();
	initfont(dc, font ? font : DEFFONT);
	normcol = initcolor(dc, normfgcolor, normbgcolor);
	selcol = initcolor(dc, selfgcolor, selbgcolor);
	setup();
    run();

	return EXIT_FAILURE;
}







void
drawmenu(void) {

	dc->x = 0;
	dc->y = 0;
    dc->w = 0;
	dc->h = height;

    dc->text_offset_y = 0;

    if(mh < height) {
        dc->text_offset_y = (height - mh) / 2;
    }

    drawrect(dc, 0, 0, mw, height, True, normcol->BG);

    for(int i = 0; i < RIGHTCOUNT; i++) {
    	dc -> w = textw(dc, rightside[i]);
    	drawtext(dc, rightside[i], normcol);
    	dc -> x += dc -> w;
    }

	//dc->w = textw(dc, "test ⮒⮒⮒");
    //drawtext(dc, "test ⮒⮒⮒", normcol);
    //dc->x += dc->w;



	mapdc(dc, win, mw, height);
}


void run(void) {
	XEvent ev;

	drawmenu();

	while(!XNextEvent(dc->dpy, &ev)){
		//drawmenu();
	}
}



// TODO: clean this shit
void setup(void) {
	int x, y, screen;
	XSetWindowAttributes wa;
#ifdef XINERAMA
	int n;
	XineramaScreenInfo *info;
#endif

	screen = DefaultScreen(dc->dpy);
	root = RootWindow(dc->dpy, screen);
	utf8 = XInternAtom(dc->dpy, "UTF8_STRING", False);

	/* menu geometry */
	bh = dc->font.height + 2;
	lines = MAX(lines, 0);
	mh = (MAX(MIN(lines + 1, itemcount), 1)) * bh;

    if(height < mh) {
        height = mh;
    }
#ifdef XINERAMA
	if((info = XineramaQueryScreens(dc->dpy, &n))) {
		int i, di;
		unsigned int du;
		Window dw;

		XQueryPointer(dc->dpy, root, &dw, &dw, &x, &y, &di, &di, &du);
		for(i = 0; i < n; i++)
			if((monitor == info[i].screen_number)
			|| (monitor < 0 && INRECT(x, y, info[i].x_org, info[i].y_org, info[i].width, info[i].height)))
				break;
		x = info[i].x_org;
		y = info[i].y_org + (topbar ? 0 : info[i].height - height);
		mw = info[i].width;
		XFree(info);
	}
	else
#endif
	{
		x = 0;
		y = topbar ? 0 : DisplayHeight(dc->dpy, screen) - height;
		mw = DisplayWidth(dc->dpy, screen);
	}
	/* menu window */
	wa.override_redirect = True;
	wa.background_pixmap = ParentRelative;
	wa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
	win = XCreateWindow(dc->dpy, root, x, y, mw, height, 0,
	                    DefaultDepth(dc->dpy, screen), CopyFromParent,
	                    DefaultVisual(dc->dpy, screen),
	                    CWOverrideRedirect | CWBackPixmap | CWEventMask, &wa);

	resizedc(dc, mw, height);
	XMapRaised(dc->dpy, win);
}
