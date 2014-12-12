/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "format.h"
#include "json.h"
#include "network.h"

#define weather_parse_size 4096

static container * jsoncontext = 0;
static format_map * formatmap = 0;
static char * weather = 0;
static time_t lastime = 0;

int format(int place, char * string) {
    char * addition = "INterest";
    memcpy(string+place, addition, strlen(addition));
    return place + strlen(addition);
}

format_map * getformatmap() {
    if (formatmap) {
        return formatmap;
    }
    //FILL IN HERE
    formatmap = malloc(sizeof(format_map));
    formatmap -> next = 0;
    formatmap -> formatter = format;
    formatmap -> formatID = 'T';

    return formatmap;
}

// TODO make the location configurable
void update_json_context() {
    char * host = "api.openweathermap.org";
    char * url  = "/data/2.5/weather?q=Worcester,usa";
    char * weather_s = malloc(weather_parse_size);

    if (url_to_memory(weather_s, weather_parse_size, url, host, "162.243.44.32")) {
        if (strstr(weather_s, "HTTP/1.1 200")) {
            char * JSON = strstr(weather_s, "{");
            if (jsoncontext) {
                free_container(jsoncontext);
            }
            jsoncontext = from_string(&JSON);
        }
    }
    free(weather_s);
}

void update_weather_string() {
    if (!weather) {
        weather = malloc(128);
    }
    if (jsoncontext) {
        container * w_main = $(jsoncontext, "main");
        container * temperature = $(w_main, "temp");
        int temp = *(temperature -> number);

        container * w_weather = $(jsoncontext, "weather");
        container * first = _(w_weather, 0);
        container * sky = $(first, "main");
        char * sky_condition = sky -> string;

        
        memset(weather, 0, 128);
        snprintf(weather, 128, "%s, %i", sky_condition, temp);
    }
}

void attempt_update_weather() {
    if (lastime == 0) {
        lastime = 1;
    } else if (time(NULL)-lastime > 1800) {
        time(&lastime);
        update_json_context();
        update_weather_string();
    }
}




char * get_weather_string() {
    attempt_update_weather();
    char * result;

    if (weather) {
        result = strdup(weather);
    } else {
        result = strndup("<<err>>", 8);
    }

    return result;
}