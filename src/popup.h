#ifndef _T3_POPUP_H_
#define _T3_POPUP_H_

#include <X11/Xlib.h>
#include <pthread.h>
#include <stdbool.h>

typedef enum window_position {
    AT_MOUSE,
    CENTERED,
    ALIGNED
} window_position;

#define uint unsigned int
typedef struct popup_window {
    uint x, y, w, h;
    Window win;
    GC context;
    Display *dsp;
    pthread_t thread;
    void (* free_reference)(void);
} popup_window;

popup_window *create_window(Display *dsp, GC *gc, int x, int y, int w, int h);
void free_window(popup_window *window);
popup_window *spawn_popup(
        baritem *item,
        window_position fallback,
        unsigned int click_xpos,
        void (* free_reference)(void));

#endif
