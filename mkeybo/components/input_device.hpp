#pragma once

#include "input_device_settings.hpp"
#include "mkeybo/utils.hpp"


namespace mkeybo {


class HidController;


class InputDevice
{
protected:
    uint16_t update_state_interval_ms{5};
    uint32_t last_update_state_ts{};

public:
    InputDevice() = default;
    virtual ~InputDevice() = default;

    virtual void apply_settings(const std::unique_ptr<InputDeviceSettings>& settings)
    {
        update_state_interval_ms = settings->update_state_interval_ms;
    }

    /**
     * Update state if it is time. Return true if update_state is called
     */
    bool update_state_async()
    {
        if (const auto current_ts = get_ms_since_boot();
            current_ts - last_update_state_ts >= update_state_interval_ms)
        {
            update_state();
            last_update_state_ts = current_ts;
            return true;
        }
        return false;
    }

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
