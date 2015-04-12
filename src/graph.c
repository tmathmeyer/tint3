/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */
#define _DEFAULT_SOURCE

#include "graph.h"

static const char *bar_map = "▁▂▃▄▅▆▇";
static glist *graphs = NULL;

graph * get_named_graph(char * name) {
    glist * temp = graphs;
    while(temp) {
        if (!strncmp(name, temp -> name, strlen(name))) {
            return temp -> data;
        }
        temp = temp -> next;
    }
    glist * addition = malloc(sizeof(glist));
    addition -> name = name;
    addition -> data = make_new_graph();
    addition -> next = graphs;
    graphs = addition;
    return addition -> data;
}

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
    }
    i = 0;
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