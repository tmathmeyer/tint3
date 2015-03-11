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


##TODO:
 - [ ] have tint3 listen on a unix socket for runtime changes
 - [ ] make better defaults, as well as pre-package some themes
 - [ ] write a man-page!!!
 - [ ] add support for pianobar (pandora internet radio)
 - [ ] mouse and notification support
 - [ ] abi for adding modules
