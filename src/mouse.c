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
    Display *dsp = XOpenDisplay(NULL);
    XSelectInput(dsp, root, ButtonPressMask | PointerMotionMask);
    XEvent xe;
    while(1) {
        XNextEvent(dsp, &xe);
        printf("%s\n", xe.type);
    }

    return NULL;
}

void init_mouse() {
	pthread_create(&mouse_listener, NULL, mouse_thread, NULL);
}

baritem *item_by_coord(unsigned int x) {

}
