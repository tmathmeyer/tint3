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




int main(int argc, char *argv[]) {
    dc = initdc();
    initfont(dc, font ? font : DEFFONT);
    normcol = initcolor(dc, BAR_BACKGROUND, BAR_FOREGROUND);
    setup();
    run();

    return EXIT_FAILURE;
}

/* SPACE FOR MODULE FUNCTIONS */

baritem * battery_s(DC * dc) {
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


baritem * wmname_s(DC * dc) {
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


baritem * timeclock_s(DC * d) {
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

// lol WIP
baritem * desktops_s(DC * d) {
    baritem * result = malloc(sizeof(baritem));
    result -> string = get_desktops_info();
    result -> color = initcolor(dc, CLOCK_FOREGROUND, CLOCK_BACKGROUND);
    result -> type = 'D';
    return result;
}



char ** netstack = NULL;
baritem * network_up_s() {
    if (netstack == NULL) {
        netstack = get_net_info();
    }
    baritem * result = malloc(sizeof(baritem));
    result -> string = netstack[0];
    result -> color = initcolor(dc, NET_UP_FOREGROUND, NET_UP_BACKGROUND);
    result -> type = 'N';
    return result;
}
baritem * network_down_s() {
    if (netstack == NULL) {
        netstack = get_net_info();
    }
    baritem * result = malloc(sizeof(baritem));
    result -> string = netstack[1];
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

    char * netcolor = malloc(8);
    memset(netcolor, 0, 8);
    strncpy(netcolor, "#000000", 7);
    char * colorpallete = "0123456789ABCDEF";
    netcolor[1] = colorpallete[temp_for_color/16];
    netcolor[2] = colorpallete[temp_for_color%16];
    temp_for_color = 255 - temp_for_color;
    netcolor[5] = colorpallete[temp_for_color/16];
    netcolor[6] = colorpallete[temp_for_color%16];

    baritem * result = malloc(sizeof(baritem));
    result -> color = initcolor(dc, netcolor, WEATHER_BACKGROUND);
    result -> type = 'W';

    free(netcolor);

    char * text = malloc(16);
    memset(text, 0, 16);


    if (winf -> temperature > 70) {
        snprintf(text, 16, "%i%s%i", winf -> temperature, "▉", winf -> humidity);
    } else if (winf -> temperature < 32) {
        snprintf(text, 16, "%i%s", winf -> temperature, "▋");
    } else {
        snprintf(text, 16, "%i%s", winf -> temperature, "▊");
    }

    result -> string = text;
    return result;
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
    switch(item -> type) {
        default:
            free(item -> string);
            if (netstack != NULL) {
                free(netstack);
            } netstack = NULL;
    }
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
        usleep(200000);
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
