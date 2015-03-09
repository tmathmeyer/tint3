/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _WEATHER_H_
#define _WEATHER_H_

char * get_weather_string(char * weather_format, char * weather_location);
int temperatureK(int place, char * string);
int weather_conditions(int place, char * string);

#endif

