
#ifndef __UTILS__
#define __UTILS__

#include <X11/Xft/Xft.h>

#define GRAPHLENGTH 15

#define UNUSED(x) (void)(x)


typedef struct ColorSet{
    unsigned long FG;
    XftColor FG_xft;
    unsigned long BG;
} ColorSet;

typedef struct baritem{
    ColorSet * color;
    char     * string;
    char       type; // used to determine the freeing mechanism
    unsigned int length; // length IN PIXELS
} baritem;

typedef struct itemlist {
    baritem * item;
    struct itemlist * next;
} itemlist;

typedef struct batt_info{
    char * icon;
    int  percentage;
} batt_info;

typedef struct graph{
    int graph[GRAPHLENGTH*3+1];
    int start;
    int max;
} graph;

typedef struct weather_info {
    unsigned int timeout;
    unsigned long lastime;
    int temperature;
    int humidity;
    char * condition;
} weather_info;

typedef struct {
    unsigned int timeout;
    unsigned long lastime;
    graph * up;
    graph * down;
} net_info;



batt_info * get_battery_information();
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
void url_to_memory(char * buffer, int buf_size, char * url, char * host, char * ip);
char * get_desktops_info();




weather_info * get_weather();


/* SPECIAL CHARACTERS */
//⮎
//⮒
//⮏
//⮑
//⮐
//▉ <- full thermometer
//▊ <- medium thermometer
//▋ <- low thermometer
//▌ <- humidity symbol



#endif