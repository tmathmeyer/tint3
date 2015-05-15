/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <string.h>
#include "popup.h"

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
    result->context = *gc;
    result->dsp = dsp;
    return result;
}

void free_window(popup_window *window) {
    if (window) {
        puts("freed the graphics context");
        XDestroyWindow(window->dsp, window->win);
        puts("destroyed the window");
        free(window);
        puts("freed it!");
    }
}

