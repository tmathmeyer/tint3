/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE
#define DEBUG_MODE 0
#include <stdio.h>
#include "format.h"

int BAD(int start, char * dest) {
    if (DEBUG_MODE) {
        puts(dest);
    }
    return start;
}

typedef int (*textformatter)(int, char *);
textformatter lookup(format_map * fmt, char id) {
    if (!fmt) {
        return BAD;
    } else if (id == fmt -> formatID) {
        return fmt -> formatter;
    }
    return lookup(fmt -> next, id);
}

int format_string(char * dest, char * fmt, format_map * formatmap) {
    int e = 0;
    int i = 0;
    int d = 0;
    for(;fmt[i];i++) {
        if (fmt[i] == '%') {
            d = !d;
        } else if (d) {
            d = !d;
            e = lookup(formatmap, fmt[i])(e, dest);
        } else {
            dest[e++] = fmt[i];
        }
    }
    return e;
}
