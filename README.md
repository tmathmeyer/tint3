tint3
===============
####tint3 is
 - a bar for the X11 window server
 - lightweight
 - always up to date, with new features being added continuously
 - configured with header files (it's a C project)
 - NOT BASED ON tint2 (just a similar name)
 - free software
 - open to contribution

####Contributions
tint3 uses feature branches. If you would like to submit a PR, submit it on a feature branch. all external PR's to master will be closed.

####Screenshots
![Clock](screenshots/clock.png)

![Desktops](screenshots/desktops.png)

![Network](screenshots/netgraphs.png)

![Weather](screenshots/thermometer.png)

#### Mirrors
- git.tmathmeyer.com



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




##TODO:
 - [ ] theme switcher (hopefully with an api!)
 - [ ] clean up defaults.h
 - [ ] add clock format string
 - [ ] add support for pianobar (pandora internet radio)
 - [ ] mouse and notification support
 - [ ] api for adding modules
