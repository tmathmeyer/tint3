/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "weather.h"
#include "format.h"
#include "json.h"
#include "http.h"
#include "draw.h"
#include "tint3.h"
#include "popup.h"

#define weather_parse_size 4096

static container * jsoncontext = 0;
static fmt_map *formatmap = 0;
static char * weather = 0;
static pthread_t weather_ltnr;
static int _INIT_ = 1;
static popup_window *popup = NULL;

void unset_popup(void) {
    if (popup != NULL) {
        free_window(popup);
        popup = NULL;
    }
}

void show_details(baritem *item, int xpos) {
    popup = spawn_popup(item, AT_MOUSE, xpos, &unset_popup);
    drawmenu();
}

void *weather_listen(void *DATA) {
    baritem * ipl = DATA;
    sleep(1);
    while(1) {
        ipl -> string = get_weather(ipl);
        drawmenu();
        sleep(30*60);
    }
    return NULL;
}

void spawn_weather_thread(baritem *ipl) {
    pthread_create(&weather_ltnr, NULL, weather_listen, ipl);
}

char *get_weather(baritem* item) {
    if (_INIT_) {
        _INIT_ = 0;
        return strdup("~connecting~");
    }
    return get_weather_string(item -> format, item -> source);
}

struct comb {
    char formatID;
    int (*formatter)(int, char *);
};

int _temperature() {
    container * w_main = A(jsoncontext, "main");
    container * temperature = A(w_main, "temp");
    return *(temperature -> number);
}

int _pressure() {
    container * w_main = A(jsoncontext, "main");
    container * pressure = A(w_main, "pressure");
    return *(pressure -> number);
}

int _humidity() {
    container * w_main = A(jsoncontext, "main");
    container * humidity = A(w_main, "humidity");
    return *(humidity -> number);
}

int temperatureK(int place, char * string) {
    return place + sprintf(string+place, "%i", _temperature());
}

int temperatureC(int place, char * string) {
    int tempC = _temperature()-273;
    return place + sprintf(string+place, "%i", tempC);
}

int temperatureF(int place, char * string) {
    int tempC = _temperature()-273;
    int tempF = ((tempC * 9) / 5) + 32;
    return place + sprintf(string+place, "%i", tempF);
}

int pressureHg(int place, char * string) {
    int pressureHg = _pressure();
    return place + sprintf(string+place, "%i", pressureHg);
}

int pressureATM(int place, char * string) {
    int pressureATM = _pressure() / 760;
    return place + sprintf(string+place, "%i", pressureATM);
}

int humidityPC(int place, char * string) {
    int humidity = _humidity();
    return place + sprintf(string+place, "%i", humidity);
}

int dew_point(int place, char * string) {
    int dewpoint = (_humidity() * _temperature()) / 100;
    return place + sprintf(string+place, "%i", dewpoint);
}

int weather_conditions(int place, char * string) {
    container * w_weather = A(jsoncontext, "weather");
    container * first = _(w_weather, 0);
    container * sky = A(first, "main");
    char * sky_condition = sky -> string;
    return place + sprintf(string+place, "%s", sky_condition);
}

fmt_map * getformatmap() {
    if (!formatmap) {
        formatmap = initmap(10);
        fmt_map_put(formatmap, 'K', &temperatureK);
        fmt_map_put(formatmap, 'F', &temperatureF);
        fmt_map_put(formatmap, 'C', &temperatureC);
        fmt_map_put(formatmap, 'W', &weather_conditions);
        fmt_map_put(formatmap, 'A', &pressureATM);
        fmt_map_put(formatmap, 'P', &pressureHg);
        fmt_map_put(formatmap, 'H', &humidityPC);
        fmt_map_put(formatmap, 'D', &dew_point);
    }
    
    return formatmap;
}


void update_json_context(char * location) {
    char url[100] = {0};
    snprintf(url, 100, "/data/2.5/weather?q=%s", location);

    char *host = "api.openweathermap.org";
    char *weather_s = malloc(weather_parse_size);
    char ip[16] = {0};
    if (hostname_to_ip(host, ip)) {
        if (url_to_memory(weather_s, weather_parse_size, url, host, ip)) {
            if (strstr(weather_s, "HTTP/1.1 200")) {
                char *JSON = strstr(weather_s, "{");
                if (jsoncontext) {
                    free_container(jsoncontext);
                }
                jsoncontext = from_string(&JSON);
                if (!jsoncontext) {
                    puts(JSON);
                }

            }
        }
    } else {
        perror("can't get ip of api.openweathermap.org");
    }
    free(weather_s);
}

int update_weather_string(char *weather_format) {
    if (!weather) {
        weather = malloc(128);
    }

    if (jsoncontext) {
        memset(weather, 0, 128);
        format_string(weather, weather_format, getformatmap());
        return 1;
    }
    return 0;
}

char * get_weather_string(char *weather_format, char *weather_location) {
    update_json_context(weather_location);
    char *result;

    if (update_weather_string(weather_format)) {
        result = strdup(weather);
    } else {
        result = strndup("<<err>>", 8);
    }

    return result;
}

