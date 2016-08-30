/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */


#ifndef _CONFPARSER_H_
#define _CONFPARSER_H_

#include <stdlib.h>
#include <stdio.h>
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
    char *shell_click;
    dlist *map;
} block;

typedef struct map_entry {
    char *key;
    void *value;
} entry;

typedef struct {
    int border_size;
    int margin_right;
    int margin_left;
    int margin_top;
    int margin_bottom;
    int padding_size;
    int width;
    char *font_color;
    char *border_color;
    char *background_color;
    char *font_name;
    location location;
    dlist *left;
    dlist *center;
    dlist *right;
    dlist *options;
} bar_config;


bar_config *build_bar_config(FILE *rc);
int has_options(char *opt, bar_config *conf);
void *getconfopt(block *forblock, char *element);

#endif
