/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdint.h>
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
#define DEBUG(str) if (__debug__){puts(str);}
#define IS_ID(x, a) (!(strncmp(x->id, a, strlen(a))))


static void run(void);
static void setup(void);
static void config_to_layout(void);
void update_nba(baritem *item);
static void infer_type(block *conf_inf, baritem *ipl);
static int height = 0;
static int width  = 0;
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
static int __debug__;

void free_stylized(void *ste_v) {
    element *ste = ste_v;
    switch(ste->opt) {
        case 0:
            free(ste->text->color);
            free(ste->text->text);
            free(ste->text);
            break;
        case 1:
            free(ste->graph->color);
            free(ste->graph->xys);
            free(ste->graph);
            break;
    }
    free(ste);
}

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
        DEBUG("no font-color.");
        DEBUG("please add a {fontcolor #xxxxxx} option to your config gile");
        DEBUG("Exiting...");
        exit(1);
    }

    bar_font_colour = configuration->font_color;

    DEBUG("initializing mouse interaction");
    init_mouse();
    DEBUG("done initializing mouse interaction");

    DEBUG("initializing layout");
    config_to_layout();
    DEBUG("done initializing layout");

    DEBUG("running main loop");
    run();

    DEBUG("run exited. failure. exiting");
    return EXIT_FAILURE;
}


// update the item's string contents
void update_nba(baritem *item) {
    if (item->update) {
        if ((item->elements != NULL)) {
            dlist_deep_free_custom(item->elements, &free_stylized);
        }
        item->elements = (item->update)(item);
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
    result->default_colors  = make_baritem_colours(block->forground, block->background);
    result->format = block->format;
    result->source = block->source;
    result->shell = block->shell_click;
    result->elements = dlist_new();
    result->options = block->map;
    result->click = NULL;
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
dlist *questions(baritem *meh) {
    (void)meh;
    return dlist_new();
}

void shell_exec(baritem *item, int xpos) {
    (void) xpos;
    system(item->shell);
}

// set the function that creates information
void infer_type(block *conf_inf, baritem *ipl) {
    ipl->update = &questions;
    if (ipl->shell) {
        ipl->click = &shell_exec;
    }

    if (IS_ID(conf_inf, "workspace")) {
        spawn_vdesk_thread(ipl);
        ipl->update = NULL;
        ipl->elements = get_desktops_info(ipl);
    } else if (IS_ID(conf_inf, "clock")) {
        ipl->update = &get_time_format;
        set_timeout(ipl);
    } else if (IS_ID(conf_inf, "active")) {
        if (!strncmp(conf_inf->source, "window_title", 12)) {
            ipl->update = &get_active_window_name;
        } else {
            DEBUG("unrecognized active source");
            DEBUG(conf_inf->source);
        }
    } else if (IS_ID(conf_inf, "text")) {
        ipl->update = &get_plain_text;
    } else if (IS_ID(conf_inf, "weather")) {
        spawn_weather_thread(ipl);
        ipl->update = NULL;
        ipl->elements = get_weather(ipl);
    } else if (IS_ID(conf_inf, "scale")) {
        if (!strncmp(conf_inf->source, "battery", 7)) {
            ipl->update = &get_battery;
        } else if (!strncmp(conf_inf->source, "alsa", 4)) {
            ipl->update = &get_volume_level;
        }
    } else if (IS_ID(conf_inf, "graph")) {
        //ipl->update = &get_net_graph;
    } else if (IS_ID(conf_inf, "shell")) {
       ipl->update=&shell_cmd; 
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


void drawgraph(DC *dc, graph_element *element) {
    drawline(dc, dc->x, element->xy_count, element->xys);
}

unsigned int graphlength(graph_element *element) {
    unsigned int smallest = 9999999; // honestly, its just easier
    unsigned int largest = 0;
    unsigned int i = element->xy_count;
    int *el = element->xys;
    while(i --> 0) {
        if (*el > largest) {
            largest = *el;
        }
        if (*el < smallest) {
            smallest = *el;
        }
    }
    return largest - smallest;
}

unsigned int total_list_length(dlist *list) {
    uint len = 0;
    baritem *item;
    each(list, item) {
        item->length = 0;
        element *element;
        each(item->elements, element) {
            switch(element->opt) {
                case 0:
                    item->length += (element->length = textw(dc, element->text->text));
                    break;
                case 1:
                    item->length += (element->length = graphlength(element->graph));
                    break;
            }
        }
        len += item->length;
    }
    return len;
}

void draw_list(dlist *list) {
    baritem *item;
    element *elem;
    each(list, item) {
        item->xstart = dc->x;
        each(item->elements, elem) {
            dc->w = elem->length;
            switch(elem->opt) {
                case 0:
                    drawtext(dc, elem->text->text, elem->text->color);
                    break;
                case 1:
                    drawgraph(dc, elem->graph);
            }
            dc->x += dc->w;
        }
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
            "[date]\n"
            "  id text\n"
            "  timeout 1\n"
            "  source clock\n"
            "  format %%T  %%a - %%d\n"
            "[[bar]]\n"
            "  fontcolor #ffffff\n"
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
    __debug__ = has_options("debug", configuration);

    DEBUG("DEBUG_INIT");

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
    NET_DESKTOP_NAMES   = XInternAtom(dc->dpy, "_NET_DESKTOP_NAMES", 0);
    _UTF8_STRING_       = XInternAtom(dc->dpy, "UTF8_String", 0);
    _CARDINAL_ = XA_CARDINAL;

    XSelectInput(dc->dpy, win, ExposureMask);
}


int get_x11_cardinal_property(Atom at, Atom type) {
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

dlist *get_x11_cpp_property(Atom at) {
    Atom type_ret;
    int format_ret = 0;
    unsigned long n = 0,
                  b = 0;
    unsigned char *prop = 0;
    int result;

    result = XGetWindowProperty(dc->dpy, root, at, 0, 0x7fffffff,
            0, AnyPropertyType, &type_ret, &format_ret,
            &n, &b, &prop);

    if ((result == Success) && prop) {
        dlist *list = dlist_new();
        unsigned int i = 0;
        int new = 1;
        while(i < n) {
            if (new) {
                dlist_add(list, strdup((char *)(prop+i)));
            }
            new = !(prop[i]);
            i++;
        }
        XFree(prop);
        return list;
    }
    return NULL;
}
