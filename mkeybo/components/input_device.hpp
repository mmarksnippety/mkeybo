#pragma once

#include <vector>

#include "usb_reports.hpp"


namespace mkeybo {

class InputDevice
{
public:
    virtual ~InputDevice() = default;
    virtual void update_state() = 0;
    virtual uint8_t setup_usb_reports(std::vector<UsbReport*>& reports, uint8_t start_index) = 0;
    virtual void update_usb_reports(std::vector<UsbReport*>& reports) = 0;

    virtual void on_usb_mount()
    {
    }

    virtual void on_usb_umount()
    {
    }

    virtual void on_usb_suspend()
    {
    }

    virtual void on_usb_resume()
    {
    }

    virtual void on_usb_report_receive(
        uint8_t const instance, const uint8_t report_id, const hid_report_type_t report_type,
        uint8_t const* buffer, const uint16_t bufsize)
    {
    }
};

}
