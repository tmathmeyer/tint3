/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <sys/types.h>
#include <pthread.h>
#include <pwd.h>
#include "draw.h"
#include "utils.h"
#include "confparse.h"
#include "lwbi.h"
#include "vdesk.h"
#include "weather.h"
#include "tint3.h"
#include "netinfo.h"

#define INRECT(x,y,rx,ry,rw,rh) ((x) >= (rx) && (x) < (rx)+(rw) && (y) >= (ry) && (y) < (ry)+(rh))
#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))

#define IS_ID(x, a) (!(strncmp(x -> id, a, strlen(a))))


static void run(void);
static void setup(void);
static void config_to_layout(void);
void update_nba(baritem * item);
static void infer_type(block *conf_inf, baritem *ipl);

static int height = 0;
static int width  = 0;

static char *quest = "???";
static unsigned long bg_bar = 0, draw_bg = 0;
static unsigned long bo_bar = 0, draw_bo = 0;

static Window win;
static bar_config * configuration;
static bar_layout * layout;


static pthread_mutex_t lock;


const char *font = "sakamoto-11";
int topbar = 1;


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

int main() {

    XInitThreads();
    pthread_mutex_init(&lock, NULL);
    setup();

    if (configuration -> background_color != NULL) {
        bg_bar = getcolor(dc, configuration -> background_color);
        draw_bg = 1;
    }
    if (configuration -> border_color != NULL) {
        bo_bar = getcolor(dc, configuration -> border_color);
        draw_bo = 1;
    }

    config_to_layout();

    run();

    return EXIT_FAILURE;
}


// update the item's string contents
void update_nba(baritem * item) {
    if (item->update) {
        if ((item->string != NULL) && (item->string != quest)) {
            free(item -> string);
        }
        item -> string = (item->update)(item);
    }
}

// make a color if we can
ColorSet * make_possible_color(char * fg, char * bg) {
    if (fg[0] && bg[0]) {
        return initcolor(dc, fg, bg);
    }
    ColorSet * result = initcolor(dc, fg, "#000000");
    result -> BG = bg[0]?getcolor(dc, fg):bg_bar;
    return result;
}

// turn a single item from the config stream into a displayable item
baritem * makeitem(block * config_info) {
    baritem * result = malloc(sizeof(baritem));
    result -> color  = make_possible_color(config_info -> forground, config_info -> background);
    result -> format = config_info -> format;
    result -> source = config_info -> source;
    result -> string = quest;
    infer_type(config_info, result);
    update_nba(result);
    return result;
}

// fallback, in case no other source can be found
char *questions(baritem *meh) {
    return meh?quest:NULL;
}

// set the function that creates information
void infer_type(block * conf_inf, baritem *ipl) {
    ipl -> update = &questions;

    if (IS_ID(conf_inf, "radio")) {
        if (!strncmp(conf_inf -> source, "workspaces", 10)) {
            spawn_vdesk_thread(ipl);
            ipl -> update = NULL;
            ipl -> string = get_desktops_info(ipl);
        }
    } else if (IS_ID(conf_inf, "text")) {
        if (!strncmp(conf_inf -> source, "clock", 5)) {
            ipl -> update = &get_time_format;
        } else if (!strncmp(conf_inf -> source, "window_title", 12)) {
            ipl -> update = &get_active_window_name;
        } else {
            ipl -> update = &get_plain_text;
        }
    } else if (IS_ID(conf_inf, "weather")) {
            spawn_weather_thread(ipl);
            ipl -> update = NULL;
            ipl -> string = get_weather(ipl);
    } else if (IS_ID(conf_inf, "scale")) {
        if (starts_with(conf_inf -> source, "battery")) {
            ipl -> update = &get_battery;
        } else if (starts_with(conf_inf -> source, "alsa")) {
            ipl -> update = &get_volume_level;
        }
    } else if (IS_ID(conf_inf, "graph")) {
        ipl -> update = &get_net_graph;
    }
}

// convert a config to a drawable
itemlist *config_to_drawable(block_list * bid) {
    block_list *ctrtmp = bid;
    itemlist *result = malloc(sizeof(itemlist));
    result->length = 0;
    while(ctrtmp) {
        result->length++;
        ctrtmp = ctrtmp->next;
    }

    result->buffer = malloc(sizeof(baritem*) * result->length);
    uint ctr = 0;

    for(; ctr < result->length; ctr++ , bid=bid->next) {
        (result->buffer)[ctr] = makeitem(bid->data);
    }
    return result;
}



void config_to_layout() {
    layout = malloc(sizeof(bar_layout));
    layout -> left = config_to_drawable(configuration -> left);
    layout -> right = config_to_drawable(configuration -> right);
    layout -> center = config_to_drawable(configuration -> center);
}


void update_list_of_items(itemlist *list) {
    uint ctr = 0;
    for(; ctr < list->length; ctr++) {
        update_nba(((list->buffer)[ctr]));
    }
}

void update_with_lens() {
    update_list_of_items(layout->left);
    update_list_of_items(layout->right);
    update_list_of_items(layout->center);

    layout -> leftlen = total_list_length(layout -> left);
    layout -> rightlen = total_list_length(layout -> right);
    layout -> centerlen = total_list_length(layout -> center);
}






