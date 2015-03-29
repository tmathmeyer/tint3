/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _T3_MOUSE_H_
#define _T3_MOUSE_H_

void *mouse_thread(void *data);
void init_mouse();
baritem *item_by_coord(unsigned int x);


#endif