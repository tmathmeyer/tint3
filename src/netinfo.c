/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <time.h>
#include "graph.h"
#include "netinfo.h"

unsigned long long old_down=0, old_up=0;
unsigned long net_tick = 0;

void update_network(char *interface) {
    if (time(NULL)-net_tick > 0) { // TODO redesign
        time((time_t *)&net_tick);

        FILE *fp = fopen("/proc/net/dev", "r");
        if (fp == NULL) {
            return;
        }

        char *buffer = malloc(4096), *bt = buffer, temp = 0;
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
        graph_t UP, DOWN;
        UP._i = ud;
        DOWN._i = dd;


        if (old_down != 0 && old_up != 0) {
            char *graph = malloc(strlen(interface)+6);
            sprintf(graph, "%s down", interface);
            write_graph_value(graph, DOWN);
            sprintf(graph, "%s up", interface);
            write_graph_value(graph, UP);
            free(graph);
        }

        old_down = down;
        old_up = up;

        free(buffer);
    }
}

dlist *get_net_graph(baritem *item) {
#define MAX_INTERFACE 20
    char interface[MAX_INTERFACE] = {0};
    //8 is the length of "network"
    snprintf(interface, MAX_INTERFACE, item->source+8);
    char *block = strstr(interface, " ");
    *block = 0; // break the interface and type
    update_network(interface);
    *block = ' ';
    graph_element *a = get_element_by_name(interface);
    dlist *res = dlist_new();
    if (a) {
        element *e = calloc(sizeof(element), 1);
        e->opt = 1;
        e->graph = a;
        dlist_add(res, e);
        a->colors = calloc(1, sizeof(void *));
        a->colors[0] = item->default_colors;
    } else {
        dlist_free(res);
        res = NULL;
    }
    return res;
}

