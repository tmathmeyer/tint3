/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE
#define DEBUG_MODE 0
#ifdef DEBUG_MODE
#include <stdio.h>
#endif
#include <stdlib.h>
#include "format.h"

int BAD(int start, char * dest) {
    if (DEBUG_MODE) {
        puts(dest);
    }
    return start;
}

typedef int (*textformatter)(int, char *);
textformatter lookup(format_map * fmt, char id) {
    if (!fmt) {
        return BAD;
    } else if (id == fmt -> formatID) {
        return fmt -> formatter;
    }
    return lookup(fmt -> next, id);
}

int format_string(char * dest, char * fmt, format_map * formatmap) {
    int e = 0;
    int i = 0;
    int d = 0;
    for(;fmt[i];i++) {
        if (fmt[i] == '%') {
            d = !d;
        } else if (d) {
            d = !d;
            e = lookup(formatmap, fmt[i])(e, dest);
        } else {
            dest[e++] = fmt[i];
        }
    }
    return e;
}


fmt_map *initmap(uint16_t size) {
    fmt_map *result = malloc(sizeof(fmt_map));
    if (!result) {
        return NULL;
    }
    result -> rl_size = 0;
    result -> max_size = size;
    result -> fmt_ids = malloc(sizeof(char) * size);
    result -> formatter = malloc(sizeof(void *) * size);
    return result;
}


void delmap(fmt_map *map) {
    if (map) {
        free(map -> fmt_ids);
        free(map -> formatter);
        free(map);
    }
}

void copy_map(fmt_map *map, fmt_map *copy) {
    uint ctr = 0;
    for(; ctr < map->rl_size; ctr++) {
        (map->fmt_ids)[ctr] = (copy->fmt_ids)[ctr];
        (map->formatter)[ctr] = (copy->formatter)[ctr];
    }
    copy->rl_size = map->rl_size;
}


void fmt_map_put(fmt_map *map, char fmtid, int (*fmt_fn)(int, char *)) {
    if (map -> rl_size + 1 == map -> max_size) {
        fmt_map *copy = initmap(map->max_size * 2);
        copy_map(map, copy);
        delmap(map);
        map = copy;    
    }
    (map->fmt_ids)[map->rl_size] = fmtid;
    (map->formatter)[map->rl_size] = fmt_fn;
    map->rl_size ++;
}
