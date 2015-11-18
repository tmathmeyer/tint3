/*
 * Copyright (C) 2015 Ted Meyer
 *
 * see LICENSING for details
 *
 */

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int get_battery_percent(char *forbattery) {
    char *path = "/sys/class/power_supply";
    FILE *fp;
    char battery_read[512];
    char tmp[64];

    int energy_now = 1;
    int energy_full = 1;

    snprintf(battery_read, sizeof battery_read, "%s/%s/%s", path, forbattery, "energy_now");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_now = atoi(tmp);
        fclose(fp);
    } else {
        snprintf(battery_read, sizeof battery_read, "%s/%s/%s", path, forbattery, "charge_now");
        fp = fopen(battery_read, "r");
        if(fp != NULL) {
            if (fgets(tmp, sizeof tmp, fp)) energy_now = atoi(tmp);
            fclose(fp);
        } else {
            perror(battery_read);
            return -1;
        }
    }

    snprintf(battery_read, sizeof battery_read, "%s/%s/%s", path, forbattery, "energy_full");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_full = atoi(tmp);
        fclose(fp);
    } else {
        snprintf(battery_read, sizeof battery_read, "%s/%s/%s", path, forbattery, "charge_full");
        fp = fopen(battery_read, "r");
        if(fp != NULL) {
            if (fgets(tmp, sizeof tmp, fp)) energy_full = atoi(tmp);
            fclose(fp);
        } else {
            return -2;
        }
    }
    return energy_now / (energy_full / 100);
}

