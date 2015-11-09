/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */
#define _DEFAULT_SOURCE

#include "graph.h"
#include "dmap.h"
#include "tint3.h"

static dmap *map = NULL;

graph_element *get_named(char *name) {
    if (!map) {
        map = map_new();
        put(map, name, init_ir(50)); // TODO MUST FIX
    }
    graph_ir *ir = map_get(map, name);
    if (ir) {
        return from_ir(ir, 30, 0); // TODO FIX THIS VERY IMPORTANT
    }
    return NULL;
}

void ir_put_next(graph_ir *ir, float value) {   
    ir->tail->value = value;
    ir->tail = ir->tail->next;
    ir->head = ir->head->next;
    if (ir->occupied_size < ir->max_size) {
        ir->occupied_size++;
    }
}

graph_element *write_graph_value(char *name, float value) {
    if (!map) {
        map = map_new();
    }
    graph_ir *graph = (graph_ir *)map_get(map, name);
    if (!graph) {
        graph = init_ir(50); //TODO FIX THIS VERY IMPORTANT
        put(map, name, graph);
    }
    ir_put_next(graph, value);
    return NULL; //TODO eventually return from_ir on graph, for re-arch
}

graph_chain *make_loop(int size) {
    graph_chain *head = calloc(sizeof(graph_chain), 1);
    graph_chain *tail = head;

    while(size --> 1) {
        tail->next = calloc(sizeof(graph_chain), 1);
        tail = tail->next;
    }
    tail->next = head;
    return head;
}

graph_ir *init_ir(int size) {
    graph_ir *res = calloc(sizeof(graph_ir), 1);
    res->occupied_size = 0; //redundant, but whatever
    res->max_size = size;
    res->tail = 
    res->head = make_loop(size);
    return res;
}

graph_element *from_ir(graph_ir *ir, int upperbound, int lowerbound) {
    float float_max = -3.4028e+38;
    float float_min = 3.4028e+38;
    graph_element *result = calloc(sizeof(graph_element), 1);
    int count = 0;
    int x = ir->max_size;
    graph_chain *c = ir->head;
    while(x --> 0) {
        if (c->value > float_max) {
            float_max = c->value;
        }
        if (c->value < float_min) {
            float_min = c->value;
        }
        c = c->next;
    }

    x = ir->max_size;
    c = ir->head;
    result->xy_count = x;
    result->xys = calloc(sizeof(int), x * 2);

    while(x --> 0) {
        float ratio = (c->value - float_min) / (float_max - float_min + 1);
        float gp = ((upperbound - lowerbound) * ratio) + lowerbound;
        (result->xys)[count] = count*1;
        (result->xys)[count+1] = (int)gp;
        count+=2;
        c = c->next;
    }

    return result;
}
