/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef __UTILS__
#define __UTILS__

#include <X11/Xft/Xft.h>
#include <pthread.h>

typedef struct ColorSet{
    unsigned long FG;
    XftColor FG_xft;
    unsigned long BG;
} ColorSet;

typedef struct baritem{
    ColorSet * color;
    unsigned int length;
    char * string;
    char * format;
    char * source;
    char * (* update)(struct baritem *);
} baritem;

typedef struct itemlist {
    uint length;
    baritem ** buffer;
} itemlist;

typedef struct bar_layout {
    itemlist * left;
    short leftlen;
    itemlist * right;
    short rightlen;
    itemlist * center;
    short centerlen;
} bar_layout;


itemlist * config_to_list (char * list);
baritem * char_to_item(char c);
baritem * weather_s();
void free_list(itemlist * list);
void free_baritem(baritem * item);
void draw_list(itemlist * list);
unsigned int total_list_length(itemlist * list);
int get_x11_property(Atom at, Atom type);



char * get_time_format(baritem * item);
char * get_battery(baritem * item);
char * get_volume_level(baritem * item);
char * get_active_window_name(baritem * item);
char * get_plain_text(baritem * item);

Atom NET_NUMBER_DESKTOPS,
     NET_CURRENT_DESKTOP;

Atom _CARDINAL_;


#endif
