#pragma once

#include "mkeybo/components/keyboard_settings.hpp"


template <size_t switches_count>
auto create_keyboard_settings() -> mkeybo::KeyboardSettings<switches_count>*;
