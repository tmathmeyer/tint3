#CONFIGURATION

##Blocks
###overview
 The main subtype of the tint configuration, blocks look similar to other configuration file sections:
````
[download]
  id graph
  source network wlp3s0 down
  forground #ff00ff
  background #ddffaa
````

####Required Descriptors:
- the name of the block, in the previous case, it was "download"
- the id of the block, in his case, it is a graph
- the forground and background, which are #'s followed by 6 digit RGB hexadecimal representations

####Optional descriptors:
* format: some modules require a format string, like the clock module
* type: the scale modules may require a type, for determining coloring schemes:
    * temperature is colored based on a degrees celcius scale, where 0 is blue and 100 is red
    * battery is colored based on a percent scale, where 0 is red, and 100 is green
    * more may be added later
* source: the most commonly used modifier, source points the bar its source of information. Examples include:
    * workspaces - pull workspace (virtual desktop) data from X11
    * window_title - pull current active window title from X11te r
    * clock - call unix command 'data' with a format string
    * weather XXXX - pull weather data from US Govt, will not work for other countries. XXXX is a four character airport code, which can be found on weather.noaa.gov. This may include a format string in the future, but for now results in "temp/dewpoint", in degrees celcius, ie 44/39 is the current reading.
    * network [interface] [type] where [interface] is the network interface (for my laptop it is wlp3s0) and [state] is either up or down, for upload and download.
    * battery X, where X is the numerical ID of the battery, found in /sys/class/power_supply/BATTX
    * alsa X, where X is the id of the sound card. this can be found easily using alsamixer.

####IDs:
* text - plain text
    * any characters following the source descriptor in a text block will be interpreted as a literal, provided they are not matched by one of the previously described sources
* radio - a range of either filled or unfilled circles
* scale - a number / symbol with a color range for background (and often forground)
* graph - a graph of certain things (battery percent, networks)

-----

## Bar Config
 The main config section for tint3, this block brings together all the named blocks. it must come AFTER all blocks have been declared. A basic example is:
````
[[bar]]
  bordercolor #EE8822
  borderwidth 1
  padding 1
  margin 5
  background #252117
  location top
  left 2
    desktops
    active
  center 2
    date
    time
  right 4
    download
    weather
    volume
    battery
````

####Required:
* border color : # symbol followed by six digit RGB hexadecimal
* background :  # symbol followed by six digit RGB hexadecimal
* borderwidth : the width of the border of the bar, in pixels
* margin : the margins of the bar (how far away from the edges is the border), in pixels
* padding : the padding, or space between top and bottom of the letters / characters and the border, in pixels
* location, top or bottom (bottom doesn't work yet)
* left / right / center
    * followed by a number, representing the number of blocks  in that section
    * followed by N block names, where N is the number following left / right / center

----

##Things to Remember:
* ### all hex strings must be 6 digits long
* ### block names and [[bar]] are padded with no spaces, their required descriptors are padded with two spaces, and the block lists in left / right / center are padded with four spaces. NO TABS
* ### typos in the config file may result in segfaults
* ### all lines that start with "#" are comment lines