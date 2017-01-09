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
#include <malloc.h>

#include "confparse/confparse.h"
#include "netinfo/netinfo.h"
#include "colors/colors.h"
#include "dlist/dlist.h"
#include "lwi/lwbi.h"

#include "weather.h"
#include "suggest.h"
#include "system.h"
#include "mouse.h"
#include "vdesk.h"
#include "tint3.h"
#include "draw.h"

/* Macros */
#define INRECT(x,y,a,b,c,d)((x)>=(a)&&(x)<(a)+(c)&&(y)>=(b)&&(y)<(b)+(d))
#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))
#define DEBUG(str) if (__debug__){puts(str);}

/* Functions */
static void run(void);
static void config_to_layout(void);
void update_nba(baritem *item);

/* Variables */
static int height = 0;
static int width  = 0;
static char *config_path = NULL;
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
static int __valgrind__;

/* Implementation */

void free_stylized(void *ste_v) {
    element *ste = ste_v;
    switch(ste->opt) {
        case 0:
            free(ste->text->color);
            free(ste->text->text);
            free(ste->text);
            break;
        case 1:
            free(ste->graph->colors);
            free(ste->graph->data);
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
    if (configuration->width != 0) {
        return configuration->width;
    } else {
        return display_width
             - configuration->margin_right 
             - configuration->margin_left;
    }
}

int scale_to(int from, int to, float by) {
    float f = (to-from) * by;
    return to-f;
}

// Main
int tint3_main(int argc, char *argv[]) {
    XInitThreads();
    pthread_mutex_init(&lock, NULL);

    // parse params
    char opt;
    while ((opt = getopt(argc, argv, "hc:")) != (char)-1) {
        switch (opt) {
            case 'h':
                printf ("Usage : [-h | -c CONFIG_PATH]\n");
                exit(EXIT_SUCCESS);
                break;
            case 'c':
                config_path = optarg;
                printf ("Config path is : %s \n", config_path);
                break;
        }
    }

    setup();

    if (configuration->background_color != NULL) {
        bar_background_colour = getcolor(dc, configuration->background_color);
    }
    if (configuration->border_color != NULL) {
        bar_border_colour = getcolor(dc, configuration->border_color);
    }
    if (configuration->font_color == NULL) {
        DEBUG("no font-color.");
        DEBUG("please add a {fontcolor #xxxxxx} option to your config file");
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

// turn a single item from the config stream into a displayable item
baritem *makeitem(block *block) {
    baritem *result = malloc(sizeof(baritem));
    result->default_colors =
        make_baritem_colours(block->forground, block->background);
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
    result->name = block->name;
    return result;
}

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

void shell_exec(baritem *item, int xpos) {
    (void) xpos;
    system(item->shell);
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

void drawlock(void ) {
    pthread_mutex_lock(&lock);
}

void drawunlock(void) {
    pthread_mutex_unlock(&lock);
}

// Draw the bar
void drawmenu(void) {
    drawlock();
    struct mallinfo init = mallinfo();
    printf("%i\n", init.uordblks);
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

    if (!__valgrind__) {
        dc->x = dc->color_border_pixels;
        draw_list(layout->left);
        dc->x = width-(layout->rightlen)-dc->color_border_pixels;
        draw_list(layout->right);
        dc->x = (width-(layout->centerlen))/2;
        draw_list(layout->center);
    }

    mapdc(dc, win, width, height);
    drawunlock();
}


void drawgraph(DC *dc, graph_element *element) {
    for(size_t i=0;i<element->rows; i++) {
        drawline(
                dc
               ,element->colors[i]
               ,dc->x
               ,element->cols
               ,element->data + (element->cols * i * 2)
        );
    }
}

unsigned int graphlength(graph_element *element) {
    (void)element;
    return 100;
}

unsigned int total_list_length(dlist *list) {
    uint len = 0;
    baritem *item;
    each(list, item) {
        item->length = 0;
        element *element;
        if (item->elements) {
            each(item->elements, element) {
                switch(element->opt) {
                    case 0:
                        item->length +=
                            (element->length=textw(dc, element->text->text));
                        break;
                    case 1:
                        item->length +=
                            (element->length = graphlength(element->graph));
                        break;
                }
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
        if (item->elements) {
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
int vertical_position(location bar_on_top, int display_height, int bar_height) {
    if (bar_on_top == TOP) {
        return configuration->margin_top;
    } else {
        return display_height - (bar_height + configuration->margin_bottom);
    }
}

int horizontal_position() {
    if (configuration->margin_left != 0) {
        return configuration->margin_left;
    } else if (configuration->margin_right != 0) {
        return 0 - configuration->margin_right;
    } else {
        return 0;
    }
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
    if (config_path != NULL) {
        if( access( config_path, F_OK ) == -1 ) {
            printf("The provided config file does not exist ! \n");
            exit(EXIT_FAILURE);
        }

        char arg_conf[100] = {0};

        snprintf(arg_conf, 100, config_path);

        FILE *fp_arg_conf = fopen(arg_conf, "r");

        if (fp_arg_conf) {
            return fp_arg_conf;
        }
    } else {
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
    }
    return NULL;
}

void set_dc(DC *_dc) {
    dc = _dc;
}

DC *getdc() {
    return dc;
}

void set_root(Window w) {
    root = w;
}

void setup(void) {

    configuration = build_bar_config(test_set_config());
    __debug__ = has_options("debug", configuration);
    __valgrind__ = has_options("valgrind", configuration);

    if (configuration->font_name) {
        font = configuration->font_name;
    }

    if (__debug__) {
        printf("valgrind is %i\n", __valgrind__);
    }

    DEBUG("DEBUG_INIT");

    set_dc(initdc());
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

    dc->border_width = configuration->border_size;
    dc->color_border_pixels = configuration->border_size;
    dc->text_offset_y = configuration->padding_size; 

    int x, y;

    int screen = DefaultScreen(dc->dpy);
    set_root(RootWindow(dc->dpy, screen));

    /* menu geometry */
    height = get_bar_height(dc->font.height);
    width  = get_bar_width(DisplayWidth(dc->dpy, screen));

    x = horizontal_position();
    y = vertical_position(
            configuration->location
            ,DisplayHeight(dc->dpy, screen)
            ,height);



    /* menu window */
    wa.override_redirect = True;
    wa.event_mask = 0;
    win = XCreateWindow(dc->dpy, root, x, y, width, height, 0,
            vinfo.depth, InputOutput,
            vinfo.visual,
            CWOverrideRedirect|CWEventMask|CWColormap|CWBorderPixel|CWBackPixel
            ,&wa);
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

    XSelectInput(dc->dpy, win, wa.event_mask);
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

unsigned long get_bar_background_colour() {
    return bar_background_colour;
}

char *get_bar_font_colour() {
    return bar_font_colour;
}
