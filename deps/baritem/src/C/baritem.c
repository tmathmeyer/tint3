#include "baritem.h"
#include "confparse/confparse.h"
#include "colors/colors.h"

char *get_baritem_option(char *opt_name, baritem *item) {
    entry *every;
    each(item->options, every) {
        if (!strcmp(opt_name, every->key)) {
            return every->value;
        }
    }
    return NULL;
}

extern char *bar_font_colour;
extern unsigned long bar_background_colour;
ColorSet *make_baritem_colours(char *fg, char *bg) {   
    ColorSet *result = malloc(sizeof(ColorSet));
    DC *dc = getdc();
    result->FG = (fg) ? getcolor(dc, fg) : getcolor(dc, get_bar_font_colour());
    result->BG = (bg) ? getcolor(dc, bg) : get_bar_background_colour();
    if(dc->font.xft_font) {
        if(!XftColorAllocName(
             dc->dpy
            ,DefaultVisual(dc->dpy, DefaultScreen(dc->dpy))
            ,DefaultColormap(dc->dpy, DefaultScreen(dc->dpy))
            ,fg?fg:get_bar_font_colour(), &result->FG_xft)
        ) {
            // do something if it fails
        }
    }
    return result;
}
