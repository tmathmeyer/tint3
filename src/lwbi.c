/*
 * Copyright (C) 2014 Ted Meyer
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int get_battery_percent(char * forbattery) {
    char * path = "/sys/class/power_supply";
    FILE * fp;
    char battery_read[512];
    char tmp[64];

    int energy_now = 1;
    int energy_full = 1;
    //int battery_charging = 0;


    snprintf(battery_read, sizeof battery_read, "%s/%s/%s", path, forbattery, "energy_now");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_now = atoi(tmp);
        fclose(fp);
    } else {
        perror(battery_read);
        return -1;
    }

    snprintf(battery_read, sizeof battery_read, "%s/%s/%s", path, forbattery, "energy_full");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) energy_full = atoi(tmp);
        fclose(fp);
    } else {
        return -2;
    }

/*
    snprintf(battery_read, sizeof battery_read, "%s/%s/%s", path, forbattery, "status");
    fp = fopen(battery_read, "r");
    if(fp != NULL) {
        if (fgets(tmp, sizeof tmp, fp)) {
            battery_charging = (strncmp(tmp, "Discharging", 11) != 0);
        }
        fclose(fp);
    } else {
        return -1;
    }
*/

    return energy_now / (energy_full / 100);
}