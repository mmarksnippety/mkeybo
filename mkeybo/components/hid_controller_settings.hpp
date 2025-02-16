#pragma once

#include <pico/stdlib.h>

namespace mkeybo {

class HidControllerSettings
{
public:
    uint16_t main_task_interval_ms{5};
    uint16_t usb_task_interval_ms{5};

    explicit HidControllerSettings(const uint16_t main_task_interval_ms, const uint16_t usb_task_interval_ms) :
        main_task_interval_ms(main_task_interval_ms), usb_task_interval_ms(usb_task_interval_ms)
    {
    }

    ~HidControllerSettings()
    {
        std::cout << "HID controller settings destructor" << std::endl;
    }

};

}
