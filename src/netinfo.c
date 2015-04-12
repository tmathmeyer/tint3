/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <time.h>
#include "graph.h"
#include "netinfo.h"

unsigned long long old_down=0, old_up=0;
unsigned long net_tick = 0;

void update_network(char * interface) {
    if (time(NULL)-net_tick > 2) {
        time((time_t *)&net_tick);

        FILE * fp = fopen("/proc/net/dev", "r");
        if (fp == NULL) {
            return;
        }

        char * buffer = malloc(4096), * bt = buffer, temp = 0;
        while( (temp=fgetc(fp)) != EOF) {
            *(bt++) = temp;
        }
        fclose(fp);
        bt = strstr(buffer, interface);
        if (bt == 0) {
            free(buffer);
            return;
        }
        bt += (strlen(interface) + 2);

        unsigned long long up, down;
        sscanf(bt, "%llu %llu", &down, &up);

        int ud = up-old_up, dd = down-old_down;

        char * netname = calloc(0, strlen(interface) + 4);
        snprintf(netname, strlen(interface)+4, "%s up", interface);

        if (old_down != 0 && old_up != 0) {
            add_to_graph(dd, get_named_graph(netname));
            netname[strlen(interface)+1] = 'd';
            netname[strlen(interface)+2] = 'o';
            add_to_graph(ud, get_named_graph(netname));
        }

        old_down = down;
        old_up = up;

        free(buffer);
        free(netname);
    }
}

char * get_net_graph(baritem * item) {
    char source2[20] = {0};
    snprintf(source2, 20, item -> source + 8);
    strstr(source2, " ")[0] = 0;
    update_network(source2);
    return graph_to_string(get_named_graph((item -> source)+8));
}