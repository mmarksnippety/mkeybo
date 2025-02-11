#pragma once


#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <array>
#include <map>
#include <ranges>
#include "pico/unique_id.h"
#include "usb_reports.hpp"
#include "input_device.hpp"
#include "actions.hpp"


namespace mkeybo {


class HidController
{
protected:
    // usb device properties
    std::string device_name{};
    std::string manufactured_name{};
    std::string unique_id{};
    // usb reports
    uint8_t current_usb_report_index{};
    std::vector<UsbReport*> usb_reports{};
    std::vector<uint8_t> usb_reports_description{};
    std::vector<uint8_t> usb_configuration_description{};
    // actions
    std::map<uint16_t, actions::ActionExecutor*> action_executors;
    std::array<uint16_t, 10> action_buffer{};
    uint8_t action_buffer_index = 0;
    // input devices
    std::vector<InputDevice*> input_devices{};

public:
    HidController(const std::string_view device_name, const std::string_view manufactured_name,
                  const std::map<uint16_t, actions::ActionExecutor*>& action_executors,
                  const std::vector<InputDevice*>& input_devices) :
        device_name(device_name),
        manufactured_name(manufactured_name),
        action_executors(action_executors),
        input_devices(input_devices)
    {
        setup_usb_reports();
        setup_usb_configuration_description();
        reset_actions();
    }

    virtual ~HidController()
    {
        for (const auto& usb_report : usb_reports)
        {
            delete usb_report;
        }
        for (const auto& action_executors : action_executors | std::views::values)
        {
            delete action_executors;
        }
        for (const auto& input_device : input_devices)
        {
            delete input_device;
        }
    }

    /**
     * Lifecycle
     */

    /**
     * Executes the main task of the HID controller.
     *
     * This method is responsible for managing the overall flow of the HID
     * controller's operations. It sequentially:
     * - Updates the state of input devices.
     * - Updates USB reports based on the state of the input devices.
     * - Updates actions based on processed input and current state.
     * - Executes the actions using the action manager.
     */
    void main_task()
    {
        update_state();
        update_usb_reports();
        update_actions();
        execute_actions();
    }

    void usb_task()
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
     * Usb configuration
     */

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

    auto& get_usb_reports()
    {
        return usb_reports;
    }

    uint8_t add_usb_report(UsbReport* usb_report)
    {
        usb_reports.push_back(usb_report);
        usb_report->set_report_id(usb_reports.size());
        return usb_reports.size();
    }

    [[nodiscard]] auto* get_usb_report(uint8_t report_id) const
    {
        return usb_reports[report_id - 1];
    }

    /**
     * Setup usb reports descriptor
     * This method get all inputs reports togather.
     */
    void setup_usb_reports()
    {
        // generate all reports, report_id start from 1 (but index in vector from 0)
        for (const auto& input_device : input_devices)
        {
            input_device->setup_usb_reports(this);
        }
        // consolidate reports description
        for (const auto& usb_report : usb_reports)
        {
            auto usb_report_description = usb_report->get_report_description();
            usb_reports_description.insert(usb_reports_description.end(),
                                           usb_report_description.begin(),
                                           usb_report_description.end());
        }
    }

    /**
     * Setup usb configuration description.
     * This method is depended on setup_usb_reports.
     */
    void setup_usb_configuration_description()
    {
        constexpr uint8_t ITF_NUM_HID{0};
        constexpr uint8_t ITF_NUM_TOTAL{1};
        constexpr uint8_t CONFIG_TOTAL_LEN{TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN};
        constexpr uint8_t EPNUM_HID{0x81};

        usb_configuration_description = {
            // Config number, interface count, string index, total length, attribute, power in mA
            TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

            // Interface number, string index, protocol, report descriptor len,
            // EP In address, size & polling interval
            TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, usb_reports_description.size(),
                               EPNUM_HID, CFG_TUD_HID_EP_BUFSIZE, 5)};
    }

    auto& get_usb_reports_description()
    {
        return usb_reports_description;
    }

    auto& get_usb_configuration_description()
    {
        return usb_configuration_description;
    }

    [[nodiscard]] bool is_any_usb_report_ready()
    {
        return std::ranges::any_of(usb_reports, [](const auto& report) { return report->is_report_ready(); });
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
            current_usb_report_index = 0;
        }
        while (current_usb_report_index < usb_reports.size())
        {
            if (usb_reports[current_usb_report_index]->is_report_ready())
            {
                usb_reports[current_usb_report_index]->send_report();
                current_usb_report_index++;
                break;
            }
            current_usb_report_index++;
        }
    }

    /**
     * Input devices
     */

    void update_state() const
    {
        for (const auto& input_device : input_devices)
        {
            input_device->update_state();
        }
    }

    void update_usb_reports()
    {
        for (const auto& input_device : input_devices)
        {
            input_device->update_usb_reports(this);
        }
    }

    void update_actions()
    {
        reset_actions();
        for (const auto& input_devices : input_devices)
        {
            input_devices->update_actions(this);
        }
    }

    /**
     * Actions
     */

    void reset_actions()
    {
        action_buffer_index = 0;
        action_buffer.fill(0);
    }

    bool push_action(const uint16_t action_id)
    {
        if (action_buffer_index >= action_buffer.size())
        {
            return false;
        }
        action_buffer[action_buffer_index] = action_id;
        action_buffer_index++;
        return true;
    }

    void execute_actions()
    {
        for (const auto& action_id : action_buffer)
        {
            if (action_id != 0)
            {
                if (auto executor_it = action_executors.find(action_id); executor_it != action_executors.end())
                {
                    executor_it->second->execute();
                }
            }
        }
    }

    /**
     * TinyUsb callbacks
     */

    /**
     * that callbacks are called by TinyUSB, see mkeybo/defaults/usb_callbacks.cpp
     */

    virtual void on_usb_mount()
    {
        std::cout << "USB mounted" << std::endl;
        for (const auto& input_device : input_devices)
        {
            input_device->on_usb_mount();
        }
    }

    virtual void on_usb_umount()
    {
        std::cout << "USB unmounted" << std::endl;
        for (const auto& input_device : input_devices)
        {
            input_device->on_usb_umount();
        }
    }

    virtual void on_usb_suspend()
    {
        std::cout << "USB suspended" << std::endl;
        for (const auto& input_device : input_devices)
        {
            input_device->on_usb_suspend();
        }
    }

    virtual void on_usb_resume()
    {
        std::cout << "USB resumed" << std::endl;
        for (const auto& input_device : input_devices)
        {
            input_device->on_usb_resume();
        }
    }

    virtual void on_usb_report_receive(
        uint8_t const instance, const uint8_t report_id, const hid_report_type_t report_type,
        uint8_t const* buffer, const uint16_t bufsize)
    {
        std::cout << "USB report received" << std::endl;
        for (const auto& input_device : input_devices)
        {
            input_device->on_usb_report_receive(instance, report_id, report_type, buffer, bufsize);
        }
    }

};

}
