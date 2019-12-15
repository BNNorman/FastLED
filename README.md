# Readme #
This sketch illustrates how to drive 4 separate WS2812 led strips each with it's own animation.

It uses WiFi to enable NTP to obtain the current hour of the day so that the script can automatically turn on/off the animations.

Turning off the animations is done by setting the led strips to black, which should kill the power consumption.

The sketch is written for a WeMOS D1 R2 Mini using pins D8, D7, D6 and D5.
