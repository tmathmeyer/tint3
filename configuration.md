#CONFIGURATION

##Blocks
###overview
Blocks are simple units that describe each module in the bar. They start with a title in []'s followed by any number of lines, until the next header of a bar is encountered. 
The first word in any line are keywords. There are several built in keywords: {id, source, forground, background, format, type}, though anything can be used, so long as it does not start with a "[". After the keyword comes any text, followed by a newline. The text may contain spaces and any non-zero/non-newline character. 
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
- forground and background are preferred, however if left out they will defualt to the forground or background of the bar as a whole. 

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
  * N: the number of the radio button 
  * J: the number of the radio button, japanese (chinese) numerals
  * R: the number of the radio button, roman numerals
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
Each configuration file MUST include a "bar" block, which is denoted by double brackets around the word "bar". This is required for the bar to start. A basic example of a configuration is shown:
````
[[bar]]
  bordercolor #22EE88
  background #252117
  borderwidth 1
  padding 1
  margin 5
  location bottom
  left
    desktops
    active
  center
    date
    time
  right
    upload
    download
    weather
    volume
    batteryone
    batterytwo
````
There are a few required keywords in the bar block, including: 
* border color : # symbol followed by six digit RGB hexadecimal
* background :  # symbol followed by six digit RGB hexadecimal
* borderwidth : the width of the border of the bar, in pixels
* margin : the margins of the bar (how far away from the edges is the border), in pixels
* padding : the padding, or space between top and bottom of the letters / characters and the border, in pixels
* location, top or bottom (bottom doesn't work yet)
* left
* right
* center

The left, center, and right keywords are special however. Following them is any number of names, which correspond to the names of the blocks you have defined. These determine the order and location of the elements you have defined. For this reason, blocks cannot be named any of the default keywords in the bar block. 
----

##Things to Remember:
* ### all hex strings must be 6 digits long and preceded with a '#'
* ### spaces at the beginning of line are ignored, but tab characters may (read: will) break things.
* ### typos in the config file may result in segfaults (should fix this later, at least add warnings...)
* ### all lines that start with "#" are comment lines
