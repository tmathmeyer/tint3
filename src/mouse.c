/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */
#define _DEFAULT_SOURCE

#include <pthread.h>
#include "mouse.h"
#include "tint3.h"

#include <stdio.h>


static pthread_t mouse_listener;

void *mouse_thread(void *data) {
    (void) data;
    XEvent xe;
    baritem *selected;
    Display *dsp;

    dsp = XOpenDisplay(NULL);
    XSelectInput(dsp, win, ButtonPressMask | PointerMotionMask | LeaveWindowMask);
    

    baritem *last_hover = NULL;
    while(1) {
        XNextEvent(dsp, &xe);
        switch(xe.type) {
            case ButtonPress:
                selected = item_by_coord(xe.xbutton.x);
                if (selected && selected->click ){
                    (selected -> click)(selected, xe.xbutton.x);
                }
            	break;
            case MotionNotify:
                selected = item_by_coord(xe.xbutton.x);
                if (last_hover&&last_hover->mouse_exit&&last_hover!=selected){
                    (last_hover -> mouse_exit)(last_hover);
                }
                if (selected && selected->mouseover) {
                    (selected -> mouseover)(selected, xe.xbutton.x);
                }
                last_hover = selected;
                break;
            case LeaveNotify:
                selected = item_by_coord(xe.xbutton.x);
                if (selected && selected->mouse_exit) {
                    (selected -> mouse_exit)(selected);
                }
                last_hover = NULL;
                break;
        }
    }

    return NULL;
}

void init_mouse() {
    pthread_create(&mouse_listener, NULL, mouse_thread, NULL);
}
