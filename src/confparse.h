/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#include <stdlib.h>

#ifndef _CONFPARSER_H_
#define _CONFPARSER_H_

typedef struct {
    char *id;
    char *name;
    char *type;
    char *format;
    char *source;
    char forground [8];
    char background [8];
} block;

typedef struct l {
    struct l *next;
    block *data;
} block_list;

typedef struct {
    int border_size;
    int margin_size;
    int padding_size;
    char *border_color;
    char *background_color;
    char *location;
    block_list *left;
    block_list *center;
    block_list *right;
    char **options;
} bar_config;

bar_config * readblock(FILE * fp);
int starts_with(char * source, char * check);
int has_options(char *opt, bar_config *conf);

#endif
