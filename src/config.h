#ifndef __CONFIGURATION__
#define __CONFIGURATION__

/** COLORS **/
/* BAR DEFAULTS */
#define BAR_FOREGROUND "#000000"
#define BAR_BACKGROUND "#eeeeee"

/* BATTERY COLORS */
#define BATTERY_FOREGROUND_HIGH "#0f0"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#f00"

/* CLOCK COLORS */
#define CLOCK_BACKGROUND "#000000"
#define CLOCK_FOREGROUND "#00aaaa"

/* NETWORK COLORS */
#define NET_UP_FOREGROUND "#777"
#define NET_UP_BACKGROUND "#000"
#define NET_DOWN_FOREGROUND "#aaa"
#define NET_DOWN_BACKGROUND "#000"

/* MULTI-DESKTOP COLORS */
#define DESKTOP_FOREGROUND "#00aaaa"
#define DESKTOP_BACKGROUND "#000000"

/* ACTIVE WINDOW TITLE COLORS */
#define CURRENT_WINDOW_BACKGROUND "#000000"
#define CURRENT_WINDOW_FOREGROUND "#00aaaa"

#define WEATHER_BACKGROUND "#000000"

/* MPD INFO COLORS */
#define MPD_INFO_BACKGROUND "#000000"
#define MPD_INFO_FOREGROUND "#00aaaa"



/** UTILS **/
/* NETWORK INTERFACE */
#define NETIFACE "eth0"

/* MULTIDESKTOP QUERY */
#define DESKTOP_QUERY "bspc query -D | grep -nx $(bspc query --desktops --desktop focused)" // executable
#define DESKTOP_COUNT "bspc query -D | wc -l" // executable
#define DESKTOP_ZIDEX 1 // is DESKTOP_QUERY from {0 -> DESKTOP_COUNT-1}?? or {1 -> DESKTOP_COUNT}
#define DESKTOP_CURRENT "◆" //◇◆◈○◉
#define DESKTOP_DEFAULT "◇"

/* FONT */
#define FONT "sakamoto-11"

/* WINDOW TITLE QUERY */
#define CURRENT_WINDOW_GETTER "xtitle" // executable

/* SOUND CARD ID */
#define ALSA_DEVICE_ID "1"

/* microseconds between redrawing */
#define UPDATE_DELAY 200000

/* MPD INFO FORMAT STRING
 * Describes the format of the MPD info utility
 * A string containing any of the following patterns:
 *  %n  The name of the current song.
 *  %a  The artist of the current song.
 *  %l  The album title of the current song.
 *  %c  The current time in the song.
 *  %r  The amount of time remaining in the current song.
 *  %t  The total time of the current song.
 *  %%  Print out a literal '%' character.
 */
#define MPD_INFO_FORMAT_STRING "%n - %a | %c/%t"

/*
 *
 * Elements are:
 *   A -> active window
 *   B -> battery
 *   T -> time
 *   D -> desktops
 *   N -> network down graph   NOTE: uses custom icons (see sakamoto font)
 *   M -> network up graph     NOTE: uses custom icons (see sakamoto font)
 *   W -> Weather              NOTE: uses custom icons (see sakamoto font)
 *   V -> volume
 *   E -> MPD info
 *
 *   Any character not in the list will show up as a simple string in the bar
 *
 */

#define LEFT_ALIGN ""
#define RIGHT_ALIGN "EMNW"
#define CENTER_ALIGN "T"



#endif
