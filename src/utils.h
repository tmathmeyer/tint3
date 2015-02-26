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

#define GRAPHLENGTH 15

#define UNUSED(x) (void)(x)

typedef struct ColorSet{
    unsigned long FG;
    XftColor FG_xft;
    unsigned long BG;
} ColorSet;

typedef struct baritem{
    pthread_t thread_id;
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

typedef struct graph{
    int graph[GRAPHLENGTH*3+1];
    int start;
    int max;
} graph;

typedef struct {
    unsigned int timeout;
    unsigned long lastime;
    graph * up;
    graph * down;
} net_info;


itemlist * config_to_list (char * list);
baritem * char_to_item(char c);
baritem * weather_s();
void free_list(itemlist * list);
void free_baritem(baritem * item);
void draw_list(itemlist * list);
unsigned int total_list_length(itemlist * list);
net_info * get_net_info(void);
void add_to_graph(int i, graph * gr);
char * graph_to_string(graph * gr);
graph * make_new_graph();
void host_to_ip(char *ptr, char* address);
int get_socket(int port_number, char* ip);
char * generate_header(char * url, char * host);
int url_to_memory(char * buffer, int buf_size, char * url, char * host, char * ip);
int get_x11_property(Atom at, Atom type);



char * get_time_format(baritem * item);
char * get_weather(baritem * item);
char * get_battery(baritem * item);
char * get_volume_level(baritem * item);
char * get_desktops_info(baritem * item);
char * get_active_window_name(baritem * item);
char * get_net_graph(baritem * item);
char * get_plain_text(baritem * item);
char * get_scrolling_text(baritem * item);

Atom NET_NUMBER_DESKTOPS,
     NET_CURRENT_DESKTOP;

Atom _CARDINAL_;




/* SPECIAL CHARACTERS */
//⮎
//⮒
//⮏
//⮑
//⮐
//▉ <- full thermometer
//▊ <- medium thermometer
//▋ <- low thermometer
//▌ <- humidity symbol <- lol what the fuck



#endif
