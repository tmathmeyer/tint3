#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <malloc.h>

#include "test.h"
#include "../source/tint3.h"
#include "../source/vdesk.h"
#include "../source/dlist.h"
#include "../source/draw.h"

char *strdup(const char *s);
void simple_setup() {
    DC *dc = initdc();
    XSetWindowAttributes wa;
    XVisualInfo vinfo;
    XMatchVisualInfo(dc->dpy, DefaultScreen(dc->dpy), 32, TrueColor, &vinfo);
    wa.colormap = XCreateColormap(dc->dpy,
            DefaultRootWindow(dc->dpy),
            vinfo.visual,
            AllocNone);
    wa.border_pixel = 0;
    wa.background_pixel = 0;
    dc->wa = wa;
    NET_CURRENT_DESKTOP = XInternAtom(dc->dpy, "_NET_CURRENT_DESKTOP", 0);
    NET_NUMBER_DESKTOPS = XInternAtom(dc->dpy, "_NET_NUMBER_OF_DESKTOPS", 0);
    NET_DESKTOP_NAMES   = XInternAtom(dc->dpy, "_NET_DESKTOP_NAMES", 0);
    set_root(RootWindow(dc->dpy, DefaultScreen(dc->dpy)));
    set_dc(dc);
}


START_TEST(mallinfo_supported) {
    struct mallinfo init = mallinfo();
    struct mallinfo post = mallinfo();
    ASSERT_REF(init.uordblks, post.uordblks,
            "mallinfo uordblks not returning garbage");
    
    void *c = malloc(200);
    post = mallinfo();
    ASSERT_REF(init.uordblks, post.uordblks-208,
            "mallinfo uordblks returns correct allocated memory amount (bytes)");
    free(c);
    post = mallinfo();
    ASSERT_REF(init.uordblks, post.uordblks,
            "mallinfo uordblks returns nothing after free");

    
    ASSERT_SUCCESS();
}

START_TEST(dlist_deep_free_custom_no_leak) {
    struct mallinfo init = mallinfo();
    dlist *list = dlist_new();
    dlist_add(list, strdup("hello"));
    dlist_add(list, strdup("world"));
    dlist_add(list, strdup("test"));

    ASSERT_REF(list->size, 3, "number of elements not 3");
    dlist_deep_free_custom(list, &free);

    struct mallinfo post = mallinfo();
    ASSERT_REF(init.uordblks, post.uordblks, "dlist not leaking memory");

    ASSERT_SUCCESS();
}


START_TEST(desktops_memory_freed) {
    baritem *result = malloc(sizeof(baritem));
    result->default_colors = make_baritem_colours("#cfc", "#f0f");
    result->format = "%N";
    result->source = NULL;
    result->shell = NULL;
    result->elements = dlist_new();
    result->options = dlist_new();
    result->click = NULL;
    result->xstart = 0;
    result->length = 0;
    result->name = "desktops";
    result->update = NULL;
    
    result->elements = get_desktops_info(result);
    dlist_deep_free_custom(result->elements, &free_stylized);
    
    for(int i=0; i<10; i++) {
        struct mallinfo init = mallinfo();
        result->elements = get_desktops_info(result);
        
        struct mallinfo tmp = mallinfo();
        printf("[%i]allocated size: %i\n", i, tmp.uordblks - init.uordblks);

        ASSERT_REF(result->elements->size, 4, "number of desktops not 4");
        dlist_deep_free_custom(result->elements, &free_stylized);

        struct mallinfo post = mallinfo();
        ASSERT_REF(init.uordblks, post.uordblks,
                "get_desktops_info leaking memory");
    }
    ASSERT_SUCCESS();
}


int main() {
    simple_setup();
    int runs = 1;
    while(runs --> 0) {
        RUN_TESTS();
    }
}
