#pragma once

#include <pico/stdlib.h>

namespace mkeybo {

class HidControllerSettings
{
public:
    uint16_t usb_task_interval_ms{5};

    explicit HidControllerSettings(const uint16_t usb_task_interval_ms) :
        usb_task_interval_ms(usb_task_interval_ms)
    {
    }

    virtual ~HidControllerSettings() = default;
};

}
