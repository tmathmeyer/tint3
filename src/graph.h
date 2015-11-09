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

typedef struct graph_chain {
    struct graph_chain *next;
    float value;
} graph_chain;

typedef struct graph_ir {
    unsigned int occupied_size;
    unsigned int max_size;
    graph_chain *head;
    graph_chain *tail;
} graph_ir;

/**
 * returns a graph element WITH NO COLOR
 * */
graph_element *write_graph_value(char *name, float value);

graph_chain *make_loop(int size);

graph_ir *init_ir(int size);

graph_element *from_ir(graph_ir *ir, int upperbound, int lowerbound);

graph_element *get_named(char *name);

#endif

