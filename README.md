tint3
===============
####tint3 is
 - a lightweight bar for the X11 window server
 - (almost) completely X11 compliant
 - easily configurable, and ready for the AUR [configuration.md](https://github.com/tmathmeyer/tint3/blob/master/configuration.md)

####Contributions
Contributions are welcome! If you would like to see a feature implemented, go for it, and there is a good chance it will get merged. The only things to remember are no minimize calls to system(), fopen(), and popen(). Sometimes they are inevitable, but we prefer to keep them out. Also, Don't try to include massive libraries (Cairo, for example).


##TODO:
 - [ ] use pulseaudio libraries instead of alsa
 - [ ] clean up xlib calls
 - [ ] abi for adding modules
