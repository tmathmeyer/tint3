/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */
#define _DEFAULT_SOURCE

#include "graph.h"
#include "dmap.h"
#include <stdarg.h>

static dmap *map = NULL;

graph *get_graph_by_name(char *name) {
    if (!map) {
        return NULL;
    }
    return map_get(map, name);
}

void insert_graph_by_name(char *name, graph_t value) {
    if (!map) {
        map = map_new();
    }
    graph *gr = (graph *)map_get(map, name);
    if (!gr) {
        gr = initialize_graph(20, 0);
        put(map, name, gr);
    }
    insert_value(gr, value);
}

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

graph_element *percentage_graph(graph *graph) {
    graph_element *result = calloc(sizeof(graph_element), 1);
    result->cols = graph->occupancy;
    result->rows = 1;
    result->data = calloc(sizeof(int), graph->occupancy * 2);

    int r = graph->headptr;
    if (r < 0) {
        r = graph->occupancy - 1;
    }
    for(size_t i=0;i<graph->occupancy;i++) {
        int y = 0;
        r--;
        if (r < 0) {
            r = graph->occupancy-1;
        }
        if (graph->floating) {
            y = 2 + (graph->data)[r]._f*26.0 / 100.0;
        } else {
            y = 2 + (graph->data)[r]._i*26 / 100;
        }
        (result->data)[2*i] = i*4;
        (result->data)[2*i+1] = y;
    }
    return result;
}

graph_element *scaled_graph(graph *graph) {
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

graph_element *get_percentage_graph_element_by_name(char *name) {
    if (!map) {
        map = map_new();
        put(map, name, initialize_graph(20, 0));
        return NULL;
    }
    graph *graph = map_get(map, name);
    if (graph) {
        return percentage_graph(graph);
    }
    return NULL;
}

graph_element *get_scaled_graph_element_by_name(char *name) {
    if (!map) {
        map = map_new();
        put(map, name, initialize_graph(20, 0));
        return NULL;
    }
    graph *graph = map_get(map, name);
    if (graph) {
        return scaled_graph(graph);
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

graph_element *multigraph_create_scaled(combined_graph *cg) {
    graph_element *result = calloc(sizeof(graph_element), 1);
    result->cols = cg->cols;
    result->rows = cg->rows;
    result->data = calloc(sizeof(int), cg->rows * cg->cols * 2);

    graph_t max, min, D, test;
    min._i = LLONG_MAX;
    max._i = LLONG_MIN;
    for(size_t i=0;i<cg->rows;i++) {
        for(size_t j=0;j<cg->cols;j++) {
            D = (cg->data)[i][j];
            if (D._i > max._i) {
                max._i = D._i;
            }
            if (D._i < min._i) {
                min._i = D._i;
            }
        }
    }
    D._i = max._i - min._i;
    if (D._i == 0) {
        D._i = 1;
    }
    size_t W = 0;
    size_t X = 0;
    for(size_t i=0;i<cg->rows;i++) {
        for(size_t j=0;j<cg->cols;j++) {
            test = (cg->data)[i][j];
            long long y = test._i - min._i;
            y = (y * 26) / D._i + 2;
            (result->data)[W++] = (X+=4);
            (result->data)[W++] = (int)y;
        }
        X = 0;
    }
    return result;
}

void combine_graph_scaled(combined_graph *cg, graph *g, int scale) {
    if (!cg || !g) {
        return;
    }
    if (g->occupancy > cg->cols) {
        graph_t ZERO;
        ZERO._i = 0;
        int old = cg->cols;
        cg->cols = g->occupancy;
        for(size_t i=0;i<cg->rows;i++) {
            (cg->data)[i] = realloc((cg->data)[i], cg->cols*sizeof(graph_t));
            for(size_t j=old;j<cg->cols;j++) {
                (cg->data)[i][j] = ZERO;
            }
        }
    }

    cg->rows++;
    cg->data = realloc(cg->data, cg->rows*sizeof(graph_t *));

    graph_t *W = (cg->data)[cg->rows-1] = calloc(sizeof(graph_t), cg->cols);
    for(size_t i=0;i<cg->cols;i++) {
        W[i] = (g->data)[(g->headptr+i)%cg->cols];
        if (g->floating) {
            W[i]._f *= scale;
        } else {
            W[i]._i *= scale;
        }
    }
}

void combine_graph(combined_graph *cg, graph *g) {
    combine_graph_scaled(cg, g, 1);
}

combined_graph *combine_named_graph(int names, ...) {
    if (!map) {
        return NULL;
    }
    va_list argp;
    char *name;
    va_start(argp, names);
    combined_graph *combo = calloc(sizeof(combined_graph), 1);
    combo->rows = 0;
    combo->cols = 0;
    while(names --> 0) {
        name = va_arg(argp, char *);
        graph *graph = map_get(map, name);
        if (graph != NULL) {
            combine_graph(combo, graph);
        }
    }

    return combo;
}

