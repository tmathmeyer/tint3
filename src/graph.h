/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _T3_GRAPH_H_
#define _T3_GRAPH_H_

#include "tint3.h"

#define GRAPHLENGTH 15

typedef struct graph{
    int graph[GRAPHLENGTH*3+1];
    int start;
    int max;
} graph;

typedef struct glist{
    struct glist * next;
    char * name;
    graph * data;
} glist;


graph *make_new_graph();
graph *get_named_graph(char * name);
char *graph_to_string(graph * gr);
void recalc_max(graph * gr);
void add_to_graph(int i, graph * gr);

#endif

