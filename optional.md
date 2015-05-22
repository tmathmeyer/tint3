#Optional Configuration

## Weather Details
by using the option "details" in tint3rc (see configuration.md), the Weather Detail popup window can be enabled. 
This feature is not complete, and should only be enabled for testing and development. This feature adds one new
property with three values to the weather block in the configuration. The property is named "details-location",
and can take any of the three values: {mouse, centre, aligned}. "mouse" causes the top left corner of the popup
window to be horizontally aligned with the location of the mouse when the first button releases. "centre" causes
the popup to appear exactly in the middle of the screen (though this does not work yet). "aligned" causes the
popup to be aligned with the weather baritem - that is that the center of the popup window is horizontally
aligned with the center of the weather text.