void drawmenu(void) {
    pthread_mutex_lock(&lock);
    dc->x = 0;
    dc->y = 0;
    dc->w = 0;
    dc->h = height;

    if (draw_bo) {
        draw_rectangle(dc, 0, 0, width+2, height+2, True, bo_bar);
    }
    
    draw_rectangle(dc, configuration -> border_size, configuration -> border_size,
            width-2*configuration -> border_size,
            height-2*configuration -> border_size, True, bg_bar);
    

    update_with_lens();
    

    dc -> x = dc->color_border_pixels;
    draw_list(layout -> left);
    dc -> x = width-(layout -> rightlen)-dc->color_border_pixels;
    draw_list(layout -> right);
    dc -> x = (width-(layout -> centerlen))/2;
    draw_list(layout -> center);

    mapdc(dc, win, width, height);
    pthread_mutex_unlock(&lock);
}




unsigned int total_list_length(itemlist * list) {
    uint len = 0;
    uint ctr = 0;
    for(; ctr < list->length; ctr++) {
        int tmp = (list->buffer)[ctr]->length
                = textw(dc, (list->buffer)[ctr]->string);
        len += tmp;
    }
    return len;
}

void draw_list(itemlist * list) {
    uint ctr = 0;
    for(; ctr < list->length; ctr++) {
        dc -> w = (list->buffer)[ctr]->length;
        drawtext(dc, (list->buffer)[ctr]->string, (list->buffer)[ctr]->color);
        dc->x += dc->w;
    }
}

void run(void) {
    XEvent xe;
    while(1){
        drawmenu();
        while(QLength(dc->dpy)) {
            XNextEvent(dc->dpy, &xe);
        }

        usleep(900000);
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
void setup() {
    char cwd[100] = {0};
    getcwd(cwd, 100);
    char pwd[100] = {0};
    snprintf(pwd, 100, "%s/tint3rc", cwd);
    FILE * fp = fopen(pwd, "r");
    if (fp == NULL) {
        snprintf(pwd, 100, "%s/.tint3rc", getenv("HOME"));
        fp = fopen(pwd, "r");
        if (fp == NULL) {
            fp = fopen("/etc/tint3/tint3rc", "r");
        }
    }

    if (fp == NULL) {
        perror("can't find a config file!! put one in ~/.tint3rc");
        exit(0);
    }

    configuration = readblock(fp);

    dc = initdc();
    XVisualInfo vinfo;
    XMatchVisualInfo(dc->dpy, DefaultScreen(dc->dpy), 32, TrueColor, &vinfo);
    XSetWindowAttributes wa;
    wa.colormap = XCreateColormap(dc->dpy,
            DefaultRootWindow(dc->dpy),
            vinfo.visual,
            AllocNone);
    wa.border_pixel = 0;
    wa.background_pixel = 0;
    dc -> wa = wa;
    initfont(dc, font ? font : "fixed");


    dc -> border_width = configuration -> margin_size;
    dc -> color_border_pixels = configuration -> border_size;
    dc -> text_offset_y = configuration -> padding_size; 

    int x, y;

    int screen = DefaultScreen(dc->dpy);
    root = RootWindow(dc->dpy, screen);

    /* menu geometry */
    height = get_bar_height(dc->font.height);
    width  = get_bar_width(DisplayWidth(dc->dpy, screen));

    x = horizontal_position();
    y = vertical_position(topbar, DisplayHeight(dc->dpy, screen), height);




    /* menu window */
    wa.override_redirect = True;
    //wa.background_pixmap = ParentRelative;
    wa.event_mask = ExposureMask | KeyPressMask | VisibilityChangeMask;
    win = XCreateWindow(dc->dpy, root, x, y, width, height, 0,
            vinfo.depth, InputOutput,
            vinfo.visual,
            CWOverrideRedirect|CWEventMask|CWColormap|CWBorderPixel|CWBackPixel,
            &wa);
    dc->gc = XCreateGC(dc->dpy, win, 0, NULL);

    resizedc(dc, width, height, &vinfo, &wa);
    XMapRaised(dc->dpy, win);

    long pval = XInternAtom (dc->dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    long prop = XInternAtom (dc->dpy, "_NET_WM_WINDOW_TYPE", False);

    XChangeProperty (dc->dpy,
            win,
            prop,
            XA_ATOM,
            32,
            PropModeReplace,
            (unsigned char *) &pval,
            1);

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
    XChangeProperty (dc->dpy,
            win,
            prop,
            ptyp,
            16,
            PropModeReplace,
            (unsigned char *) &strut[0],
            12);

    /* This is for support with legacy WMs */
    prop = XInternAtom (dc->dpy, "_NET_WM_STRUT", False);
    unsigned long strut_s[4] = {0, 0, 0, 0};
    if (topbar)
        strut_s[2] = height;
    else
        strut_s[3] = height;
    XChangeProperty (dc->dpy, win, prop, ptyp, 32,
            PropModeReplace, (unsigned char *) &strut_s[0], 4);

    /* Appear on all desktops */
    prop = XInternAtom (dc->dpy, "_NET_WM_DESKTOP", False);
    long all_desktops = 0xffffffff;
    XChangeProperty(dc->dpy, win, prop, ptyp, 32,
            PropModeReplace, (unsigned char *) &all_desktops, 1);

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

