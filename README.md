tint3
===============
####tint3 is not a fork of tint2.
####tint3 does not use any code from tint2.
####tint3 is not (for now) backwards compatible with tint2.
####tint2 appears to be a dead project, has an unmaintainable codebase, and has strayed from it's original goal of being lightweight.

######*much of the code wrapping X is pulled from the suckless tools, namely dmenu
######*like many of the suckless tools, tint3 is configured with header files. I eventually hope to write a parser from tint2rc to tint3/config.h.

######**tint3 uses feature branches. If you would like to submit a PR, submit it on a feature branch. all external PR's to master will be closed.

if you use bspwm, and use my default commands for getting desktop information, you can't start tint3 from inside tmux. It works fine with dmenu however

##Screenshots (and screencasts*)
![Clock](screenshots/clock.png)

![Desktops](screenshots/desktops.png)

![Network](screenshots/netgraphs.png)

![Weather](screenshots/thermometer.png)







*maybe github will support these one day



Using tint3 without mpd
=======================

tint3 now supports mpd. It depends on libmpdclient by default. If you want to
build tint3 without support for mpd, open the file src/config.mk and find these
three lines:

    MPDLIBS = -lmpdclient
    MPDDEPS = mpd.o
    MPDFLAGS= -D_WITH_MPD

Comment out these lines by inserting a '#' character at the beginning of each
line. When you're done, it will look like this:

    # MPDLIBS = -lmpdclient
    # MPDDEPS = mpd.o
    # MPDFLAGS= -D_WITH_MPD


#### see the official mirror at git.tmathmeyer.com

##TODO:
 - [ ] theme switcher (hopefully with an api!)
 - [ ] clean up defaults.h
 - [x] remove the call to xtitle
 - [ ] add clock format string
 - [ ] add support for pianobar (pandora internet radio)
