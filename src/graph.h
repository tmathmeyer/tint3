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
    graph_t *data;
    size_t floating : 1;
    graph_t prevmax;
} graph;

typedef struct {
    size_t rows, cols;
    graph_t **data;
} combined_graph;

graph *initialize_graph(size_t size, size_t floating);
graph *insert_value(graph *, graph_t);
graph * get_graph_by_name(char *name);
combined_graph *combine_named_graph(int, ...);
void combine_graph(combined_graph *cg, graph *g);
void combine_graph_scaled(combined_graph *cg, graph *g, int);

graph_element *multigraph_create_scaled(combined_graph *cg);
graph_element *write_graph_value(char *name, graph_t value);
graph_element *get_percentage_graph_element_by_name(char *name);
graph_element *get_scaled_graph_element_by_name(char *name);

#endif

