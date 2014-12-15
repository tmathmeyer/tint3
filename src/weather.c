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

struct comb {
    char formatID;
    int (*formatter)(int, char *);
};

int _temperature() {
    container * w_main = $(jsoncontext, "main");
    container * temperature = $(w_main, "temp");
    return *(temperature -> number);
}

int _pressure() {
    container * w_main = $(jsoncontext, "main");
    container * pressure = $(w_main, "pressure");
    return *(pressure -> number);
}

int _humidity() {
    container * w_main = $(jsoncontext, "main");
    container * humidity = $(w_main, "humidity");
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
    container * w_weather = $(jsoncontext, "weather");
    container * first = _(w_weather, 0);
    container * sky = $(first, "main");
    char * sky_condition = sky -> string;
    return place + sprintf(string+place, "%s", sky_condition);
}

void add_to_format(struct comb comb) {
    format_map * next = malloc(sizeof(format_map));
    next -> next = formatmap;
    next -> formatID = comb.formatID;
    next -> formatter = comb.formatter;
    formatmap = next;
}

format_map * getformatmap() {
    if (formatmap) {
        return formatmap;
    }

    struct comb kelvin = {'K', &temperatureK};
    struct comb farenheight = {'F', &temperatureF};
    struct comb celsius = {'C', &temperatureC};
    struct comb weather = {'W', &weather_conditions};
    struct comb atmospheres = {'A', &pressureATM};
    struct comb mmhg = {'P', &pressureHg};
    struct comb humidity = {'H', &humidityPC};
    struct comb dewpoint = {'D', &dew_point};

    add_to_format(kelvin);
    add_to_format(weather);
    add_to_format(farenheight);
    add_to_format(celsius);
    add_to_format(atmospheres);
    add_to_format(mmhg);
    add_to_format(humidity);
    add_to_format(dewpoint);

    return formatmap;
}

// TODO make the location configurable
void update_json_context(char * location) {
    char url[100] = {0};
    snprintf(url, 100, "/data/2.5/weather?q=%s", location);

    char * host = "api.openweathermap.org";
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

void update_weather_string(char * weather_format) {
    if (!weather) {
        weather = malloc(128);
    }
    if (jsoncontext) {
        memset(weather, 0, 128);
        format_string(weather, weather_format, getformatmap());
    }
}

void attempt_update_weather(char * weather_format, char * weather_location) {
    if (lastime == 0) {
        lastime = 1;
    } else if (time(NULL)-lastime > 1800) {
        time(&lastime);
        update_json_context(weather_location);
        update_weather_string(weather_format);
    }
}

char * get_weather_string(char * weather_format, char * weather_location) {
    attempt_update_weather(weather_format, weather_location);
    char * result;

    if (weather) {
        result = strdup(weather);
    } else {
        result = strndup("<<err>>", 8);
    }

    return result;
}
