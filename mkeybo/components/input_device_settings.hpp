#pragma once

#include <pico/stdlib.h>


namespace mkeybo{

class InputDeviceSettings {
public:
    uint16_t update_state_interval_ms{50};

    explicit InputDeviceSettings(const uint16_t update_state_interval_ms) :
        update_state_interval_ms(update_state_interval_ms)
    {
    }

};


}