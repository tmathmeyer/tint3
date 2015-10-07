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


#define BATTERY_FOREGROUND_HIGH "#00ff00"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#ff0000"
#define BATTERY_BACKGROUND_LOW "#222222"


dlist *get_active_window_name(baritem *source) {
    if (!source) {
        return NULL;
    }
    char *window_title = malloc(MAX_WINDOW_TITLE_LENGTH);
    get_title(window_title, MAX_WINDOW_TITLE_LENGTH);

    dlist *result = dlist_new();
    text_element *elem = calloc(sizeof(text_element), 1);
    elem->text = window_title;
    elem->color = copy_color(source->default_colors);

    dlist_add(result, elem);
    return result;
}

dlist *get_time_format(baritem *item) {
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

    dlist *result = dlist_new();
    text_element *elem = calloc(sizeof(text_element), 1);
    elem->text = msg;
    elem->color = copy_color(item->default_colors);

    dlist_add(result, elem);
    return result;
}

dlist *get_battery(baritem *item) {
    char *batt = (item -> source)+8;
    char *msg = calloc(0,9);
    int battery_percent = get_battery_percent(batt);
    snprintf(msg, 9, "%i%%", battery_percent);
    ColorSet *colors;
    if (battery_percent > 80) {
        colors = initcolor(dc, "#000000", BATTERY_FOREGROUND_HIGH);
    } else if (battery_percent > 20) {
        colors = initcolor(dc, "#000000", BATTERY_FOREGROUND_MED);
    } else {
        colors = initcolor(dc, "#000000", BATTERY_FOREGROUND_LOW);
    }

    text_element *element = calloc(sizeof(text_element), 1);
    element->text = msg;
    element->color = colors;

    dlist *newlist = dlist_new();
    dlist_add(newlist, element);
    return newlist;
}

dlist *get_volume_level(baritem *item) {
    char * pipe_format = "amixer get -c %s | tail -n 1 | cut -d '[' -f 2,4";
    int size = 44 + strlen(item -> source);
    char *pipe = calloc(sizeof(char), size);
    char muted;
    snprintf(pipe, size, pipe_format, (item -> source) + 5);
    FILE * pf = popen(pipe, "r");
    int i = 0;
    if (pf) {
        fscanf(pf, "%i%%] [o%c", &i, &muted);
        fclose(pf);
    }
    free(pipe);

    text_element *element = calloc(sizeof(text_element), 1);
    element->text = calloc(sizeof(char), 5);
    snprintf(element->text, 5, "%i%%", i);
    element->color = calloc(sizeof(ColorSet), 1);
    element->color->FG_xft = item->default_colors->FG_xft;
    if (muted == 'f') {
        element->color->FG = item->default_colors->BG;
        element->color->BG = item->default_colors->FG;
        element->color->FG_xft = get_xft_color(dc, "#000");
    } else {
        element->color->FG = item->default_colors->FG;
        element->color->BG = item->default_colors->BG;
    }
    
    dlist *result = dlist_new();
    dlist_add(result, element);
    return result;
}

dlist *get_plain_text(baritem *item) {
    int len = strlen(item -> source);
    char *text = malloc(len+1);
    snprintf(text, len+1, item -> source);
    
    
    dlist *result = dlist_new();
    text_element *elem = calloc(sizeof(text_element), 1);
    elem->text = text;
    elem->color = copy_color(item->default_colors);

    dlist_add(result, elem);
    return result;
}
