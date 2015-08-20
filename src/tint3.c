/*
 * Copyright (C) 2015 Ted Meyer
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
#include "system.h"
#include "confparse.h"
#include "lwbi.h"
#include "vdesk.h"
#include "weather.h"
#include "tint3.h"
#include "netinfo.h"
#include "mouse.h"
#include "dlist.h"

#define INRECT(x,y,rx,ry,rw,rh) ((x) >= (rx) && (x) < (rx)+(rw) && (y) >= (ry) && (y) < (ry)+(rh))
#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))

#define IS_ID(x, a) (!(strncmp(x->id, a, strlen(a))))


static void run(void);
static void setup(void);
static void config_to_layout(void);
void update_nba(baritem *item);
static void infer_type(block *conf_inf, baritem *ipl);

static int height = 0;
static int width  = 0;

static char *quest = "???";

static unsigned long bar_background_colour;
static unsigned long bar_border_colour;
static char *bar_font_colour;

static unsigned long timeout = 60000000;

static bar_config *configuration;
static bar_layout *layout;


static pthread_mutex_t lock;


const char *font = "sakamoto-11";
Window win;
int topbar = 1;


// get the height of the bar
int get_bar_height(int font_height) {
    return font_height-1
        + 2*(configuration->padding_size
                + configuration->border_size);
}

// get the bar width
int get_bar_width(int display_width) {
    return display_width - 2 * configuration->margin_size;
}

int scale_to(int from, int to, float by) {
    float f = (to-from) * by;
    return to-f;
}

int main() {
    //mod_init();
    //load_module("./test.so");

    XInitThreads();
    pthread_mutex_init(&lock, NULL);
    setup();
    
    if (configuration->background_color != NULL) {
        bar_background_colour = getcolor(dc, configuration->background_color);
    }
    if (configuration->border_color != NULL) {
        bar_border_colour = getcolor(dc, configuration->border_color);
    }
    if (configuration->font_color == NULL) {
        exit(1);
    }
    bar_font_colour = configuration->font_color;

    init_mouse();

    config_to_layout();

    run();

    return EXIT_FAILURE;
}


// update the item's string contents
void update_nba(baritem *item) {
    if (item->update) {
        if ((item->string != NULL) && (item->string != quest)) {
            free(item->string);
        }
        item->string = (item->update)(item);
    }
}

ColorSet *make_baritem_colours(char *fg, char *bg) {   
    ColorSet *result = malloc(sizeof(ColorSet));
    result->FG = (fg) ? getcolor(dc, fg) : getcolor(dc, bar_font_colour);
    result->BG = (bg) ? getcolor(dc, bg) : bar_background_colour;
    if(dc->font.xft_font) {
        if(!XftColorAllocName(dc->dpy, DefaultVisual(dc->dpy, DefaultScreen(dc->dpy)),
            DefaultColormap(dc->dpy, DefaultScreen(dc->dpy)), fg?fg:bar_font_colour, &result->FG_xft)) {
        }
    }
    return result;
}

// turn a single item from the config stream into a displayable item
baritem *makeitem(block *block) {
    baritem *result = malloc(sizeof(baritem));
    result->invert = make_baritem_colours("#000000", "#ffffff");
    result->color  = make_baritem_colours(block->forground, block->background);
    result->format = block->format;
    result->source = block->source;
    result->click = NULL;
    result->mouseover = NULL;
    result->mouse_exit = NULL;
    result->string = quest;
    result->options = block->map;
    result->inverted = 0;
    result->xstart = 0;
    result->length = 0;
    infer_type(block, result);
    update_nba(result);
    return result;
}

#define is_digit(x) ((x)<='9' && (x)>='0')
void set_timeout(baritem *ipl) {
    char *set = get_baritem_option("timeout", ipl);
    unsigned short t_timeout = 0;
    if (set) {
        while(*set) {
            t_timeout *= 10;
            if (!is_digit(*set)) {
                return;
            }
            t_timeout += (*set - '0');
            set++;
        }

        if (t_timeout < timeout/1000000) {
            timeout = t_timeout * 1000000;
        }
    }
}

char *get_baritem_option(char *opt_name, baritem* item) {
    entry *every;
    each(item->options, every) {
        if (!strcmp(opt_name, every->key)) {
            return every->value;
        }
    }
    return NULL;
}

// fallback, in case no other source can be found
char *questions(baritem *meh) {
    return meh?quest:NULL;
}

// set the function that creates information
void infer_type(block *conf_inf, baritem *ipl) {
    ipl->update = &questions;

    if (IS_ID(conf_inf, "radio")) {
        if (!strncmp(conf_inf->source, "workspaces", 10)) {
            spawn_vdesk_thread(ipl);
            ipl->update = NULL;
            ipl->string = get_desktops_info(ipl);
        }
    } else if (IS_ID(conf_inf, "text")) {
        if (!strncmp(conf_inf->source, "clock", 5)) {
            ipl->update = &get_time_format;
            set_timeout(ipl);
        } else if (!strncmp(conf_inf->source, "window_title", 12)) {
            ipl->update = &get_active_window_name;
        } else {
            ipl->update = &get_plain_text;
        }
    } else if (IS_ID(conf_inf, "weather")) {
        spawn_weather_thread(ipl);
        ipl->update = NULL;
        ipl->string = get_weather(ipl);
        if (has_options("details", configuration)) {
            //ipl->click = &show_details;
        }
    } else if (IS_ID(conf_inf, "scale")) {
        if (!strncmp(conf_inf->source, "battery", 7)) {
            ipl->update = &get_battery;
        } else if (!strncmp(conf_inf->source, "alsa", 4)) {
            ipl->update = &get_volume_level;
            ipl->click = &toggle_mute;
            ipl->mouseover = &expand_volume;
            ipl->mouse_exit = &leave_volume;
        }
    } else if (IS_ID(conf_inf, "graph")) {
        ipl->update = &get_net_graph;
    }
}

// gets the item by 
baritem *item_by_coord(uint x) {
    baritem *bar;
    each(layout->left, bar) {
        uint st = bar->xstart;
        uint en = bar->xstart+bar->length;
        if(x>=st && x<=en) {
            return bar;
        }
    }

    each(layout->right, bar) {
        uint st = bar->xstart;
        uint en = bar->xstart+bar->length;
        if(x>=st && x<=en) {
            return bar;
        }
    }

    each(layout->center, bar) {
        uint st = bar->xstart;
        uint en = bar->xstart+bar->length;
        if(x>=st && x<=en) {
            return bar;
        }
    }

    return NULL;
}

// convert a config to a drawable
dlist *config_to_drawable(dlist *bid) {
    dlist *result = dlist_new();
    block *block;
    each(bid, block) {
        dlist_add(result, makeitem(block));
    }
    return result;
}



void config_to_layout() {
    layout = malloc(sizeof(bar_layout));
    layout->left = config_to_drawable(configuration->left);
    layout->right = config_to_drawable(configuration->right);
    layout->center = config_to_drawable(configuration->center);
}


void update_list_of_items(dlist *list) {
    baritem *item;
    each(list, item) {
        update_nba(item);
    }
}

void update_with_lens() {
    update_list_of_items(layout->left);
    update_list_of_items(layout->right);
    update_list_of_items(layout->center);

    layout->leftlen = total_list_length(layout->left);
    layout->rightlen = total_list_length(layout->right);
    layout->centerlen = total_list_length(layout->center);
}






void drawmenu(void) {
    pthread_mutex_lock(&lock);
    dc->x = 0;
    dc->y = 0;
    dc->w = 0;
    dc->h = height;

    if (bar_border_colour) {
        draw_rectangle(dc, 0, 0, width+2, height+2, True, bar_border_colour);
    }

    draw_rectangle(dc, configuration->border_size, configuration->border_size,
            width-2*configuration->border_size,
            height-2*configuration->border_size, True, bar_background_colour);


    update_with_lens();


    dc->x = dc->color_border_pixels;
    draw_list(layout->left);
    dc->x = width-(layout->rightlen)-dc->color_border_pixels;
    draw_list(layout->right);
    dc->x = (width-(layout->centerlen))/2;
    draw_list(layout->center);

    mapdc(dc, win, width, height);
    pthread_mutex_unlock(&lock);
}




unsigned int total_list_length(dlist *list) {
    uint len = 0;
    baritem *item;
    each(list, item) {
        len += (item->length = textw(dc, item->string));
    }
    return len;
}

void draw_list(dlist *list) {
    baritem *item;
    each(list, item) {
        dc->w = item->length;
        if (item->inverted) {
            drawtext(dc, item->string, item->invert);
        } else {
            drawtext(dc, item->string, item->color);
        }
        item->xstart = dc->x;
        dc->x += dc->w;
    }
}

void run(void) {
    XEvent xe;
    int xlib_debug = has_options("xldb", configuration);
    while(1){
        while(QLength(dc->dpy)) {
            if (xlib_debug) printf("%i\n", QLength(dc->dpy));
            XNextEvent(dc->dpy, &xe);
            if (xlib_debug)printf("%i\n", xe.type);
        }

        drawmenu();
        usleep(timeout);
    }
}




// gets the vertical position of the bar, depending on margins and position
int vertical_position(Bool bar_on_top, int display_height, int bar_height) {
    if (bar_on_top) {
        return configuration->margin_size;
    } else {
        return display_height - (bar_height + configuration->margin_size);
    }
}

int horizontal_position() {
    return configuration->margin_size;
}



void write_default(FILE *fp) {
    fprintf(fp,
            "[active]\n"
            "  id text\n"
            "  source window_title\n"
            "  forground #ffffff\n"
            "[date]\n"
            "  id text\n"
            "  source clock\n"
            "  format %%T  %%a - %%d\n"
            "  forground #ffffff\n"
            "[[bar]]\n"
            "  bordercolor #ffffff\n"
            "  background #000000\n"
            "  borderwidth 0\n"
            "  padding 2\n"
            "  margin 5\n"
            "  location top\n"
            "  center\n"
            "    active\n"
            "    date\n");
}



FILE *test_set_config() {
    char home[100] = {0};
    char conf[100] = {0};
    snprintf(home, 100, "%s/.tint3rc", getenv("HOME"));
    snprintf(conf, 100, "%s/.config/tint3/tint3rc", getenv("HOME"));

    FILE *fp_home = fopen(home, "r");
    if (fp_home) {
        return fp_home;
    }

    FILE *fp_conf = fopen(conf, "r");
    if (fp_conf) {
        return fp_conf;
    }

    FILE *fp_new = fopen(home, "w");
    if (fp_new) {
        write_default(fp_new);
        fclose(fp_new);

        fp_home = fopen(home, "r");
        if (fp_home) {
            return fp_home;
        }
    }

    return NULL;
}




// TODO: clean this shit
void setup() {

    configuration = build_bar_config(test_set_config());

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
    dc->wa = wa;
    initfont(dc, font ? font : "fixed");

    dc->border_width = configuration->margin_size;
    dc->color_border_pixels = configuration->border_size;
    dc->text_offset_y = configuration->padding_size; 

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

    NET_CURRENT_DESKTOP = XInternAtom(dc->dpy, "_NET_CURRENT_DESKTOP", 0);
    NET_NUMBER_DESKTOPS = XInternAtom(dc->dpy, "_NET_NUMBER_OF_DESKTOPS", 0);
    _CARDINAL_ = XA_CARDINAL;

    XSelectInput(dc->dpy, win, ExposureMask);
}


int get_x11_property(Atom at, Atom type) {
    Atom type_ret;
    int format_ret = 0, data = 1;
    unsigned long nitems_ret = 0,
                  bafter_ret = 0;
    unsigned char *prop_value = 0;
    int result;

    result = XGetWindowProperty(dc->dpy, root, at, 0, 0x7fffffff,
            0, type, &type_ret, &format_ret,
            &nitems_ret, &bafter_ret, &prop_value);

    if (result == Success && prop_value) {
        data = ((unsigned long *) prop_value)[0];
        XFree(prop_value);
    }

    return data;
}

