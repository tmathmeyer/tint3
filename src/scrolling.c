/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "scrolling.h"


void swap(char * A, char * B) {
    *A ^= *B;
    *B ^= *A;
    *A ^= *B;
}

rotation * make_rotation(char * msg, int size) {
    rotation * result = malloc(sizeof(rotation));
    result -> length = strlen(msg) + size;
    result -> start = size;
    result -> size = size;
    result -> swap = 0;

    result -> speed = 50;

    result -> strfull = malloc(result -> length + size + size);
    memset(result -> strfull, ' ', result -> length + size);
    strncpy(result -> strfull + size, msg, result -> length);
    (result -> strfull)[result -> length + size - 1] = 0;
    result -> creation = clock();

    swap(&(result -> swap), 
            (char *)((result -> strfull)+(result -> start)+(result -> size)));

    return result;
}

int update_rotation(rotation * rotation) {
    if (clock()-(rotation->creation) > rotation -> speed) {
        rotation -> creation = clock();
        swap(&(rotation -> swap),
                (char *)((rotation -> strfull)
                        +(rotation -> start)
                        +(rotation -> size)));
        rotation -> start = (rotation -> start + 1) % (rotation -> length);
        swap(&(rotation -> swap),
                (char *)((rotation -> strfull)
                        +(rotation -> start)
                        +(rotation -> size)));
        return 1;
    }
    return 0;
}
