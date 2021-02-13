/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef __UTILS__
#define __UTILS__

#include <X11/Xft/Xft.h>
#include "tint3.h"
#include "dlist/dlist.h"


dlist *config_to_list (char *list);
baritem *char_to_item(char c);
baritem *weather_s();
void free_list(dlist *list);
void free_baritem(baritem *item);
void draw_list(dlist *list);
unsigned int total_list_length(dlist *list);
int get_mem_percent();


dlist *get_battery(baritem *item);
dlist *get_volume_level(baritem *item);
dlist *get_active_window_name(baritem *item);
dlist *get_plain_text(baritem *item);
dlist *get_time_format(baritem *item);
dlist *shell_cmd(baritem *item);
dlist *get_mem_graph(baritem *item);
dlist *get_mem_text(baritem *item);

#endif
