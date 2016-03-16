#tint3 configuration

##Table of Contents
 * [Requirements](#requirements)
 * [Configuration Location](#configuration-location)
 * [Extended Color Codes](#extended-color-codes)
 * [Custom Format Specifiers](#custom-format-specifiers)
   * [Weather](#formatting-weather)
   * [X11 Workspaces](#formatting-workspaces)
   * [Clock](#formatting-clock)
   * [Location](#formatting-location)
 * [Bar Configuration](#bar-configuration)
 * [Blocks](#blocks)
   * [workspace](#workspace)
   * [clock](#clock)
   * [active](#active)
   * [text](#text)
   * [weather](#weather)
   * [scale](#scale)
   * [graph](#graph)
   * [shell](#shell)
 * [Example](#example)


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



##Extended Color Codes
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

####Formatting Location
some example locations are:
 * Boston,usa
 * Seattle,usa
 * Brisbane,aus
 * London,uk
See openweathermap.org for how these locations work



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
Blocks are simple units that describe each module in the bar. A block consists
of a title surrounded in a single pair of square braces, followed by any number
of lines that describe the block. A block is only ended by the beginning of
another block, or by <EOF>. Every line inside a block describes a property of
that block. In the example we previously saw:
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
`[hello]` is a simple block which just shows the text 'hello'. Blocks must
have an `id` property, which is one of the following:
 - [workspace](#workspace)
 - [clock](#clock)
 - [active](#active)
 - [text](#text)
 - [weather](#weather)
 - [scale](#scale)
 - [graph](#graph)
 - [shell](#shell)

Based on the id, blocks may require other properties. click on each id for
more specific information. In addition, all blocks will recognize 3 additional
properties:
 - shell: execute shell script on bar click
 - fontcolor: override bar font color for this block
 - background: override bar background color for this block

###Workspace
The workspace block requires a [workspace format string](#formatting-workspaces).
In addition, this block also recognized the properties
`active:background` and `active:font`, which are used to change the background
and forground colors of the currently active desktop. These two properties each
take a [color code](#extended-color-codes).


###Clock
The clock block requires a [clock format string](#formatting-clock).
In addition, this block also recognizes a `timeout` property, which is
used to change how often this element should refresh in seconds. It defaults
to 60 seconds.

###Active
The active block requires a `source` property. currently the only valid
value for this property is `window_title`. The window_title value will
cause this block to display the title of the currently active X11 window.
In the future there will be more sources for this block.

###Text
The text block requires a `source` property. The value of this property is
displayed in the bar as plain text.

###Weather
The weather block requires the `format` and `source` properties.
The `format` property controls how the weather data is formatted, see
[weather formatting](#formatting-weather). The source property tells tint3
what location to pull weather data from, see
[location formatting](#formatting-location).

###Scale
The scale block requires the `source` property. Currently, the supported values
of the `source` property are `battery [BAT_ID]` and `alsa [CARD] [MIXER]`.
 - availible choices for `BAT_ID` can be found in /sys/class/power_supply
 - availible choices for `CARD` and `MIXER` can be found with the `amixer` utility
   (however these are usually `0` and `Master` respectively)

###Graph
The graph block requires the `source` property in all cases. The supported
values of the `source` property are:
 - `memory`
 - `network [interface] [up/down]`
 - `multi-network [interface]`
For the network graphs, interfaces can be found in `/proc/net/dev`.
The multi-network value also requires additional properties declared:
 - `targets [A] ...`
 - `[A] [up/down] x[number] [color code]`
 - ...
An example configuration is:
````
[network]
  id graph
  source multi-network wlp3s0
  targets upload download
  upload up x400 #ccffcc
  download down x1 #aabbcc
````

###Shell
The shell block requires the `source` property. The value of this property is
passed to the `popen` syscall, so be careful what you put in here; the stdout
from that process is displayed as text in this block.




## Example
Example rc file:
[tint3rc](https://github.com/tmathmeyer/tint3/blob/master/tint3rc)
produces:
![screenshot](https://github.com/tmathmeyer/tint3/blob/master/screenshot.png)
