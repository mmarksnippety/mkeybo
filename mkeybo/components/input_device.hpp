#pragma once

namespace mkeybo {


class HidController;


class InputDevice
{
public:
    virtual ~InputDevice() = default;
    virtual void update_state() = 0;
    virtual void setup_usb_reports(HidController* hid_controller) = 0;
    virtual void update_usb_reports(HidController* hid_controller) = 0;
    virtual void update_actions(HidController* hid_controller)
    {
    }

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
