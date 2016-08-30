/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE
#include <stdlib.h>
#include "format.h"


#include <stdio.h>

int BAD(int start, char *dest) {
    (void) dest;
    return start;
}

typedef int (*textformatter)(int, char *);
textformatter lookup(fmt_map *fmt, char id) {
    if (!fmt) {
        return BAD;
    } else {
        uint16_t ctr = fmt->rl_size;
        while(ctr --> 0) {
            if ((fmt->fmt_ids)[ctr] == id) {
                return (fmt->formatter)[ctr];
            }
        }
    }
    return BAD;
}

int format_string(char *dest, char *fmt, fmt_map *formatmap) {
    int e = 0;
    int i = 0;
    int d = 0;
    for(;fmt[i];i++) {
        if (fmt[i] == '%') {
            if (d) {
                dest[e++] = '%';
            }
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
        (copy->fmt_ids)[ctr] = (map->fmt_ids)[ctr];
        (copy->formatter)[ctr] = (map->formatter)[ctr];
    }
    copy->rl_size = map->rl_size;
}


void fmt_map_put(fmt_map *map, char fmtid, int (*fmt_fn)(int, char *)) {
    if (map -> rl_size == map -> max_size) {
        fmt_map *copy = initmap(map->max_size * 2);
        copy_map(map, copy);
        delmap(map);
        map = copy;
    }
    (map->fmt_ids)[map->rl_size] = fmtid;
    (map->formatter)[map->rl_size] = fmt_fn;
    map->rl_size ++;
}
