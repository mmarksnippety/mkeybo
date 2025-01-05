# mkeybo

Keyboard Firmware for PI2040 in C++20

Copyright 2023-2025 Marcin Markiewicz, marcin.kivrin@gmail.com

Project home page https://github.com/mmarksnippety/mkeybo

Legacy version of this firmware https://github.com/mmarksnippety/legacy

Jet another keyboard firmware. Why I developed another keyboard firmware? 
I want to make modern code in  C++20. 
This project is design to compile on RP2040. I don't test it on RP2350, but it should work too.

## New version

After more than a year of living with the project, and using two keyboard with this firmware,
I decide to rewire code. Some parts of code are near the same, but generaly shape of components 
and relations between them has been severely altered

## Draft version
This code is totally draft at this moment (begin of 2025)

## How this firmware works (Draft)

Code is divided for several components:

**SwitchReader** - component responsible for read hardware state of switch

**SwitchMapper** - responsible for mapping simple switch pressed to some events (not yet key codes).
This is place where we implement simple press, tap dance, hold

**KeyMapper** - this component map switch number to key codes, using layout (like qwerty, dworak)

**HidRaportSender** - this component send USB Hid report to host.

There is more component like settings (key mapping config), 
config (some hardware dependent setting, that are const on compiling time). 
