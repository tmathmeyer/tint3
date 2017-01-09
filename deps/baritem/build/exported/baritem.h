#ifndef _T3_BARITEM_H_
#define _T3_BARITEM_H_

#include "dlist/dlist.h"
#include "xftutils/colorset.h"
#include "graph/graph.h"

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

    // name
    char *name;
    
    // update listeners
    dlist *(* update)(struct baritem *);
    void (* click)(struct baritem *, int xpos);
} baritem;

typedef struct stylized_text_element {
    unsigned long attributes;
    char *text;
    ColorSet *color;
    unsigned int length;
} text_element;

typedef struct generic_element {
    union {
        graph_element *graph;
        text_element *text;
    };
    unsigned int opt;
    unsigned int length;
} element;

char *get_baritem_option(char *, baritem *);
ColorSet *make_baritem_colours(char *, char *);

unsigned long get_bar_background_colour();
char *get_bar_font_colour();
#endif
