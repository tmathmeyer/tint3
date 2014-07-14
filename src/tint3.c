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

#define INRECT(x,y,rx,ry,rw,rh) ((x) >= (rx) && (x) < (rx)+(rw) && (y) >= (ry) && (y) < (ry)+(rh))
#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))
#define DEFFONT "fixed"


static void drawmenu(void);
static void run(void);
static void setup(void);

static int bh, mw, mh;
static int height = 0;
static int itemcount = 0;
static int lines = 0;
static const char *font = FONT;
static ColorSet *normcol;

static Atom utf8;
static Bool topbar = True;
static DC *dc;
static Window root, win;


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
    result -> type = 'B';
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
    result -> type = ' ';
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
    result -> type = 'W';
    return result;
}


baritem * timeclock_s() {
    FILE * desc = popen("date +'%H:%M:%S'", "r");
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
    result -> color = initcolor(dc, CLOCK_FOREGROUND, CLOCK_BACKGROUND);
    result -> type = 'T';
    return result;
}

baritem * desktops_s() {
    baritem * result = malloc(sizeof(baritem));
    result -> string = get_desktops_info();
    result -> color = initcolor(dc, DESKTOP_FOREGROUND, DESKTOP_BACKGROUND);
    result -> type = 'D';
    return result;
}


baritem * network_up_s() {
    net_info * netstack = get_net_info();
    baritem * result = malloc(sizeof(baritem));
    result -> string = graph_to_string(netstack -> up);
    result -> color = initcolor(dc, NET_UP_FOREGROUND, NET_UP_BACKGROUND);
    result -> type = 'N';
    return result;
}
baritem * network_down_s() {
    net_info * netstack = get_net_info();
    baritem * result = malloc(sizeof(baritem));
    result -> string = graph_to_string(netstack -> down);
    result -> color = initcolor(dc, NET_DOWN_FOREGROUND, NET_DOWN_BACKGROUND);
    result -> type = 'N';
    return result;
}

baritem * weather_s() {
    weather_info * winf = get_weather();

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
    result -> type = 'W';
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
    result -> color = initcolor(dc, NET_UP_FOREGROUND, NET_UP_BACKGROUND);
    result -> type = 'V';
    return result;
}

baritem * window_s() {
    baritem * result = malloc(sizeof(baritem));
    result -> string = get_active_window_name();
    result -> color = initcolor(dc, CURRENT_WINDOW_FOREGROUND, CURRENT_WINDOW_BACKGROUND);
    result -> type = 'A';
    return result -> string == NULL ? NULL : result;
}




/* END SPACE FOR MODULE FUNCTIONS */


void drawmenu(void) {
    dc->x = 0;
    dc->y = 0;
    dc->w = 0;
    dc->h = height;

    dc->text_offset_y = 0;

    if(mh < height) {
        dc->text_offset_y = (height - mh) / 2;
    }

    drawrect(dc, 0, 0, mw, height, True, normcol->BG);

    itemlist * left = config_to_list(LEFT_ALIGN);
    itemlist * right = config_to_list(RIGHT_ALIGN);
    itemlist * center = config_to_list(CENTER_ALIGN);

               total_list_length(left);
    int rlen = total_list_length(right);
    int clen = total_list_length(center);

    draw_list(left);
    dc -> x = mw-rlen;
    draw_list(right);
    dc -> x = (mw-clen)/2;
    draw_list(center);

    free_list(left);
    free_list(right);
    free_list(center);

    mapdc(dc, win, mw, height);
}




itemlist * config_to_list (char * list) {
    itemlist * head = NULL;
    itemlist * tail = NULL; // add to tail
    while (*(list) != 0) {
        baritem * item = char_to_item(*(list++));
        if (item != NULL) {
            itemlist * next = malloc(sizeof(itemlist));
            next -> item = item;
            next -> next = NULL;
            if (head == NULL) {
                head = next;
                tail = next;
            } else {
                tail -> next = next;
                tail = tail -> next;
            }
        }
    }
    return head;
}

baritem * char_to_item(char c) {
    switch(c) {
        case 'B':
            return battery_s(dc);
        case 'T':
            return timeclock_s(dc);
        case 'D':
            return desktops_s(dc);
        case 'N':
            return network_down_s();
        case 'M':
            return network_up_s();
        case 'W':
            return weather_s();
        case 'A':
            return window_s();
        case 'V':
            return volume_s();
        default:
            return spacer_s(c);
    }
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
    while(1){
        drawmenu();
        XEvent xe;
        while(QLength(dc->dpy)) {
            XNextEvent(dc->dpy, &xe);
        }
        usleep(100000);
    }
}

// TODO: clean this shit
void setup(void) {
    int x, y, screen;
    XSetWindowAttributes wa;

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
    x = 0;
    y = topbar ? 0 : DisplayHeight(dc->dpy, screen) - height;
    mw = DisplayWidth(dc->dpy, screen);
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
