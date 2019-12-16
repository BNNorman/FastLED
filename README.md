# Readme #
This sketch illustrates how to drive 4 separate WS2812 led strips each with it's own animation.

It uses WiFi to enable NTP to obtain the current hour of the day so that the script can automatically turn on/off the animations.

Turning off the animations is done by setting the led strips to black, which should kill the power consumption.

The sketch is written for a WeMOS D1 R2 Mini using pins D8, D7, D6 and D5.

## NOTE ##

Whilst working on this code I experienced a problem whereby a WeMOS D1 R2 Mini I had previously used whilst experimenting with ESP Easy first refused to connect to my network and second would not connect to an NTP server because it lost the DNS addresses (probably my fault that).

I tried uploading a blank sketch - but didn't get around to using esptool erase_flash - and that made no difference.

Anyway, the final solution was to manually (in code) reconfigure the WiFi (which is remembered by the WeMOS). I can only assume that the previous (retained) config had gotten corrupted somehow.


