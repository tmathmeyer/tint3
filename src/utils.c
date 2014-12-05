/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <xcb/xcb.h>
#include "utils.h"
#include "lwxt.h"
#include "lwbi.h"
#include "format.h"
#include "json.h"






typedef struct glist{
    struct glist * next;
    char * name;
    graph * data;
} glist;

glist * graphs = NULL;

graph * get_named_graph(char * name) {
    glist * temp = graphs;
    while(temp) {
        if (!strncmp(name, temp -> name, strlen(name))) {
            return temp -> data;
        } temp = temp -> next;
    }
    glist * addition = malloc(sizeof(glist));
    addition -> name = name;
    addition -> data = make_new_graph();
    addition -> next = graphs;
    graphs = addition;
    return addition -> data;
}

unsigned long long old_down=0, old_up=0;
unsigned long net_tick = 0;
void update_network(char * interface) {
    if (time(NULL)-net_tick > 2) {
        time((time_t *)&net_tick);

        FILE * fp = fopen("/proc/net/dev", "r");
        if (fp == NULL) {
            return;
        }

        char * buffer = malloc(4096), * bt = buffer, temp = 0;
        while( (temp=fgetc(fp)) != EOF) {
            *(bt++) = temp;
        }
        fclose(fp);
        bt = strstr(buffer, interface);
        if (bt == 0) {
            free(buffer);
            return;
        }
        bt += (strlen(interface) + 2);

        unsigned long long up, down;
        sscanf(bt, "%llu %llu", &down, &up);

        int ud = up-old_up, dd = down-old_down;

        char * netname = calloc(0, strlen(interface) + 4);
        snprintf(netname, strlen(interface)+4, "%s up", interface);

        if (old_down != 0 && old_up != 0) {
            add_to_graph(dd, get_named_graph(netname));
            netname[strlen(interface)+1] = 'd';
            netname[strlen(interface)+2] = 'o';
            add_to_graph(ud, get_named_graph(netname));
        }

        old_down = down;
        old_up = up;

        free(buffer);
        free(netname);
    }
}

int eot = 0;
char * get_net_graph(baritem * item) {
    char source2[20] = {0};
    snprintf(source2, 20, item -> source + 8);
    strstr(source2, " ")[0] = 0;
    update_network(source2);
    return graph_to_string(get_named_graph((item -> source)+8));
}

int get_number_of_desktops () {
    return get_x11_property(NET_NUMBER_DESKTOPS, _CARDINAL_);
}

int get_current_desktop () {
    return get_x11_property(NET_CURRENT_DESKTOP, _CARDINAL_);
}

char * get_desktops_info(baritem * source) {
    int numdesk = get_number_of_desktops();
    int curdesk = get_current_desktop();
    int swap = 0;

    int dsktplen = numdesk * 4 - 1;
    char * result = malloc(dsktplen);

    for(swap=0; swap < dsktplen; swap++) {
        int sqp = swap%4;
        result[swap] = sqp==3?' ':"◇"[sqp];
    }

    result[curdesk*4 + 0] = "◆"[0];
    result[curdesk*4 + 1] = "◆"[1];
    result[curdesk*4 + 2] = "◆"[2];

    return result;
}

char * get_active_window_name(baritem * source) {
    char * window_title = malloc(256); // max displayed window size
    get_title(window_title, 256);
    return window_title;
}

char * get_time_format(baritem * item) {
    char exec[100] = {0};
    snprintf(exec, sizeof exec, "date +'%s'", item -> format);
    FILE * desc = popen(exec, "r");
    char * msg = calloc(0, 20);
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
    return msg;
}













int show_humidity(int start, char * dest) {
    snprintf(dest+start, strlen("humidity")+1, "humidity");
    return start+8;
}
format_map * fmtmp;



void print_json(container * cont, char * name) {
    puts(name);
    switch(cont -> id) {
        case 0:
            printf("%s\n\n", cont -> string);
            break;
        case 1:
            printf("%i\n\n", *(cont -> number));
            break;
        case 2:
            printf("%s\n\n", (*(cont -> boolean)) ? "true" : "false");
            break;
        case 3:
            printf("an object\n\n");
    }
}

