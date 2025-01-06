#pragma once

#include "mkeybo/components/KeyboardSettings.hpp"


template <size_t switches_count>
auto create_keyboard_settings() -> mkeybo::KeyboardSettings<switches_count>*;
