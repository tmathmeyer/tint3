/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _T3_NETINFO_H_
#define _T3_NETINFO_H_

#include "graph.h"

typedef struct {
    unsigned int timeout;
    unsigned long lastime;
    graph * up;
    graph * down;
} net_info;

net_info * get_net_info(void);
char * get_net_graph(baritem * item);

#endif