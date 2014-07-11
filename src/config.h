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



#define LEFT_ALIGN "A"
#define RIGHT_ALIGN "T uM dN WB"
#define CENTER_ALIGN "D"

// Colors
#define BATTERY_FOREGROUND_HIGH "#0f0"
#define BATTERY_BACKGROUND_HIGH "#282"
#define BATTERY_FOREGROUND_MED "#ff0"
#define BATTERY_BACKGROUND_MED "#886"
#define BATTERY_FOREGROUND_LOW "#f00"
#define CLOCK_FOREGROUND "#5f5"
#define NET_UP_FOREGROUND "#f80"
#define NET_DOWN_FOREGROUND "#08f"

// Net
#define NETIFACE "eth0"


#define FONT "sakamoto-11"

#define DESKTOP_QUERY "bspc query -D | grep -nx $(bspc query --desktops --desktop focused)"
#define DESKTOP_COUNT "bspc query -D | wc -l"
#define DESKTOP_ZIDEX 1 // is DESKTOP_QUERY from {0 -> DESKTOP_COUNT-1}?? or {1 -> DESKTOP_COUNT}
#define DESKTOP_CURRENT "◆" //◇◆◈○◉
#define DESKTOP_DEFAULT "◇"

#define DESKTOP_FOREGROUND "#ff8800"
#define DESKTOP_BACKGROUND "#222222"

#define CURRENT_WINDOW_BACKGROUND "#222222"
#define CURRENT_WINDOW_FOREGROUND "#bbbbbb"

#define CURRENT_WINDOW_GETTER "xtitle"

#endif
