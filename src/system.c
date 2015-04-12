/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <xcb/xcb.h>

#include "tint3.h"
#include "system.h"
#include "lwxt.h"
#include "lwbi.h"
#include "draw.h"
#include "format.h"

#define MAX_WINDOW_TITLE_LENGTH 256


#define BATTERY_FOREGROUND_HIGH "#0f0"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#f00"
#define BATTERY_BACKGROUND_LOW "#222222"


char * get_active_window_name(baritem * source) {
    if (!source) {
        return NULL;
    }
    char * window_title = malloc(MAX_WINDOW_TITLE_LENGTH);
    get_title(window_title, MAX_WINDOW_TITLE_LENGTH);
    return window_title;
}

char * get_time_format(baritem * item) {
    char exec[100] = {0};
    snprintf(exec, sizeof exec, "date +'%s'", item -> format);
    FILE * desc = popen(exec, "r");
    char * msg = calloc(0, 20);
    int msg_c = 0; char msg_s;
    if (desc) {
        while( (msg_s = fgetc(desc)) != '\n') {
            msg[msg_c++] = msg_s;
        }
        if (msg_c < 20) {
            msg[msg_c] = 0;
        }
        pclose(desc);
    }
    return msg;
}

char * get_battery(baritem * item) {
    char *batt = (item -> source)+8;
    char * msg = calloc(0,9);
    int battery_percent = get_battery_percent(batt);
    char * query = (item->format)[0] - '1' ? "╻:%i%%" : "╺:%i%%";
    snprintf(msg, 9, query, battery_percent);

    free(item -> color);

    if (battery_percent > 80) {
        item -> color = initcolor(dc, "#000000", BATTERY_FOREGROUND_HIGH);
    } else if (battery_percent > 20) {
        item -> color = initcolor(dc, "#000000", BATTERY_FOREGROUND_MED);
    } else {
        item -> color = initcolor(dc, "#000000", BATTERY_FOREGROUND_LOW);
    }

    return msg;
}

char * get_volume_level(baritem * item) {
    char * pipe_format = "amixer get -c %s Master | tail -n 1 | cut -d '[' -f 2,4";
    char pipe[54] = {0};
    char muted;
    snprintf(pipe, 55, pipe_format, (item -> source)+5);
    FILE * pf = popen(pipe, "r");
    int i = 0;
    if (pf) {
        fscanf(pf, "%i%%] [o%c", &i, &muted);
        item -> inverted = muted == 'f' ? 0 : 1;
        fclose(pf);
    }
    char * result = malloc(5);
    snprintf(result, 5, "%i%%", i);
    return result;
}

char * get_plain_text(baritem * item) {
    int len = strlen(item -> source);
    char * result = malloc(len+1);
    snprintf(result, len+1, item -> source);
    return result;
}


