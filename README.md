tint3
===============
####tint3 is
 - a lightweight bar for the X11 window server
 - (almost) completly X11 compliant
 - easily configurable, and ready for the AUR [configuration.md](https://github.com/tmathmeyer/tint3/blob/master/configuration.md)

####Contributions
tint3 uses feature branches. If you would like to submit a PR, submit it on a feature branch. all external PR's to master will be closed. The braches that exemplify the unix way and don't rely on external libraries may be merged into master. The most important things to remember are:
- make as few syscalls as possible
- avoid dependancies, especially non-common libraries

#### Mirrors
- git.tmathmeyer.com



Using tint3 without mpd (will be moved to feature branch soon)
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
 - [ ] have tint3 listen on a unix socket for runtime changes
 - [ ] make better defaults, as well as pre-package some themes
 - [ ] write a man-page!!!
 - [ ] add support for pianobar (pandora internet radio)
 - [ ] mouse and notification support
 - [ ] api for adding modules
