#ifndef __CONFIGURATION__
#define __CONFIGURATION__

#define BAR_FOREGROUND "#222222"
#define BAR_BACKGROUND "#bbbbbb"

/*
 * NFE = not fully implemented
 * NYE = not yet implemented
 *
 * Elements are:
 *   B -> battery
 *   T -> time
 *   W -> window name
 *   H -> hostname       NYE
 *   D -> desktops       NFE
 *   M -> music (mpd?)   NYE
 */
#define LEFT_ALIGN "W"
#define RIGHT_ALIGN "TMNB"
#define CENTER_ALIGN "D"

// Colors
#define BATTERY_FOREGROUND_HIGH "#0f0"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#f00"
#define CLOCK_FOREGROUND "#f80"
#define NET_UP_FOREGROUND "#f80"
#define NET_UP_BACKGROUND "#840"
#define NET_DOWN_FOREGROUND "#08f"
#define NET_DOWN_BACKGROUND "#048"

// Net
#define NETIFACE "eth0"


#define FONT "sakamoto-11"


#endif
