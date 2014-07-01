#include "utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "defaults.h"

/*
⮎
⮒
⮏
⮑
⮐
*/


batt_info * get_battery_information() {
    batt_info * bi = malloc(sizeof (batt_info));
    bi -> icon = "X";
    bi -> percentage = 0;

    char * path = "/sys/class/power_supply/BAT0";
    FILE * fp;
    char battery_read[512];
    char tmp[64];

    int energy_now = 1;
    int energy_full = 1;
    int battery_charging = 0;


    snprintf(battery_read, sizeof battery_read, "%s/%s", path, "energy_now");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_now = atoi(tmp);
        fclose(fp);
    } else {
        return NULL;
    }

    snprintf(battery_read, sizeof battery_read, "%s/%s", path, "energy_full");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_full = atoi(tmp);
        fclose(fp);
    } else {
        return NULL;
    }

    snprintf(battery_read, sizeof battery_read, "%s/%s", path, "status");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) {
            battery_charging = (strncmp(tmp, "Discharging", 11) != 0);
        }
        fclose(fp);
    } else {
        return bi;
    }

    bi -> percentage = energy_now / (energy_full / 100);
    if (bi -> percentage > 90) {
        bi -> icon = "⮒";
    } else if (bi -> percentage > 65) {
        bi -> icon = "⮏";
    } else if (bi -> percentage > 10) {
        bi -> icon = "⮑";
    } else if (bi -> percentage > 0) {
        bi -> icon = "⮐";
    } else {
        bi -> icon = "?";
    }
    if (battery_charging) {
        bi -> icon = "⮎";
    }

    return bi;
}


unsigned long long old_down, old_up;
graph * gu = NULL;
graph * gd = NULL;
char * netiface = NETIFACE;
int matching_length = -1;
char ** get_net_info (void) {
    int i = 0;
    if (gu == NULL) {
        gu = make_new_graph();
    }
    if (gd == NULL) {
        gd = make_new_graph();
    }
    if (matching_length == -1) {
        matching_length = strlen(netiface);
    }
    FILE * fp = fopen("/proc/net/dev", "r");
    if (fp == NULL) {
        return NULL;
    }

    unsigned long long up, down;
    char * name = malloc(10);
    for(i=0; i<10; i++) {
        name[i] = 0;
    }

    while ( (i = strncmp(name, netiface, matching_length)) != 0) {
        up = fscanf(fp, "%s", name);
    }

    free(name);
    i = fscanf(fp, "%llu %llu", &down, &up);
    unsigned long long diff_down = down - old_down;
    unsigned long long diff_up = up - old_up;
    if (old_up != 0) {
        add_to_graph( (int)diff_up , gu);
    }
    if (old_down != 0) {
        add_to_graph( (int)diff_down , gd);
    }
    old_down = down;
    old_up = up;
    fclose(fp);

    char ** result = malloc(sizeof(char*) * 2);
    result[0] = graph_to_string(gu);
    result[1] = graph_to_string(gd);
    return result;
}












char * bar_map = "▁▂▃▄▅▆▇";

void recalc_max(graph * gr) {
    int ctr = 0, i = 0;
    for(; i < GRAPHLENGTH; i++) {
        if ((gr -> graph)[i] > ctr) {
            ctr = (gr -> graph)[i];
        }
    }
    gr -> max = ctr;
}

void add_to_graph(int i, graph * gr) {
    int old = (gr -> graph)[gr -> start];
    (gr -> graph)[gr -> start] = i;
    if (i > gr -> max) {
        gr -> max = i;
    }
    if (old ==  gr -> max) {
        recalc_max(gr);
    }
    gr -> start = ((gr -> start)+1)%GRAPHLENGTH;
}

char * graph_to_string(graph * gr) {
    char * result = malloc(GRAPHLENGTH*3+1);
    int ctr = 0, i = 0;
    for(; i < GRAPHLENGTH*3+1; i++) {
        result[i] = 0;
    } i = 0;
    for(; i < GRAPHLENGTH; i++) {
        int val = (gr -> graph)[((gr -> start)+i)%GRAPHLENGTH]*7/((gr -> max)+1) + 1;
        result[ctr++] = bar_map[(val-1)*3+0];
        result[ctr++] = bar_map[(val-1)*3+1];
        result[ctr++] = bar_map[(val-1)*3+2];
    }
    result[GRAPHLENGTH*3] = 0;
    return result;
}

graph * make_new_graph() {
    graph * g = NULL;
    g = malloc(sizeof(graph));
    g -> start = 0;
    g -> max = 0;
    int i=0; for(; i<GRAPHLENGTH; i++) {
        (g->graph)[i] = 0;
    }
    return g;
}