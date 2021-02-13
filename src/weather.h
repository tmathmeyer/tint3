/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _WEATHER_H_
#define _WEATHER_H_

#include "tint3.h"

#define API_KEY "4fdd945fddcc1a090934dacc3e38b8d7"

dlist *get_weather(baritem *item);

char *get_weather_string(char *weather_format, char *weather_location);
int temperatureK(int place, char *string);
int weather_conditions(int place, char *string);
void spawn_weather_thread(baritem *ipl);
void show_details(baritem *item, int xpos);
#endif

