/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#ifndef _T3_NETINFO_H_
#define _T3_NETINFO_H_

#include "baritem/baritem.h"

dlist *get_net_graph(baritem *);
dlist *get_multi_net_graph(baritem *);
void update_network(char *);

#endif

