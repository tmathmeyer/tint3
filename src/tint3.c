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
#include "config.h"
#include "defaults.h"
#include "confparse.h"

#ifdef _WITH_MPD
#include "mpd.h"
#endif

#define INRECT(x,y,rx,ry,rw,rh) ((x) >= (rx) && (x) < (rx)+(rw) && (y) >= (ry) && (y) < (ry)+(rh))
#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))
#define DEFFONT "fixed"


static void drawmenu(void);
static void run(void);
static void setup(void);


static int height = 0;
static int width  = 0;


static const char *font = FONT;
static ColorSet *normcol;

static Bool topbar = True;
static DC *dc;
static Window root, win;

static bar_config * configuration;


// get the height of the bar
int get_bar_height(int font_height) {
    return font_height - 1 + 2 * (BAR_PADDING + BAR_BORDER);
}

// get the bar width
int get_bar_width(int display_width) {
    return display_width - 2 * BAR_MARGIN;
}




int scale_to(int from, int to, float by) {
    float f = (to-from) * by;
    return to-f;
}

int main(int argc, char *argv[]) {
    dc = initdc();
    initfont(dc, font ? font : DEFFONT);
    normcol = initcolor(dc, BAR_BACKGROUND, BAR_FOREGROUND);
    setup();
    run();

    return EXIT_FAILURE;
}

/* SPACE FOR MODULE FUNCTIONS */

baritem * battery_s() {
    batt_info * info = get_battery_information();
    if (info == NULL) {
        return NULL;
    }
    char * inf_as_str = malloc(20);
    snprintf(inf_as_str, 20, "%i %s", info -> percentage, info -> icon);
    baritem * result = malloc(sizeof(baritem));
    result -> string = inf_as_str;
    if (info -> percentage > BATTERY_CUTOF_HIGH) {
        result -> color = initcolor(dc, BATTERY_FOREGROUND_HIGH, BATTERY_BACKGROUND_HIGH);
    } else if (info -> percentage < BATTERY_CUTOF_LOW) {
        result -> color = initcolor(dc, BATTERY_FOREGROUND_LOW, BATTERY_BACKGROUND_LOW);
    } else {
        result -> color = initcolor(dc, BATTERY_FOREGROUND_MED, BATTERY_BACKGROUND_MED);
    }
    free(info);
    return result;
}

baritem * spacer_s(char c) {
    baritem * result = malloc(sizeof(baritem));
    result -> string = malloc(2);
    (result -> string)[0] = c;
    (result -> string)[1] = 0;
    result -> color = initcolor(dc, BAR_BACKGROUND, BAR_FOREGROUND);
    return result;
}


baritem * wmname_s() {
    FILE * desc = popen("wmname", "r");
    char * msg = malloc(20);
    int msg_c = 0; char msg_s;
    if (desc) {
        while( (msg_s = fgetc(desc)) != '\n') {
            msg[msg_c++] = msg_s;
        }
        if (msg_c < 20) {
            msg[msg_c] = 0;
        }
        pclose(desc);
    }

    baritem * result = malloc(sizeof(baritem));
    result -> string = msg;
    result -> color = initcolor(dc, WMNAME_FOREGROUND, WMNAME_BACKGROUND);
    return result;
}

baritem * network_up_s() {
    net_info * netstack = get_net_info();
    baritem * result = malloc(sizeof(baritem));
    result -> string = graph_to_string(netstack -> up);
    result -> color = initcolor(dc, NET_UP_FOREGROUND, NET_UP_BACKGROUND);
    return result;
}
baritem * network_down_s() {
    net_info * netstack = get_net_info();
    baritem * result = malloc(sizeof(baritem));
    result -> string = graph_to_string(netstack -> down);
    result -> color = initcolor(dc, NET_DOWN_FOREGROUND, NET_DOWN_BACKGROUND);
    return result;
}

