#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>

#define uint unsigned int
typedef struct window_package {
    uint x, y, w, h;
    XSetWindowAttributes wa;
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
    Window win;
    Display *dsp;
} window_package;









#define MAX(a, b)   ((a) > (b) ? (a) : (b))
void initfont(window_package *wp, const char *fontstr) {
    char * def, ** missing = NULL;
    int i, n;

    if((wp->font.xfont = XLoadQueryFont(wp->dsp, fontstr))) {
        wp->font.ascent = wp->font.xfont->ascent;
        wp->font.descent = wp->font.xfont->descent;
    } else if((wp->font.set = XCreateFontSet(wp->dsp, fontstr, &missing, &n, &def))) {
        char ** names;
        XFontStruct ** xfonts;

        n = XFontsOfFontSet(wp->font.set, &xfonts, &names);
        for(i = wp->font.ascent = wp->font.descent = 0; i < n; i++) {
            wp->font.ascent = MAX(wp->font.ascent, xfonts[i]->ascent);
            wp->font.descent = MAX(wp->font.descent, xfonts[i]->descent);
        }
    } else if((wp->font.xft_font = XftFontOpenName(wp->dsp,
                    DefaultScreen(wp->dsp), fontstr))) {
        wp->font.ascent = wp->font.xft_font->ascent;
        wp->font.descent = wp->font.xft_font->descent;
    } else {
        printf("cannot load font '%s'\n", fontstr);
    }
    if(missing) {
        XFreeStringList(missing);
    }
    wp->font.height = wp->font.ascent + wp->font.descent;
}


void resizedc(window_package *wp, int w, int h, XVisualInfo *vinfo, XSetWindowAttributes *wa) {
    if(wp->canvas) {
        XFreePixmap(wp->dsp, wp->canvas);
    }
    wp->canvas = XCreatePixmap(
            wp->dsp,
            DefaultRootWindow(wp->dsp),
            w, h,
            vinfo -> depth
            );
    if(wp->font.xft_font && !(wp->xftdraw)) {
        wp->xftdraw = XftDrawCreate(wp->dsp, wp->canvas, vinfo->visual, wa -> colormap);
        if(!(wp->xftdraw)) {
            printf("error, cannot create xft drawable\n");
        }
    }
}

window_package *init_window(const char *font, int x, int y, int width, int height) {
    window_package *result = malloc(sizeof(window_package));
    if(!result) {
        printf("cannot allocate memory");
        return NULL;
    }

    result->dsp = XOpenDisplay(NULL);
    if(!(result->dsp)) {
        printf("cannot open display\n");
        free(result);
        return NULL;
    }

    result->x = x;
    result->y = y;
    result->w = width;
    result->h = height;


    result->gc = XCreateGC(result->dsp, DefaultRootWindow(result->dsp), 0, NULL);
    XSetLineAttributes(result->dsp, result->gc, 1, LineSolid, CapButt, JoinMiter);
    result->font.xft_font = NULL;
    result->font.xfont = NULL;
    result->font.set = NULL;
    result->xftdraw = NULL;
    result->canvas = None;

    XVisualInfo vinfo;
    XMatchVisualInfo(result->dsp, DefaultScreen(result->dsp), 32, TrueColor, &vinfo);
    XSetWindowAttributes wa;
    wa.colormap = XCreateColormap(
            result->dsp,
            DefaultRootWindow(result->dsp),
            vinfo.visual,
            AllocNone
            );

    wa.border_pixel = 0;
    wa.background_pixel = 0;
    result->wa = wa;
    initfont(result, font ? font : "fixed");






    wa.override_redirect = True;
    wa.background_pixmap = ParentRelative;
    wa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    result->win = XCreateWindow(
            result->dsp, 
            RootWindow(result->dsp, DefaultScreen(result->dsp)),
            x, y, width, height,
            0,
            vinfo.depth,
            InputOutput,
            vinfo.visual,
            CWOverrideRedirect|CWEventMask|CWColormap|CWBorderPixel|CWBackPixel,
            &wa
            );
    result->gc = XCreateGC(result->dsp, result->win, 0, NULL);

    resizedc(result, width, height, &vinfo, &wa);
    XMapRaised(result->dsp, result->win);

    return result;
}


unsigned long getcolor(window_package *wp, const char *colstr) {
    XColor color;
    if(!XAllocNamedColor(wp->dsp, wp->wa.colormap, colstr, &color, &color)) {
        printf("cannot allocate color '%s'\n", colstr);
        return 0;
    }
    return color.pixel;
}

void draw_window(window_package *win) {   
    XRectangle rect = {win->x, win->y, win->w, win->h};
    puts("made rect");   
    XSetForeground(win->dsp, win->gc, getcolor(win, "#ccffcc"));
    puts("set forground");
    XFillRectangles(win->dsp, win->canvas, win->gc, &rect, 1);
}

int main() {
    window_package *win = init_window("sakamoto-11", 400, 400, 400, 400);
    puts("fuck");

    while(1){

        draw_window(win);

    }
}




