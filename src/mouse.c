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
    XSelectInput(dsp, win, ButtonPressMask);
    

    while(1) {
        XNextEvent(dsp, &xe);
        switch(xe.type) {
            case ButtonPress:
                selected = item_by_coord(xe.xbutton.x);
                if (selected && selected->click){
                    (selected->click)(selected, xe.xbutton.x);
                    drawmenu();
                }
                break;
        }
    }

    return NULL;
}

void init_mouse() {
    pthread_create(&mouse_listener, NULL, mouse_thread, NULL);
}
