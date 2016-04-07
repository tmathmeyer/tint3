/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>
#include "tint3.h"
#include "draw.h"
#include "popup.h"

void draw_popup(popup_window *w) {
    XDrawArc(w->dsp, w->canvas, w->ctx, 60, 40, 40, 40, 45, 135);

    XClearArea(w->dsp, w->win, 0, 0, w->w, w->h, 0);
    XCopyArea(dc->dpy, dc->canvas, w->win, w->ctx, 0, 0, w->w, w->h, 0, 0);
    XFlush(dc->dpy);
}

void *window_action_listener(void *ctx) {
    popup_window *win = (popup_window *)ctx;
    XEvent xe;
    XSelectInput(win->dsp, win->win, LeaveWindowMask);
event:
    draw_popup(win);
    XNextEvent(win->dsp, &xe);
    if (xe.type != LeaveNotify) {
        goto event;
    }

    (win->free_reference)();
    return NULL;
}

popup_window *spawn_popup(
        baritem *item,
        window_position fallback,
        unsigned int click_xpos,
        void (* free_reference)(void)) {

    char *loc = get_baritem_option("details-location", item);
    if (loc != NULL) {
        if (!strcmp(loc, "mouse")) {
            fallback = AT_MOUSE;
        } else if (!strcmp(loc, "centre")) {
            fallback = CENTERED;
        } else if (!strcmp(loc, "aligned")) {
            fallback = ALIGNED;
        }
    }

    unsigned int X = 0;
    unsigned int Y = 0;
    unsigned int W = 120;
    unsigned int H = 80;

    switch(fallback) {
        case CENTERED:
            Y = 500; // make this generic
            X = 900; // not just for 1080p
            break;
        case AT_MOUSE:
            Y = dc->h;
            X = click_xpos;
            break;
        case ALIGNED:
            Y = dc->h;
            X = item->xstart + (item->length)/2 - (W / 2);
            break;
    }

    popup_window *popup = create_window(dc->dpy, &(dc->gc), X, Y, W, H);
    popup->free_reference = free_reference;
    return popup;
}

popup_window *create_window(Display *dsp, GC *gc, int x, int y, int w, int h) {
    popup_window *result = malloc(sizeof(popup_window));
    if (!result) {
        perror("could not allocate space for a popupwindow");
        return NULL;
    }

    int s = DefaultScreen(dsp);
    unsigned long wht = WhitePixel(dsp, s);
    unsigned long blk = BlackPixel(dsp, s);
    Window root = RootWindow(dsp, s);
    Window win = XCreateSimpleWindow(dsp, root, x, y, w, h, 1, blk, wht);
    XSelectInput(dsp, win, ExposureMask | KeyPressMask);
    XSetWindowAttributes set_attr;
    set_attr.override_redirect = True;
    XChangeWindowAttributes(dsp, win, CWOverrideRedirect, &set_attr);
    XMapWindow(dsp, win);

    result->x = x;
    result->y = y;
    result->w = w;
    result->h = h;
    result->win = win;
    result->ctx = *gc;
    result->dsp = dsp;
    result->canvas = XCreatePixmap(dsp, root, w, h, 32);

    pthread_create(&(result->thread), NULL, &window_action_listener, result);
    pthread_detach(result->thread);
    return result;
}

void free_window(popup_window *window) {
    if (window) {
        XFreePixmap(window->dsp, window->canvas);
        XDestroyWindow(window->dsp, window->win);
        free(window);
    }
}

