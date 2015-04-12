/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */


#ifndef _CONFPARSER_H_
#define _CONFPARSER_H_

#include <stdlib.h>
#include "dlist.h"

typedef enum loc {
    TOP, BOTTOM
} location;

typedef struct {
    char *id;
    char *name;
    char *type;
    char *format;
    char *source;
    char *forground;
    char *background;
    dlist *map;
} block;

typedef struct map_entry {
    char *key;
    void *value;
} entry;

typedef struct {
    int border_size;
    int margin_size;
    int padding_size;
    char *border_color;
    char *background_color;
    location location;
    dlist *left;
    dlist *center;
    dlist *right;
    dlist *options;
} bar_config;


bar_config *build_bar_config(FILE *rc);
int has_options(char *opt, bar_config *conf);

#endif
