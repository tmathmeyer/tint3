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
#include "dlist.h"


dlist *config_to_list (char *list);
baritem *char_to_item(char c);
baritem *weather_s();
void free_list(dlist *list);
void free_baritem(baritem *item);
void draw_list(dlist *list);
unsigned int total_list_length(dlist *list);



char *get_time_format(baritem *item);
char *get_battery(baritem *item);
char *get_volume_level(baritem *item);
char *get_active_window_name(baritem *item);
char *get_plain_text(baritem *item);
void  toggle_mute(baritem *item, int xpos);
void  expand_volume(baritem *item, int xpos);
void  leave_volume(baritem *item);

#endif
