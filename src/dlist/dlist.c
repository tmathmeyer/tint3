
#define _DEFAULT_SOURCE

#include "dlist.h"
#include <stdlib.h>
#include <stdio.h>

int dlist_add(dlist *d, void *data){
    if (cap(d) <= size(d)+1) {
        cap(d) *= 2;
        void **update = realloc(d->data, cap(d)*sizeof(void*));
        if (!update) {
            return 0;
        }
        d->data = update;
    }
    dget(d, size(d)) = data;
    size(d) ++;
    return 1;
}

dlist *dlist_new() {
    dlist *res = malloc(sizeof(dlist));
    if (!res) {
        return NULL;
    }

    size(res) = 0;
    cap(res) = 10;
    res->data = malloc(sizeof(void*)*10);

    return res;
}

void dlist_deep_free(dlist *d) {
    void *data;
    each(d, data) {
        if (data) {
            free(data);
        }
    }
    dlist_free(d);
}

void dlist_deep_free_custom(dlist *d, void(*cfree)(void *)) {
    void *data;
    each(d, data) {
        if (data) {
            cfree(data);
        }
    }
    dlist_free(d);
}

void dlist_free(dlist *d) {
    free(d->data);
    free(d);
}
