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



itemlist * config_to_list (char * list);
baritem * char_to_item(char c);
baritem * weather_s();
void free_list(itemlist * list);
void free_baritem(baritem * item);
void draw_list(itemlist * list);
unsigned int total_list_length(itemlist * list);



char * get_time_format(baritem * item);
char * get_battery(baritem * item);
char * get_volume_level(baritem * item);
void   toggle_mute(baritem * item);
char * get_active_window_name(baritem * item);
char * get_plain_text(baritem * item);


#endif
