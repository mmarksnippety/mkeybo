#pragma once

#include "mkeybo/components/keyboard/keyboard_settings.hpp"
#include "mkeybo/components/hid_controller.hpp"
#include "config.hpp"
#include "keyboard.hpp"
#include "status_display.hpp"


Keyboard<keyboard_config.switches_count>* create_keyboard();

StatusDisplay<keyboard_config.switches_count>*
create_status_display(Keyboard<keyboard_config.switches_count>* keyboard);

mkeybo::HidController*
create_hid_controller(Keyboard<keyboard_config.switches_count>* keyboard,
                      StatusDisplay<keyboard_config.switches_count>* status_display);
