
#ifndef __UTILS__
#define __UTILS__

#include <X11/Xft/Xft.h>

#define GRAPHLENGTH 10

#define UNUSED(x) (void)(x)


typedef struct {
    unsigned long FG;
    XftColor FG_xft;
    unsigned long BG;
} ColorSet;

typedef struct baritem {
    ColorSet * color;
    char     * string;
    char       type; // used to determine the freeing mechanism
    unsigned int length; // length IN PIXELS
} baritem;

typedef struct itemlist {
    baritem * item;
    struct itemlist * next;
} itemlist;

typedef struct batt_info {
    char * icon;
    int  percentage;
} batt_info;

typedef struct net_info {
    char * graph;
    int  percentage;
} net_info;

typedef struct graph {
    int graph[GRAPHLENGTH*3+1];
    int start;
    int max;
} graph;



batt_info * get_battery_information();
itemlist * config_to_list (char * list);
baritem * char_to_item(char c);
void free_list(itemlist * list);
void free_baritem(baritem * item);
void draw_list(itemlist * list);
unsigned int total_list_length(itemlist * list);
char ** get_net_info(void);
void add_to_graph(int i, graph * gr);
char * graph_to_string(graph * gr);
graph * make_new_graph();

#endif