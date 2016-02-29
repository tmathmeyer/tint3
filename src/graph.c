/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */
#define _DEFAULT_SOURCE

#include "graph.h"
#include "dmap.h"

static dmap *map = NULL;

graph *initialize_graph(size_t size, size_t floating) {
    graph *result = calloc(1, sizeof(graph));
    result->occupancy = size;
    result->data = calloc(size, sizeof(double));
    result->floating = floating;
    return result;
}

graph *insert_value(graph *graph, graph_t value) {
    memcpy(graph->data + graph->headptr, &value, sizeof(graph_t));
    graph->headptr++;
    graph->headptr %= graph->occupancy;
    return graph;
}

graph_element *from_graph(graph *graph) {
    graph_t min, max;
    min._i = LLONG_MAX;
    max._i = LLONG_MIN;
    graph_element *result = calloc(sizeof(graph_element), 1);
    result->cols = graph->occupancy;
    result->rows = 1;
    result->data = calloc(sizeof(int), graph->occupancy * 2);

    for(size_t i=0;i<graph->occupancy; i++) {
        long long test = graph->data[i]._i;
        if (test < min._i) {
            min._i = test;
        }
        if (test > max._i) {
            max._i = test;
        }
    }
    if (graph->prevmax._i > max._i) {
        graph_t tmp = max;
        tmp._i += graph->prevmax._i;
        tmp._i /= 2;
        max = graph->prevmax;
        graph->prevmax = tmp;
    } else {
        graph->prevmax = max;
    }
    double dd = max._f - min._f;
    long long id = max._i - min._i;
    if (dd==0) {
        dd++;
        id++;
    }

    int r = graph->headptr;
    if (r < 0) {
        r = graph->occupancy - 1;
    }
    for(size_t i=0;i<graph->occupancy;i++) {
        int y = 0;
        r--;
        if (r < 0) {
            r = graph->occupancy - 1;
        }
        if (graph->floating) {
            y = (int)(((graph->data)[r]._f - min._f) * 26.0 / dd) + 2;
        } else {
            y = (int)(((graph->data)[r]._i - min._i) * 26 / id) + 2;
        }
        (result->data)[2*i] = i*4;
        (result->data)[2*i+1] = y;
    }
    return result;
}

graph_element *get_element_by_name(char *name) {
    if (!map) {
        map = map_new();
        put(map, name, initialize_graph(20, 0));
        return NULL;
    }
    graph *graph = map_get(map, name);
    if (graph) {
        return from_graph(graph);
    }
    return NULL;
}

graph_element *write_graph_value(char *name, graph_t value) {
    if (!map) {
        map = map_new();
    }
    graph *gr = (graph *)map_get(map, name);
    if (!gr) {
        gr = initialize_graph(20, 0);
        put(map, name, gr);
    }
    insert_value(gr, value);
    return NULL;
}

