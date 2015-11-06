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

#define BATTERY_FOREGROUND_HIGH "#000000"
#define BATTERY_FOREGROUND_MED "#000000"
#define BATTERY_FOREGROUND_LOW "#000000"

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

    element *e = calloc(sizeof(element), 1);
    e->text = elem;
    e->opt = 0;
    dlist_add(result, e);
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

    element *e = calloc(sizeof(element), 1);
    e->text = elem;
    e->opt = 0;

    dlist_add(result, e);
    return result;
}

dlist *shell_cmd(baritem *item) {
    dlist *result = dlist_new();

    FILE *pf = popen(item->source, "r");
    if (pf) {
        char line[50];
        fgets(line, 50, pf);
        line[48] = 0;

        text_element *elem = calloc(sizeof(text_element), 1);
        elem->text = strdup(line);
        elem->color = copy_color(item->default_colors);

        element *e = calloc(sizeof(element), 1);
        e->text = elem;
        e->opt = 0;

        dlist_add(result, e);
        fclose(pf);
    }

    return result;
}

dlist *get_battery(baritem *item) {
    char *batt = (item -> source)+8;
    char *msg = calloc(0,9);
    int battery_percent = get_battery_percent(batt);
    snprintf(msg, 9, "%i%%", battery_percent);
    ColorSet *colors;

    char *high_forground = get_baritem_option("high_font", item);
    char *high_background = get_baritem_option("high_color", item);
    char *med_forground = get_baritem_option("med_font", item);
    char *med_background = get_baritem_option("med_color", item);
    char *low_forground = get_baritem_option("low_font", item);
    char *low_background = get_baritem_option("low_color", item);

    if (!high_forground) {
        high_forground = BATTERY_FOREGROUND_HIGH;
    }
    if (!med_forground) {
        med_forground = BATTERY_FOREGROUND_MED;
    }
    if (!low_forground) {
        low_forground = BATTERY_FOREGROUND_LOW;
    }

    if (battery_percent > 80) {
        colors = make_baritem_colours(high_forground, high_background);
    } else if (battery_percent > 20) {
        colors = make_baritem_colours(med_forground, med_background);
    } else {
        colors = make_baritem_colours(low_forground, low_background);
    }

    text_element *elem = calloc(sizeof(text_element), 1);
    elem->text = msg;
    elem->color = colors;

    dlist *newlist = dlist_new();

    element *e = calloc(sizeof(element), 1);
    e->text = elem;
    e->opt = 0;

    dlist_add(newlist, e);
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

    text_element *elem = calloc(sizeof(text_element), 1);
    elem->text = calloc(sizeof(char), 5);
    snprintf(elem->text, 5, "%i%%", i);
    elem->color = calloc(sizeof(ColorSet), 1);
    elem->color->FG_xft = item->default_colors->FG_xft;
    if (muted == 'f') {
        elem->color->FG = item->default_colors->BG;
        elem->color->BG = item->default_colors->FG;
        elem->color->FG_xft = get_xft_color(dc, "#000");
    } else {
        elem->color->FG = item->default_colors->FG;
        elem->color->BG = item->default_colors->BG;
    }

    dlist *result = dlist_new();

    element *e = calloc(sizeof(element), 1);
    e->text = elem;
    e->opt = 0;

    dlist_add(result, e);
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

    element *e = calloc(sizeof(element), 1);
    e->text = elem;
    e->opt = 0;

    dlist_add(result, e);
    return result;
}
