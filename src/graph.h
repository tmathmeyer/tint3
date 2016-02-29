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

typedef union {
    double _f;
    long long _i;
} graph_t;

typedef struct {
    size_t occupancy;
    size_t headptr;
    graph_t * data;
    size_t floating : 1;
    graph_t prevmax;
} graph;

graph *initialize_graph(size_t size, size_t floating);
graph *insert_value(graph *, graph_t);
graph_element *write_graph_value(char *name, graph_t value);
graph_element *get_element_by_name(char *name);

#endif

