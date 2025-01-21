#pragma once

#include <vector>

#include "usb_reports.hpp"


namespace mkeybo {

class InputDevice
{
public:
    virtual ~InputDevice() = default;
    virtual void update() = 0;
    virtual void generate_usb_reports(const std::vector<UsbReport*>& reports) = 0;
};

}
