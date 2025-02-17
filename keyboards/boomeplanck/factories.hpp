#pragma once

#include "mkeybo/components/hid_controller.hpp"
#include "config.hpp"
#include "keyboard.hpp"
#include "status_leds.hpp"


Keyboard<keyboard_config.switches_count>* create_keyboard();
StatusLeds<keyboard_config.switches_count>* create_status_leds(Keyboard<keyboard_config.switches_count>* keyboard);
mkeybo::HidController* create_hid_controller(Keyboard<keyboard_config.switches_count>* keyboard,
                                             StatusLeds<keyboard_config.switches_count>* status_leds);