baritem * weather_s() {
    weather_info * winf = get_weather();
    if (winf == NULL) {
        return NULL;
    }
    int temp_for_color = winf -> temperature;


    if (temp_for_color > 99) {
        temp_for_color = 99;
    }
    if (temp_for_color < 0) {
        temp_for_color = 0;
    }
    temp_for_color *= temp_for_color;
    temp_for_color *= 255;
    temp_for_color /= (99*99);

    temp_for_color = 255 - ((255 - temp_for_color) * .9);

    int r = scale_to(temp_for_color, 255, .7);
    int b = scale_to(255-temp_for_color, 255, .7);
    int g = scale_to(0, 255, .7);

    char * color = malloc(8);
    snprintf(color, 8, "#%x", b + g*256 + r*65536);
    baritem * result = malloc(sizeof(baritem));
    result -> color = initcolor(dc, color, WEATHER_BACKGROUND);
    result -> color -> FG = temp_for_color + ((255-temp_for_color) * 65536);
    free(color);

    char * text = malloc(16);
    memset(text, 0, 16);


    char * icon = NULL;
    if (winf -> temperature > 70) {
        icon = "▉";
    } else if (winf -> temperature < 32) {
        icon = "▋";
    } else {
        icon = "▊";
    }

    snprintf(text, 16, "%i%s%i", winf -> temperature, icon, winf -> humidity);

    result -> string = text;
    return result;
}

baritem * volume_s() {
    volume_info * vol = get_volume_info();
    baritem * result = malloc(sizeof(baritem));
    result -> string = malloc(4);
    snprintf(result -> string, 4, "%u", vol -> volume_level);
    result -> color = vol -> muted ? initcolor(dc, VOLUME_BACKGROUND, VOLUME_FOREGROUND) : initcolor(dc, NET_UP_FOREGROUND, NET_UP_BACKGROUND);
    return result;
}

baritem * window_s() {
    baritem * result = malloc(sizeof(baritem));
    result -> string = get_active_window_name();
    result -> color = initcolor(dc, CURRENT_WINDOW_FOREGROUND, CURRENT_WINDOW_BACKGROUND);
    return result -> string == NULL ? NULL : result;
}

#ifdef _WITH_MPD
baritem * mpd_s() {
    baritem * result = malloc(sizeof(baritem));
    result -> string = malloc(64);
    if (!get_mpd_info(MPD_INFO_FORMAT_STRING, result -> string, 64)) {
        free(result -> string);
        free(result);
        return NULL;
    }
    result -> color = initcolor(dc, MPD_INFO_FOREGROUND, MPD_INFO_BACKGROUND);
    return result;
}
#endif


/* END SPACE FOR MODULE FUNCTIONS */



















/* rework the modules */

void update_nba(baritem * item) {
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
    }

    if (!(strncmp(conf_inf -> id, "text", 4))) {
        if (!strncmp(conf_inf -> source, "clock", 5)) {
            ipl -> update = &get_time_format;
        }
    }

    if (!(strncmp(conf_inf -> id, "text", 4))) {
        if (!strncmp(conf_inf -> source, "window_title", 12)) {
            ipl -> update = &get_active_window_name;
        }
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
    draw_rectangle(dc, BAR_BORDER, BAR_BORDER,
                   width-2*BAR_BORDER, height-2*BAR_BORDER, True, normcol -> BG);

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
        usleep(UPDATE_DELAY);
    }
}

// gets the vertical position of the bar, depending on margins and position
int vertical_position(Bool bar_on_top, int display_height, int bar_height) {
    if (bar_on_top) {
        return BAR_MARGIN;
    } else {
        return display_height - (bar_height + BAR_MARGIN);
    }
}

int horizontal_position() {
    return BAR_MARGIN;
}


// TODO: clean this shit
void setup(void) {
    configuration = readblock(fopen("test", "r"));
    dc -> border_width = BAR_BORDER;

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
