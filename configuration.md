#CONFIGURATION
### Example
Example rc file:
[tint3rc](https://github.com/tmathmeyer/tint3/blob/master/tint3rc)
produces:
![screenshot](https://github.com/tmathmeyer/tint3/blob/master/screenshot.png)


##Blocks
###overview
Blocks are simple units that describe each module in the bar. A block consists
of a title surrounded in a single pair of square braces, followed by any number
of lines that describe the block. A block is only ended by the beginning of
another block, or by <EOF>. Every line inside a block describes a property of
that block. All blocks share a set of recognized properties, including:

- source
- format
- shell
- id
- fontcolor
- forground [DEPRECATED, use fontcolor]
- background

Some blocks recognize other properties as well. The specifics are discussed
below. Properties may not start with a [ or #, but any other character will
work. The values associated with properties may contain any characters,
including spaces, [s and #s. An example of a block defining weather is shown:
````
[boston-weather]
  id weather
  format %W, ▉%C 
  fontcolor #aabbcc
  background #000000
  type temperature
  source Boston,usa
````
the "▉" character is drawn as a thermometer in the sakamoto font, packaged with
tint3.

####Required properties:
Every block is required to have a unique name, as well as an ID. The ID is used
to identify the type of block, and the name is used for posisioning within the
config file.

####Valid ID values:
| ID | Properties | Required | Notes |
| --- | ---------- | -------- | ----- |
| * | fontcolor | no | a [Colorcode](#Colorcodes) font color |
| | background | no | a [Colorcode](#Colorcodes) background color |
| | shell | no | a string that is piped to sh when the block is clicked |
| workspace | format | no | [Workspace Format](#Formatting-Workspaces) |
| | current | no | (future work!) |
| clock | format | yes | [Clock Format](#Formatting-Clock) |
| | timeout | no | How often this block should refresh itself, in microseconds |
| active | source | yes | must be "window_title" (future work!) |
| text | source | yes | a raw string to display |
| weather | source | yes | a location from openweathermap.org |
| | format | yes | [Weather Format](#Formatting-Weather) |
| graph | source | yes | [System Sources](#System-Sources) |
| scale | source | yes | [System Sources](#System-Sources) |
| | high_font | no | high value font [Colorcode](#Colorcodes) |
| | low_font | no | low value font [Colorcode](#Colorcodes) |
| | med_font | no | medium value font [Colorcode](#Colorcodes) |
| | high_color | no | high value background [Colorcode](#Colorcodes) |
| | low_color | no | low value background [Colorcode](#Colorcodes) |
| | med_color | no | med value background [Colorcode](#Colorcodes) |
| | cutoff:high | no | number 0-100, denotes change from med to high |
| | cutoff:low | no | number 0-100, denotes change from low to med |
| shell | source | yes | a command to execute, the output of which will be displayed |

###Format Strings:
tint3 uses standard-style format strings, where any characters escaped with a
% are liable to be replaced by corresponding values. 
####Formatting Weather
The following characters will be replaced by the corresponding values when
escaped:

| Character | Parsed as |
| ---------- | ----------- |
| K | temperature in kelvin |
| F | temperature in degrees farenheight |
| C | temperature in degrees celcius |
| W | the current conditions (sunny, cloudy, snowing, etc) |
| A | pressure in atmospheres |
| P | pressure in millimeters of mercury (mmHg) |
| H | humidity percentage |
| D | dew point |

For example, "%W, ▉%C" might become "Clouds, ▉57", depending on the weather
in the area.

####Formatting Workspaces

| character | Parsed as |
| ---------- | ---------- |
| n | the 1-indexed workspace number | 
| J | the 1-indexed workspace number, in Han Zi (chinese/japanese numerals) |
| R | the 1-indexed workspace number, in Roman numerals |
| N | The desktop name, if provided by X11 |

####Formatting Clock

The clock is formatted with the same set of format strings as the unix 
'date' utility

###Colorcodes
Color codes come in any of the following forms:
````
#rgb
#argb
#rrggbb
#aarrggbb
````
Where A, R, G, and B are hexadecimal values corresponding to the 
alpha (transparency), red, green, and blue componenets of a color.

###System Sources
There are currently three supported system resources:
 * alsa - requires a playback id and device
 * battery - requires the name from /sys/class/power_supply
 * network - requires an interface followed by "up" or "down"

Examples:
````
source alsa 0 Master
source battery BAT0
source network wlp3s0 up
````


## Bar Config
Each configuration file MUST include a "bar" block, which is denoted by 
double brackets around the word "bar". This is required for tint3 to start. 
A basic example of a configuration is shown:
````
[[bar]]
  fontcolor #22EE88
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
####Required Properties: 
 * fontcolor : # symbol followed by six digit RGB hexadecimal
 * borderwidth : the width of the border of the bar, in pixels
 * margin : the margins of the bar, in pixels
 * padding : the padding around the top and bottom of the letters, in pixels
 * location, only "top" (future work)
 * left
 * right
 * center

####Optional Properties
* background : [Colorcode](#Colorcodes) default is fully transparent
* bordercolor : [Colorcode](#Colorcodes) This is required if borderwidth is greater than 0
* fontname : the name of the font to use. Use the name of the ttf file,
than the actual font name. use 'fc-list' to get a list of font files and their names
* width : force the bar width to the given value (in *pixel*)

The left, center, and right keywords are special however. Following them is 
any number of names, which correspond to the names of the blocks you have
defined. These determine the order and location of the elements you have
defined. For this reason, blocks cannot be named any of the default keywords
in the bar block. 

You can also replace the `margin` property by `manual-margin-left`, `manual-margin-right`, `manual-margin-top` and `manual-margin-bottom`. Note that `margin-left` override `margin-right` when `width` is defined. 

##Location Examples
 * Boston,usa
 * Seattle,usa
 * Brisbane,aus
 * London,uk
