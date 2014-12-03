/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _SCROLLING_H_
#define _SCROLLING_H_


 typedef struct rotation {
    char * strfull;
    short length;
    short start;
    short size;
    char swap;
    clock_t creation;
    clock_t speed;
} rotation;

void swap(char * A, char * B);
int update_rotation(rotation * rotation);
void draw_rotation(rotation * drawme);

rotation * make_rotation(char * msg, int size);


#endif