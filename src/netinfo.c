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

        if (old_down != 0 && old_up != 0) {
            write_graph_value("up", (float)ud);
            write_graph_value("down", (float)dd);
        }

        old_down = down;
        old_up = up;

        free(buffer);
    }
}

dlist *get_net_graph(baritem *item) {
    char source2[20] = {0};
    snprintf(source2, 20, item->source);
    strstr(source2, " ")[0] = 0;
    update_network(source2);
    graph_element *a = get_named((item -> source)+7);
    element *e = calloc(sizeof(element), 1);
    e->opt = 1;
    e->graph = a;
    dlist *res = dlist_new();
    if (a) {
        dlist_add(res, e);
        a->color = item->default_colors;
    } else {
        free(e);
        dlist_free(res);
    }
    return res;
}

