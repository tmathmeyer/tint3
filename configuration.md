#CONFIGURATION

##Blocks
###overview
Blocks are simple units that describe each module in the bar. They start with a title in []'s followed by any number of lines indented with TWO spaces.
After the two spaces follows one of the following keywords: {id, source, forground, background, format, type}, which in turn is followed by a second and more descriptive identifier.
An example is shown below (the ▉ will show up as a thermometer in the bar.)
````
[weather]
  id weather
  format %W, ▉%C 
  forground #aabbcc
  background #000000
  type temperature
  source Worcester,usa
````

####Required Keywords:
- the name of the block; it appears in the []'s
- the id of the block, which can be any of {text, radio, weather, scale, graph}
- the forground and background, which are #s (octothorpes) followed by 6 digit RGB hexadecimal representations

####Optional keywords:
* format: some modules require a format string, like the clock module. These will be covered in more detail later
* type: currently unused, though it may be used for custom color gradients in the future
* source: the most commonly used modifier, source points the bar its source of information. Examples include:
    * workspaces - pull workspace (virtual desktop) data from X11
    * window_title - pull current active window title from X11te r
    * clock - call unix command 'date' with the provided format string
    * network [interface] [type] where [interface] is the network interface (for my laptop it is wlp3s0) and [state] is either up or down, for upload and download.
    * battery X, where X is the numerical ID of the battery, found in /sys/class/power_supply/BATTX
    * alsa X, where X is the id of the sound card. this can be found easily using alsamixer.
    * Boston,usa : the source string that I personally use for weather information. (see http://openweathermap.org/ for other locations. They can be tricky to find sometimes)

####IDs:
* text - plain text
    * any characters following the source descriptor in a text block will be interpreted as a literal, provided they are not matched by one of the previously described sources
* radio - a range of either filled or unfilled circles
* scale - a number / symbol with a color range for background (and often forground)
* graph - a graph of certain things (battery percent, networks)
* weather - pulls from the website [[http://api.openweathermap.org]] with your location (you have to enter it)

####Format Strings:
currently, there are three modules that use the format strings, {id:text, source:clock}, {id:radio}, and {id:weather}. The format strings for the former can be found by typing `man date` into any unix shell. The format parameters for the weather module are:

  * K: temperature in kelvin
  * F: temperature in degrees farenheight
  * C: temperature in degrees celcius
  * W: the current conditions (sunny, cloudy, snowing, etc)
  * A: pressure in atmospheres
  * P: pressure in millimeters of mercury (mmHg)
  * H: humidity percentage
  * D: dew point

The format parameters used in the radio module are as follows:
  * N: the number of the radio button (0-indexed)
  * M: the number of the radio button (1-indexed)
  * R: the number of the radio button (0-indexed, roman numeral)
  * I: inverted
 
An example of the radio configutation for desktops might be:
````
[desktops]
  id radio
  source workspaces
  format %R ◆
  forground #dddddd
````
If I were on virtual desktop 2 of 6 for example, this would show "I ◆ III IV V VI",
being on desktop 5 of 6 would show "I II III IV ◆ VI"

the general format is the default character/formatter [space] selected character/formatter,
so the string "◇ %R" on desktop 5 of 6 would look like "◇ ◇ ◇ ◇ V ◇".
Note: this DOES work with unicode (3 byte) characters.

-----

## Bar Config
 The main config section for tint3, this block brings together all the named blocks. it must come AFTER all blocks have been declared. A basic example is:
````
[[bar]]
  bordercolor #22EE88
  background #252117
  borderwidth 1
  padding 1
  margin 5
  location bottom
  left 2
    desktops
    active
  center 2
    date
    time
  right 6
    upload
    download
    weather
    volume
    batteryone
    batterytwo
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
* ### all hex strings must be 6 digits long and preceded with a '#'
* ### block names and [[bar]] are padded with no spaces, their required descriptors are padded with two spaces, and the block lists in left / right / center are padded with four spaces. NO TABS
* ### typos in the config file may result in segfaults (should fix this later, at least add warnings...)
* ### all lines that start with "#" are comment lines