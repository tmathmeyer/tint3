#ifndef __CONFIGURATION__
#define __CONFIGURATION__

#define BAR_FOREGROUND "#222222"
#define BAR_BACKGROUND "#bbbbbb"

/*
 *
 * Elements are:
 *   B -> battery
 *   T -> time
 *   D -> desktops
 *   N -> network down graph   NOTE: uses custom icons (see sakamoto font)
 *   M -> network up graph     NOTE: uses custom icons (see sakamoto font)
 *   W -> Weather              NOTE: uses custom icons (see sakamoto font)
 *
 *   Any character not in the list will show up as a simple string in the bar
 *
 */



#define LEFT_ALIGN "T"
#define RIGHT_ALIGN " uM dN WB"
#define CENTER_ALIGN "D"

// Colors
#define BATTERY_FOREGROUND_HIGH "#0f0"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#f00"
#define CLOCK_FOREGROUND "#5f5"
#define NET_UP_FOREGROUND "#f80"
#define NET_UP_BACKGROUND "#222"
#define NET_DOWN_FOREGROUND "#08f"
#define NET_DOWN_BACKGROUND "#222"

// Net
#define NETIFACE "eth0"


#define FONT "sakamoto-11"

#define DESKTOP_QUERY "bspc query -D | grep -nx $(bspc query --desktops --desktop focused)"
#define DESKTOP_COUNT "bspc query -D | wc -l"
#define DESKTOP_ZIDEX 1 // is DESKTOP_QUERY from {0 -> DESKTOP_COUNT-1}?? or {1 -> DESKTOP_COUNT}
#define DESKTOP_CURRENT "◆" //◇◆◈○◉
#define DESKTOP_DEFAULT "◇"

#endif
