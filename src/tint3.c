/*
 * Copyright (C) 2014 Ted Meyer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#define _DEFAULT_SOURCE

#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "draw.h"
#include "utils.h"
#include "confparse.h"
#include "lwbi.h"

#ifdef _WITH_MPD
#include "mpd.h"
#endif

#define INRECT(x,y,rx,ry,rw,rh) ((x) >= (rx) && (x) < (rx)+(rw) && (y) >= (ry) && (y) < (ry)+(rh))
#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))


static void drawmenu(void);
static void run(void);
static void setup(void);


static int height = 0;
static int width  = 0;


static const char *font = "sakamoto-11";
static ColorSet *normcol;

static Bool topbar = True;
static DC *dc;
static Window root, win;

static bar_config * configuration;


// get the height of the bar
int get_bar_height(int font_height) {
    return font_height - 1 + 2 * (configuration -> padding_size + configuration -> border_size);
}

// get the bar width
int get_bar_width(int display_width) {
    return display_width - 2 * configuration -> margin_size;
}

int scale_to(int from, int to, float by) {
    float f = (to-from) * by;
    return to-f;
}

int main(int argc, char *argv[]) {

    dc = initdc();
    initfont(dc, font ? font : "fixed");
    setup();
    normcol = initcolor(dc, configuration -> border_color, configuration -> background_color);
    run();

    return EXIT_FAILURE;
}






#ifdef _WITH_MPD
baritem * mpd_s() {
    baritem * result = malloc(sizeof(baritem));
    result -> string = malloc(64);
    if (!get_mpd_info("%t", result -> string, 64)) {
        free(result -> string);
        free(result);
        return NULL;
    }
    result -> color = initcolor(dc, "#000000", "#000000");
    return result;
}
#endif









/* rework the modules */

void update_nba(baritem * item) {
    if (item -> string != NULL) {
        free(item -> string);
    }
    item -> string = (item -> update)(item);
}

baritem * makeitem(block * config_info) {
    baritem * result = malloc(sizeof(baritem));
    result -> string = NULL;
    result -> format = config_info -> format;
    result -> source = config_info -> source;
    result -> update = NULL;
    result -> color = initcolor(dc, config_info -> forground, config_info -> background);
    return result;
}

char * questions(baritem *meh) {
    char * result = malloc(6);
    result[0] = '?';
    result[1] = '?';
    result[2] = 0;
    return result;
}

void infer_type(block * conf_inf, baritem * ipl) {
    ipl -> update = &questions;

    if (!(strncmp(conf_inf -> id, "radio", 5))) {
        if (!strncmp(conf_inf -> source, "workspaces", 10)) {
            ipl -> update = &get_desktops_info;
        }
    } else if (!(strncmp(conf_inf -> id, "text", 4))) {
        if (!strncmp(conf_inf -> source, "clock", 5)) {
            ipl -> update = &get_time_format;
        } else if (!strncmp(conf_inf -> source, "window_title", 12)) {
            ipl -> update = &get_active_window_name;
        } else {
            ipl -> update = &get_plain_text;
        }
    } else if (!(strncmp(conf_inf -> id, "scale", 5))) {
        if (starts_with(conf_inf -> source, "weather")) {
            ipl -> update = &get_weather;
        } else if (starts_with(conf_inf -> source, "battery")) {
            ipl -> update = &get_battery;
        } else if (starts_with(conf_inf -> source, "alsa")) {
            ipl -> update = &get_volume_level;
        }
    } else if (!(strncmp(conf_inf -> id, "graph", 5))) {
        ipl -> update = &get_net_graph;
    }

    update_nba(ipl);
}


itemlist * c2l(block_list * bid) {
    itemlist * result = NULL;
    while(bid) {
        itemlist * cur = malloc(sizeof(itemlist));
        cur -> item = makeitem(bid -> data);
        infer_type(bid -> data, cur -> item);

        if (cur -> item -> update == NULL) {
            free(cur);
        }
        else {
            cur -> next = result;
            result = cur;
        }
        bid = bid -> next;
    }
    return result;
}


void drawmenu(void) {
    dc->x = 0;
    dc->y = 0;
    dc->w = 0;
    dc->h = height;

    dc->text_offset_y = 0;

    draw_rectangle(dc, 0, 0, width+2, height+2, True, normcol -> FG);
    draw_rectangle(dc, configuration -> border_size, configuration -> border_size,
                   width-2*configuration -> border_size,
                   height-2*configuration -> border_size, True, normcol -> BG);

    itemlist * left = c2l(configuration -> left);
    itemlist * right = c2l(configuration -> right);
    itemlist * center = c2l(configuration -> center);

    total_list_length(left);
    int rlen = total_list_length(right);
    int clen = total_list_length(center);

    draw_list(left);
    dc -> x = width-rlen;
    draw_list(right);
    dc -> x = (width-clen)/2;
    draw_list(center);

    free_list(left);
    free_list(right);
    free_list(center);

    mapdc(dc, win, width, height);
}


































