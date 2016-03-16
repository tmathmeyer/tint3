#tint3 configuration

##Table of Contents
 * [Requirements](#requirements)
 * [Configuration Location](#configuration-location)
 * [Extended Color Codes](#extended-color-codes)
 * [Custom Format Specifiers](#custom-format-specifiers)
   * [Weather](#formatting-weather)
   * [X11 Workspaces](#formatting-workspaces)
   * [Clock](#formatting-clock)
 * []
 * []
 * []
 * []


##Requirements
#### Running
 - tint3 requires a linux system and X11 in all cases.
 - To use the weather feature, tint3 requires an internet connection.
 - To use the volume toggle feature, tint3 requires alsa.
#### Building
 - tint3 requires xlib, xcb, freetype2 in all cases
 - To compile with libnotify (by default), tint3 requires gdk-pixbuf, glib2.0, libpng. 
   See src/config.mk for compiling tint3 without libnotify support.


##Configuration Location
tint3 will read from three different locations in the following order of precedence:

 - command line: `tint3 -c /path/to/config`
 - `~/.tint3rc`
 - `~/.config/tint3/tint3rc`


##Extended Colors
tint3 uses standard hexadecimal colors with an optional alpha component.
Throughout the configuration, colors can be in one of four forms:

 - `#RGB`
 - `#ARGB`
 - `#RRGGBB`
 - `#AARRGGBB`


##Custom Format Specifiers
format strings in tint3 are C style, where the `%` character is used to
escape other characters for special meanings. literal % signs can be created with `%%`.

####Formatting Weather
The following characters will be replaced by the corresponding values when
escaped:

| Character |                  Parsed as                   |
| --------- | -------------------------------------------- |
| K | temperature in kelvin                                |
| F | temperature in degrees farenheight                   |
| C | temperature in degrees celcius                       |
| W | the current conditions (sunny, cloudy, snowing, etc) |
| A | pressure in atmospheres                              |
| P | pressure in millimeters of mercury (mmHg)            |
| H | humidity percentage                                  |
| D | dew point                                            |

For example, `%W, %F°F (%H%%)` would become `Mist, 44°F (93%)` on a Misty day at 44 degrees
Farenheight and 93 percent humidity.

####Formatting Workspaces
X11 allows multiple desktops. tint3 can display these desktops either using the names provided by
X11 or by assigning a number to them.

| character | Parsed as |
| ---------- | ---------- |
| n | the 1-indexed workspace number | 
| J | the 1-indexed workspace number, in Han Zi (chinese/japanese numerals) |
| R | the 1-indexed workspace number, in Roman numerals |
| N | The desktop name, if provided by X11 |

####Formatting Clock
The clock is formatted with the same set of format strings as the unix 
'date' utility, see `man date` for examples and instructions. 


##Bar Configuration
Each configuration file MUST include a "bar" block, which is denoted by double
brackets around the word "bar". This is required for tint3 to start. A simple
configuration would be
````
[[bar]]
  fontcolor #000000
  left
  center
    hello
  right

[hello]
  id text
  source hello
````
Indentation is not required.
The only required property in the `[[bar]]` section is the `fontcolor`
property, which is followed by a [color code](#extended-color-codes).
The left, center, and right properties are groups, which are followed by
names of [blocks](#blocks). There are also many optional properties
for the `[[bar]]` block:

| Property       | Default |                  Description                    |
| -------------- | ------- | ----------------------------------------------- |
| borderwidth    | 0 | the width of the bar border, in pixels                |
| padding        | 0 | the padding on each side (space between text and edge |
| margin         | 0 | the space between the bar and edge of monitor         |
| margin-left*   | 0 | the space between the bar and left edge of monitor    |
| margin-right*  | 0 | the space between the bar and right edge of monitor   |
| margin-top*    | 0 | the space between the bar and top edge of monitor     |
| margin-bottom* | 0 | the space between the mar and bottom edge of monitor  |
| width          | 100% | the width of the bar in pixels, defaults to 100%   |
| location       | TOP | location of bar, can be either TOP or BOTTOM        |
| bordercolor    | none | the [color code](#extended-color-codes) for the border |
| fontname       | * | the name of the font. Defaults to the X11 default     |
| background     | transparent | the [color code](#extended-color-codes) for the background |
| options        | none | developmental use |

*the margin specifiers can also be preceded by `manual-`, for example,
`margin-left 10` is the same as `manual-margin-left 100`.



















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
| | timeout | no | How often this block should refresh itself, in seconds |
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

##Location Examples
 * Boston,usa
 * Seattle,usa
 * Brisbane,aus
 * London,uk

### Example
Example rc file:
[tint3rc](https://github.com/tmathmeyer/tint3/blob/master/tint3rc)
produces:
![screenshot](https://github.com/tmathmeyer/tint3/blob/master/screenshot.png)
