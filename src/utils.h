
#ifndef __UTILS__
#define __UTILS__



typedef struct batt_info {
	char * icon;
	int  percentage;
} batt_info;

batt_info * get_battery_information();


#endif