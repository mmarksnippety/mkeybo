#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include "usb_reports.hpp"
#include "input_device.hpp"
#include "pico/unique_id.h"

namespace mkeybo {


class HidController
{
protected:
    std::string device_name{};
    std::string manufactured_name{};
    std::string unique_id{};
    std::vector<UsbReport*> usb_reports{};
    size_t last_usb_report_sent_index{};
    std::vector<InputDevice*> input_devices{};

public:
    HidController(const std::string_view device_name, const std::string_view manufactured_name,
                  const std::vector<UsbReport*>& usb_reports, const std::vector<InputDevice*>& input_devices) :
        device_name(device_name), manufactured_name(manufactured_name),
        usb_reports(usb_reports), input_devices(input_devices)
    {
    }

    virtual ~HidController()
    {
        for (const auto& usb_report : usb_reports)
        {
            delete usb_report;
        }
        for (const auto& input_device : input_devices)
        {
            delete input_device;
        }
    }

    std::string_view get_device_name()
    {
        return device_name;
    }

    std::string_view get_manufactured_name()
    {
        return manufactured_name;
    }

    std::string_view get_unique_id()
    {
        if (unique_id.empty())
        {
            constexpr int pico_id_len = 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1;
            char pico_id[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
            pico_get_unique_board_id_string(pico_id, pico_id_len);
            unique_id = pico_id;
        }
        return unique_id;
    }

    /**
     * Usb reports
     */

    [[nodiscard]] auto &get_usb_report() const
    {
        return usb_reports;
    }

    bool is_any_usb_report_ready()
    {
        return std::ranges::any_of(usb_reports, [](const auto& report) { return report->is_report_ready(); });
    }


    void hid_task()
    {
        if (is_any_usb_report_ready())
        {
            if (tud_suspended())
            {
                tud_remote_wakeup();
            }
            else
            {
                // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
                send_usb_report(true);
            }
        }
    }

    /**
     * Sends the first USB report marked as ready from the provided keyboard state
     * to one of the reporters managed by the UsbReporterManager.
     *
     * The function iterates through the usb_reports in the given keyboard state and
     * finds the first UsbReport with a status of `UsbReportStatus::ready`. It then
     * sends this report using the corresponding reporter from the reporters list and
     * updates the `last_sent_usb_report_type`.
     */
    void send_usb_report(const bool reset_index = false)
    {
        if (!tud_hid_ready())
        {
            return;
        }
        if (reset_index)
        {
            last_usb_report_sent_index = 0;
        }
        while (last_usb_report_sent_index < usb_reports.size())
        {
            if (usb_reports[last_usb_report_sent_index]->is_report_ready())
            {
                usb_reports[last_usb_report_sent_index]->send_report();
                last_usb_report_sent_index++;
                break;
            }
            last_usb_report_sent_index++;
        }
    }

    /**
     * Input devices
     */

    void update_input_devices() const
    {
        for (const auto& input_device : input_devices)
        {
            input_device->update();
        }
    }

    void generate_usb_reports() const
    {
        for (const auto& input_device : input_devices)
        {
            input_device->generate_usb_reports(get_usb_report());
        }
    }

    /**
     * TinyUsb callbacks
     */

    /**
     * that callbacks are called by TinyUSB, see mkeybo/defaults/usb_callbacks.cpp
     */

    void on_usb_mount()
    {
        std::cout << "USB mounted" << std::endl;
    }

    void on_usb_umount()
    {
        std::cout << "USB unmounted" << std::endl;
    }

    void on_usb_suspend()
    {
        std::cout << "USB suspended" << std::endl;
    }

    void on_usb_resume()
    {
        std::cout << "USB resumed" << std::endl;
    }

};

}