unsigned long lastime = 0;
char * weather = NULL;
char * get_weather(baritem * item) {
    if (time(NULL)-lastime > 1800) {
        time((time_t *)&lastime);

        time((time_t*)&lastime);
        int weather_parse_size = 4096;
        char * weather_s = malloc(weather_parse_size);
        char * host = "api.openweathermap.org";
        char * url  = "/data/2.5/weather?q=Worcester,usa";

        if (!url_to_memory(weather_s, weather_parse_size, url, host, "162.243.44.32")) {
            weather = malloc(8);
            snprintf(weather, 8, "<<err>>");
        } else if (strstr(weather_s, "HTTP/1.1 200")) {
            char * JSON = strstr(weather_s, "{");
            if (JSON) {
                container * json = from_string(&JSON);

                container * w_main = $(json, "main");
                container * temperature = $(w_main, "temp");
                int temp = *(temperature -> number);

                container * w_weather = $(json, "weather");
                container * first = _(w_weather, 0);
                container * sky = $(first, "main");
                char * sky_condition = sky -> string;


                weather = calloc(0, 100);
                snprintf(weather, 100, "%s, %i", sky_condition, temp);
                
                puts(weather);

                free_container(json);
                free(weather_s);
            }
        } else {
            weather = malloc(8);
            snprintf(weather, 8, "<<err>>");
        }
    }

    int length = strlen(weather);
    char * weather2 = malloc(length+1);
    snprintf(weather2, length+1, "%s", weather);
    return weather2;
}



























char * get_battery(baritem * item) {
    char batt[5] = "BAT0";
    char c = (item -> source)[8];
    batt[3] = c;
    char * msg = calloc(0,11);
    int battery_percent = get_battery_percent(batt);
    snprintf(msg, 11, "batt%c:%i%%", c, battery_percent);

    // change color here
    return msg;
}



char * get_volume_level(baritem * item) {
    char * pipe_format = "amixer get -c %s Master | tail -n 1 | cut -d '[' -f 2 | tr -d '%]'";
    char pipe[72] = {0};
    snprintf(pipe, 72, pipe_format, (item -> source)+5);
    FILE * pf = popen(pipe, "r");
    int i = 0;
    if (pf) {
        fscanf(pf, "%i", &i);
        fclose(pf);
    }
    char * result = malloc(5);
    snprintf(result, 5, "%i%%", i);
    return result;
}

char * get_plain_text(baritem * item) {
    int len = strlen(item -> source);
    char * result = malloc(len+1);
    snprintf(result, len+1, item -> source);
    return result;
}





char * bar_map = "▁▂▃▄▅▆▇";

void recalc_max(graph * gr) {
    int ctr = 0, i = 0;
    for(; i < GRAPHLENGTH; i++) {
        if ((gr -> graph)[i] > ctr) {
            ctr = (gr -> graph)[i];
        }
    }
    gr -> max = ctr;
}

void add_to_graph(int i, graph * gr) {
    int old = (gr -> graph)[gr -> start];
    (gr -> graph)[gr -> start] = i;
    if (i > gr -> max) {
        gr -> max = i;
    }
    if (old ==  gr -> max) {
        recalc_max(gr);
    }
    gr -> start = ((gr -> start)+1)%GRAPHLENGTH;
}

char * graph_to_string(graph * gr) {
    char * result = malloc(GRAPHLENGTH*3+1);
    int ctr = 0, i = 0;
    for(; i < GRAPHLENGTH*3+1; i++) {
        result[i] = 0;
    } i = 0;
    for(; i < GRAPHLENGTH; i++) {
        int val = (gr -> graph)[((gr -> start)+i)%GRAPHLENGTH]*7/((gr -> max)+1) + 1;
        if (val < 0) {
            val = 0;
        }
        result[ctr++] = bar_map[(val-1)*3+0];
        result[ctr++] = bar_map[(val-1)*3+1];
        result[ctr++] = bar_map[(val-1)*3+2];
    }
    result[GRAPHLENGTH*3] = 0;
    return result;
}

graph * make_new_graph() {
    graph * g = NULL;
    g = malloc(sizeof(graph));
    g -> start = 0;
    g -> max = 0;
    int i=0; for(; i<GRAPHLENGTH; i++) {
        (g->graph)[i] = 0;
    }
    return g;
}