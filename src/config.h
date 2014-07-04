#ifndef __CONFIGURATION__
#define __CONFIGURATION__

#define BAR_FOREGROUND "#222222"
#define BAR_BACKGROUND "#bbbbbb"

/*
 * NFI = not fully implemented
 * NYI = not yet implemented
 *
 * Elements are:
 *   B -> battery
 *   T -> time
 *   W -> Weather        NOTE: uses custom icons (see sakamoto font)
 *   H -> hostname       NYI
 *   D -> desktops       NFI
 *   M -> music (mpd?)   NYI
 *  ' '-> spacer(one space)
 */
#define LEFT_ALIGN "T"
#define RIGHT_ALIGN " uM dN WB"
#define CENTER_ALIGN ""

// Colors
#define BATTERY_FOREGROUND_HIGH "#0f0"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#f00"
#define CLOCK_FOREGROUND "#f80"
#define NET_UP_FOREGROUND "#f80"
#define NET_UP_BACKGROUND "#222"
#define NET_DOWN_FOREGROUND "#08f"
#define NET_DOWN_BACKGROUND "#222"

// Net
#define NETIFACE "wlp2s0"


#define FONT "sakamoto-11"


#endif
