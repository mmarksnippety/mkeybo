#pragma once

#include "mkeybo/components/hid_controller.hpp"
#include "config.hpp"
#include "keyboard.hpp"


Keyboard<keyboard_config.switches_count>* create_keyboard();
mkeybo::HidController* create_hid_controller(Keyboard<keyboard_config.switches_count>* keyboard);
