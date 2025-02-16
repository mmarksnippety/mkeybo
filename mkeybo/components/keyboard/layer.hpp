#pragma once

#include <pico/stdlib.h>
#include <array>
#include <string>
#include "mkeybo/components/base.hpp"


namespace mkeybo::keyboard {

template <size_t switches_count>
struct KeyboardLayer
{
    std::string name;
    std::array<Keycode, switches_count> keycodes;
};

}