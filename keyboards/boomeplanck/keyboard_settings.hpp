#pragma once

#include "config.hpp"
#include "mkeybo/components/keyboard/keyboard_settings.hpp"


std::unique_ptr<mkeybo::keyboard::KeyboardSettings<keyboard_config.switches_count>> create_keyboard_settings();