void free_list(itemlist * list) {
    while(list != NULL) {
        free_baritem(list -> item);
        itemlist * old = list;
        list = list -> next;
        free(old);
    }
}

void free_baritem(baritem * item) {
    free(item -> string);
    free(item -> color);
    free(item);
}

unsigned int total_list_length(itemlist * list) {
    unsigned int len = 0;
    while(list != NULL) {
        list -> item -> length = textw(dc, list -> item -> string);
        len += list -> item -> length;
        list = list -> next;
    }
    return len;
}

void draw_list(itemlist * list) {
    while(list != NULL) {
        dc -> w = list -> item -> length;
        drawtext(dc, list -> item -> string, list -> item -> color);
        dc -> x += dc -> w;
        list = list -> next;
    }
}

void run(void) {
    XEvent xe;
    drawmenu();
    while(!XNextEvent(dc->dpy, &xe)){
        drawmenu();
        usleep(200000);
    }
}

// gets the vertical position of the bar, depending on margins and position
int vertical_position(Bool bar_on_top, int display_height, int bar_height) {
    if (bar_on_top) {
        return configuration -> margin_size;
    } else {
        return display_height - (bar_height + configuration -> margin_size);
    }
}

int horizontal_position() {
    return configuration -> margin_size;
}


// TODO: clean this shit
void setup(void) {
    FILE * fp = fopen("~/.config/tint3/tint3rc", "r");
    if (!fp) {
        fp = fopen("/etc/tint3/tint3rc", "r");
    }
    if (!fp) {
        perror("can't find a config file!! put one in ~/.config/tint3/tint3rc");
        exit(0);
    }
    configuration = readblock(fp);
    dc -> border_width = configuration -> margin_size;

    int x, y;
    XSetWindowAttributes wa;

    int screen = DefaultScreen(dc->dpy);
    root = RootWindow(dc->dpy, screen);

    /* menu geometry */
    height = get_bar_height(dc->font.height);
    width  = get_bar_width(DisplayWidth(dc->dpy, screen));

    x = horizontal_position();
    y = vertical_position(topbar, DisplayHeight(dc->dpy, screen), height);


    /* menu window */
    wa.override_redirect = True;
    wa.background_pixmap = ParentRelative;
    wa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    win = XCreateWindow(dc->dpy, root, x, y, width, height, 0,
            DefaultDepth(dc->dpy, screen), CopyFromParent,
            DefaultVisual(dc->dpy, screen),
            CWOverrideRedirect | CWBackPixmap | CWEventMask, &wa);

    resizedc(dc, width, height);
    XMapRaised(dc->dpy, win);

    long pval = XInternAtom (dc->dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    long prop = XInternAtom (dc->dpy, "_NET_WM_WINDOW_TYPE", False);

    XChangeProperty (dc->dpy, win, prop, XA_ATOM, 32, PropModeReplace, (unsigned char *) &pval, 1);

    /* reserve space on the screen */
    prop = XInternAtom (dc->dpy, "_NET_WM_STRUT_PARTIAL", False);
    long ptyp = XInternAtom (dc->dpy, "CARDINAL", False);
    int16_t strut[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (topbar) {
        strut[2] = height+y;
        strut[9] = width;
    } else {
        strut[3] = height;
        strut[11] = width;
    }
    XChangeProperty (dc->dpy, win, prop, ptyp, 16, PropModeReplace, (unsigned char *) &strut[0], 12);

    /* This is for support with legacy WMs */
    prop = XInternAtom (dc->dpy, "_NET_WM_STRUT", False);
    unsigned long strut_s[4] = {0, 0, 0, 0};
    if (topbar)
        strut_s[2] = height;
    else
        strut_s[3] = height;
    XChangeProperty (dc->dpy, win, prop, ptyp, 32, PropModeReplace, (unsigned char *) &strut_s[0], 4);

    /* Appear on all desktops */
    prop = XInternAtom (dc->dpy, "_NET_WM_DESKTOP", False);
    long all_desktops = 0xffffffff;
    XChangeProperty(dc->dpy, win, prop, ptyp, 32, PropModeReplace, (unsigned char *) &all_desktops, 1);

    NET_CURRENT_DESKTOP = XInternAtom(dc -> dpy, "_NET_CURRENT_DESKTOP", 0);
    NET_NUMBER_DESKTOPS = XInternAtom(dc -> dpy, "_NET_NUMBER_OF_DESKTOPS", 0);
    _CARDINAL_ = XA_CARDINAL;
}


int get_x11_property(Atom at, Atom type) {
    Atom type_ret;
    int format_ret = 0, data = 1;
    unsigned long nitems_ret = 0,
                  bafter_ret = 0;
    unsigned char * prop_value = 0;
    int result;

    result = XGetWindowProperty(dc->dpy, root, at, 0, 0x7fffffff,
            0, type, &type_ret, &format_ret,
            &nitems_ret, &bafter_ret, &prop_value);

    if (result == Success && prop_value) {
        data = ((unsigned long * ) prop_value)[0];
        XFree(prop_value);
    }

    return data;
}
