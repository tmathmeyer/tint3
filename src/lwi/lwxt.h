/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */
 
#ifndef _LWXT_H_
#define _LWXT_H_

int lwxt_setup(void);
char* expand_escapes(const char *src);
int get_active_window(xcb_window_t *win);
void get_window_title(xcb_window_t win, char *title, size_t len);
void get_title(char * buffer, size_t buf_len);

#define MAXLEN            256
#define MIN(A, B)         ((A) < (B) ? (A) : (B))

#endif

