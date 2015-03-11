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
#include "draw.h"
#include "scrolling.h"
#include "weather.h"
#include "format.h"

#define MAX_WINDOW_TITLE_LENGTH 256


#define BATTERY_FOREGROUND_HIGH "#0f0"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#f00"
#define BATTERY_BACKGROUND_LOW "#222222"



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





static int current_desktop = 0;
static fmt_map *formatmap = 0;
static char *romans[10] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"};
static int roman_le[10] = {1, 2, 3, 2, 1, 2, 3, 4, 2, 1};
uint16_t strcons(char *dest, const char *src, uint16_t ctr) {
    uint res = ctr;
    while(ctr --> 0) {
            dest[ctr] = src[ctr];
    }
    return res;
}

int _arabic_numerals(int place, char * string) {
    return place + sprintf(string+place, "%i", current_desktop);
}

int _roman_numerals(int place, char * string) {
    return place + strcons(string+place, romans[current_desktop-1], roman_le[current_desktop-1]);
}

char * get_desktops_info(baritem * source) {
    if (!source) {
        return NULL;
    }
    if (!(source -> format)) {
        perror("It seems you've not included a format string for the virtual desktop information.");
        perror("Please read the changes to configuration.md, github.com/tmathmeyer/tint3.");
        return NULL; // may change this later
    }
    if (!formatmap) {
        formatmap = initmap(8);
        fmt_map_put(formatmap, 'N', &_arabic_numerals);
        fmt_map_put(formatmap, 'R', &_roman_numerals);
    }

    current_desktop = 1;
    char default_desktop[10] = {0};
    char selected_desktop[10] = {0};
    uint8_t sel = 0;
    uint8_t wpos = 0;
    uint8_t rpos = 0;
    int numdesk = get_number_of_desktops();
    int curdesk = get_current_desktop();
    char tmp_bfr[32] = {0};
    uint8_t ctr = 0;
    uint16_t offset = 0;
    while((source->format)[rpos]) {
        if ((source->format)[rpos] == ' ') {
            sel = 1;
            wpos = 0;
            rpos++;
            continue;
        }
        (sel?selected_desktop:default_desktop)[wpos++] = (source->format)[rpos++];
    }
    
    while(ctr < numdesk) {
        if (ctr == curdesk) {
            offset += format_string(tmp_bfr+offset, selected_desktop, formatmap);
        } else {
            offset += format_string(tmp_bfr+offset, default_desktop, formatmap);
        }
        offset += strcons(tmp_bfr+offset, " ", 1);
        current_desktop++;
        ctr++;
    }

    char *result = calloc(offset+1, sizeof(char));
    memcpy(result, tmp_bfr, offset);
    return result;
}


char * get_active_window_name(baritem * source) {
    if (!source) {
        return NULL;
    }
    char * window_title = malloc(MAX_WINDOW_TITLE_LENGTH);
    get_title(window_title, MAX_WINDOW_TITLE_LENGTH);
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

char * get_weather(baritem * item) {
    return get_weather_string(item -> format, item -> source);
}

char * get_battery(baritem * item) {
    char batt[5] = "BAT0";
    char c = (item -> source)[8];
    batt[3] = c;
    char * msg = calloc(0,9);
    int battery_percent = get_battery_percent(batt);
    char * query = (c-'0') ? "╻:%i%%" : "╺:%i%%";
    snprintf(msg, 9, query, battery_percent);

    free(item -> color);

    if (battery_percent > 80) {
        item -> color = initcolor(dc, "#000000", BATTERY_FOREGROUND_HIGH);
    } else if (battery_percent > 20) {
        item -> color = initcolor(dc, "#000000", BATTERY_FOREGROUND_MED);
    } else {
        item -> color = initcolor(dc, "#000000", BATTERY_FOREGROUND_LOW);
    }

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

rotation * ROT = NULL;
char * get_scrolling_text(baritem * item) {
    if (ROT == NULL) {
        ROT = make_rotation(item -> source, 30);
    }

    char * res = malloc(ROT -> size);
    strncpy(res, (ROT -> strfull + ROT -> start), ROT -> size);

    update_rotation(ROT);

    return res;
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

