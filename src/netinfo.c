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

ColorSet **t_colors = NULL;
dlist *get_multi_net_graph(baritem *item) {
    update_network(item->source+14); //update network
    char *target = get_baritem_option("targets", item);
    char *split, *targets[10] = {NULL}; // max of 10 targets
    size_t target_c = 0;
    do {
        split = strstr(target, " ");
        if (split) {
            *split = 0;
        }
        char *each = get_baritem_option(target, item);
        if (!each) {
            printf("warning - could not find target: %s\n", target);
            return NULL;
        }
        targets[target_c] = malloc(sizeof(char) * strlen(each));
        strcpy(targets[target_c], each);

        if (split) {
            *split = ' ';
            target = split+1;
        }
        target_c++;
    } while(split);

    size_t error = 0, gen_colors = 0;
    size_t W = target_c;
    if (!t_colors) {
        gen_colors = 1;
        t_colors = calloc(W, sizeof(ColorSet *));
    }

    combined_graph *cg = calloc(sizeof(combined_graph), 1);
    cg->rows = 0;
    cg->cols = 0;
    while(W --> 0) {
        char *space = strstr(targets[W], " ");
        if (!space) {
            puts("error; target string is \"[up/down] xN #color\"");
            error = 1;
            goto free_targets;
        }
        *space = 0;
        char *netname = malloc(strlen(targets[W])+strlen(item->source+14)+3);
        sprintf(netname, "%s %s", item->source+14, targets[W]);

        graph *graph = get_graph_by_name(netname);
        if (space[1] != 'x') {
            error = 1;
            free(netname);
            puts("error; target string is \"[up/down] xN #color\"");
            goto free_targets;
        }
        char *mult = space+2;
        int multiplier = 0;
        while(is_digit(*mult)) {
            multiplier *=10;
            multiplier +=(*mult-'0');
            mult++;
        }
        mult+=1;
        if (graph != NULL) {
            combine_graph_scaled(cg, graph, multiplier);
        }

        free(netname);
        if (gen_colors) {
            t_colors[W] = make_baritem_colours(mult, "#000000");
        }
    }

free_targets:
    W = target_c;
    while(W --> 0) {
        free(targets[W]);
    }
    if(error) {
        return NULL;
    }

    if (cg) {
        graph_element *ge = multigraph_create_scaled(cg);

        if (ge) {
            dlist *res = dlist_new();
            element *e = calloc(sizeof(element), 1);
            e->opt = 1;
            e->graph = ge;
            dlist_add(res, e);
            ge->colors = calloc(2, sizeof(void *));
            W = target_c;
            while(W --> 0) {
                (ge->colors)[W] = t_colors[W];
            }
            return res;
        }
    }

    return NULL;
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
    graph_element *a = get_scaled_graph_element_by_name(interface);
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

