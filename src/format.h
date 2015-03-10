/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _FORMAT_H_
#define _FORMAT_H_

#include <stdint.h>

typedef struct format_map {
    char formatID;
    int (*formatter)(int, char *);
    struct format_map * next;
} format_map;

typedef struct fmt_map {
	char *fmt_ids;
	int (**formatter)(int, char *);
	uint16_t rl_size;
	uint16_t max_size;
} fmt_map;



fmt_map *initmap(uint16_t size);
void delmap(fmt_map *map);
void fmt_map_put(fmt_map *map, char fmtid, int (*fmt_fn)(int, char *));





int format_string(char * dest, char * fmt, format_map * formatmap);

#endif