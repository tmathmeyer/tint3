#include "utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

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

