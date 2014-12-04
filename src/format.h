/*
 * Copyright (C) 2014 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _FORMAT_H_
#define _FORMAT_H_

typedef struct format_map {
	char formatID;
	int (*formatter)(int, char *);
	struct format_map * next;
} format_map;

int format_string(char * dest, char * fmt, format_map * formatmap);

#endif