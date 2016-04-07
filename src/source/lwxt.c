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
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <xcb/xcb.h>
#include <xcb/xcb_event.h>
#include <xcb/xcb_icccm.h>
#include <xcb/xcb_ewmh.h>

#include "lwxt.h"

xcb_ewmh_connection_t *ewmh;
xcb_connection_t *dpy;
int default_screen;
xcb_window_t root;

int _setup = 1;

void get_title(char * buffer, size_t buf_len) {
    if (_setup) {
        if (lwxt_setup()) {
            return;
        }
    }
    xcb_window_t win = XCB_NONE;
    if (get_active_window(&win)) {
        get_window_title(win, buffer, buf_len);
    }
}

int lwxt_setup(void) {
    _setup = 0;
    dpy = xcb_connect(NULL, &default_screen);
    if (xcb_connection_has_error(dpy)) {
        return 1;    
    }
    xcb_screen_t *screen = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
    if (screen == NULL) {
        return 1;    
    }
    root = screen->root;
    ewmh = malloc(sizeof(xcb_ewmh_connection_t));
    if (xcb_ewmh_init_atoms_replies(ewmh, xcb_ewmh_init_atoms(dpy, ewmh), NULL) == 0) {
        return 1;
    }
    return 0;
}

char *expand_escapes(const char *src) {
    char *dest = (char *)malloc(2 * strlen(src) + 1);
    char *start = dest;
    char c;
    while ((c = *(src++))) {
        if (c == '\'' || c == '\"' || c == '\\')
            *(dest++) = '\\';
        *(dest++) = c;
    }
    *dest = '\0';
    return start;
}

int get_active_window(xcb_window_t *win) {
    return (xcb_ewmh_get_active_window_reply(ewmh, xcb_ewmh_get_active_window(ewmh, default_screen), win, NULL) == 1);
}


void get_window_title(xcb_window_t win, char *title, size_t len) {
    xcb_ewmh_get_utf8_strings_reply_t ewmh_txt_prop;
    xcb_icccm_get_text_property_reply_t icccm_txt_prop;
    ewmh_txt_prop.strings = icccm_txt_prop.name = NULL;
    title[0] = '\0';
    if (win != XCB_NONE
            && (xcb_ewmh_get_wm_name_reply(ewmh, xcb_ewmh_get_wm_name(ewmh, win), &ewmh_txt_prop, NULL) == 1
                || xcb_icccm_get_wm_name_reply(dpy, xcb_icccm_get_wm_name(dpy, win), &icccm_txt_prop, NULL) == 1)) {
        char *src = NULL;
        size_t title_len = 0;
        if (ewmh_txt_prop.strings != NULL) {
            src = ewmh_txt_prop.strings;
            title_len = MIN(len, ewmh_txt_prop.strings_len);
        } else if (icccm_txt_prop.name != NULL) {
            src = icccm_txt_prop.name;
            title_len = MIN(len, icccm_txt_prop.name_len);
        }
        if (src != NULL) {
            strncpy(title, src, title_len);
            title[title_len] = '\0';
        }
    }
    if (ewmh_txt_prop.strings != NULL) {
        xcb_ewmh_get_utf8_strings_reply_wipe(&ewmh_txt_prop);
    }
    if (icccm_txt_prop.name != NULL) {
        xcb_icccm_get_text_property_reply_wipe(&icccm_txt_prop);
    }
}

