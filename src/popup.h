#include <X11/Xlib.h>

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
} popup_window;

popup_window *create_window(Display *dsp, GC *gc, int x, int y, int w, int h);
void free_window(popup_window *window);
