#pragma once

#include <array>
#include "keyboard_state.hpp"
#include "base.hpp"
#include "tusb.h"
#include <ranges>


namespace mkeybo {


/**
 * Abstract base usb reporter
 */
template <size_t switches_count>
class UsbReporter
{
protected:
    KeycodeType keycode_type;

    virtual void send_report_(const UsbReport*) = 0;

public:
    explicit UsbReporter(const KeycodeType keycode_type) :
        keycode_type(keycode_type)
    {
    }

    virtual ~UsbReporter() = default;

    UsbReport* get_report(KeyboardState<switches_count>* keyboard_state)
    {
        auto report = keyboard_state->usb_reports.find(keycode_type);
        if (report != keyboard_state->usb_reports.end())
        {
            return report->second;
        }
        return nullptr;
    }

    auto get_keycode_events(KeyboardState<switches_count>* keyboard_state, KeycodeEventPriority priority)
    {
        return keyboard_state->get_filtered_keycode_events(keycode_type, KeycodeEventType::finalized, priority);
    }

    virtual void generate_empty_record(KeyboardState<switches_count>* keyboard_state) = 0;

    virtual void generate_report(KeyboardState<switches_count>* keyboard_state) = 0;

    void conditionally_generate_report(KeyboardState<switches_count>* keyboard_state)
    {
        if (keyboard_state->have_keycode_events_changed(keycode_type))
        {
            generate_report(keyboard_state);
        }
    }

    void send_report(UsbReport* report)
    {
        send_report_(report);
        report->status = UsbReportStatus::sent;
    }
};

template <size_t switches_count>
class UsbReporterManager
{
    std::vector<UsbReporter<switches_count>*> reporters_;

public:
    explicit UsbReporterManager(const std::vector<UsbReporter<switches_count>*>& reporters) :
        reporters_(reporters)
    {
    }

    ~UsbReporterManager()
    {
        for (auto reporter : reporters_)
        {
            delete reporter;
        }
    }

    void generate_empty_records(KeyboardState<switches_count>* keyboard_state)
    {
        for (auto reporter : reporters_)
        {
            reporter->generate_empty_record(keyboard_state);
        }
    }

    void generate_reports(KeyboardState<switches_count>* keyboard_state)
    {
        for (auto reporter : reporters_)
        {
            reporter->conditionally_generate_report(keyboard_state);
        }
    }

    bool is_any_report_ready(KeyboardState<switches_count>* keyboard_state)
    {
        return std::ranges::any_of(keyboard_state->usb_reports | std::views::values,
                                   [](const auto& report) { return report->status == UsbReportStatus::ready; });
    }

    /**
     * Sends the first USB report marked as ready from the provided keyboard state
     * to one of the reporters managed by the UsbReporterManager.
     *
     * The function iterates through the usb_reports in the given keyboard state and
     * finds the first UsbReport with a status of `UsbReportStatus::ready`. It then
     * sends this report using the corresponding reporter from the reporters list and
     * updates the `last_sent_usb_report_type` in the keyboard state.
     *
     * @param keyboard_state Pointer to the KeyboardState object containing the USB reports
     *                       and the state of the keyboard.
     */
    void send_reports(KeyboardState<switches_count>* keyboard_state)
    {
        if (!tud_hid_ready())
        {
            return;
        }
        auto reporters_it = reporters_.begin();
        auto reporters_end = reporters_.end();
        auto reports_it = keyboard_state->usb_reports.begin();
        auto reports_end = keyboard_state->usb_reports.end();
        while (reports_it != reports_end)
        {
            if (auto report_pair = *reports_it; report_pair.second->status == UsbReportStatus::ready)
            {
                auto reporter = *reporters_it;
                reporter->send_report(report_pair.second);
                break;
            }
            ++reporters_it;
            ++reports_it;
        }
        // Here all reports are sent, or no ready report to send.
    }
};


/**
 * Standard hid key report
 *
 * How to make reports:
 * https://wiki.osdev.org/USB_Human_Interface_Devices#USB_keyboard
 */
template <size_t switches_count>
class UsbHidKeycodeReporter final : public UsbReporter<switches_count>
{
    using UsbReporter<switches_count>::keycode_type;

public:
    explicit UsbHidKeycodeReporter() :
        UsbReporter<switches_count>(KeycodeType::hid)
    {
    }

    void send_report_(const UsbReport* report) override
    {
        const auto hid_key_report = reinterpret_cast<const UsbHidKeycodeReport*>(report);
        tud_hid_keyboard_report(static_cast<uint8_t>(keycode_type), hid_key_report->modifiers,
                                hid_key_report->keycodes.data());
    }

    void generate_empty_record(KeyboardState<switches_count>* keyboard_state) override
    {
        keyboard_state->usb_reports[keycode_type] = new UsbHidKeycodeReport();
    }

    void generate_report(KeyboardState<switches_count>* keyboard_state) override
    {
        const auto report = reinterpret_cast<UsbHidKeycodeReport*>(this->get_report(keyboard_state));
        report->status = UsbReportStatus::draft;
        report->modifiers = 0;
        report->keycodes.fill(0);
        generate_report_modifiers(get_modifiers_keycodes(keyboard_state, KeycodeEventPriority::high), report);
        generate_report_modifiers(get_modifiers_keycodes(keyboard_state, KeycodeEventPriority::normal), report);
        auto index = generate_report_keycodes(get_regular_keycodes(keyboard_state, KeycodeEventPriority::high), report);
        generate_report_keycodes(get_regular_keycodes(keyboard_state, KeycodeEventPriority::normal), report, index);
        report->status = UsbReportStatus::ready;
    }

    /**
     * Modifier are coded as bits.
     * Luckily all modifiers keycodes are in the same order as the bits in the report
     *
     * Modifier name          | code   | bit index
     * -----------------------+--------+----------
     * HID_KEY_CONTROL_LEFT   |  0xE0  | 0
     * HID_KEY_SHIFT_LEFT     |  0xE1  | 1
     * HID_KEY_ALT_LEFT       |  0xE2  | 2
     * HID_KEY_GUI_LEFT       |  0xE3  | 3
     * HID_KEY_CONTROL_RIGHT  |  0xE4  | 4
     * HID_KEY_SHIFT_RIGHT    |  0xE5  | 5
     * HID_KEY_ALT_RIGHT      |  0xE6  | 6
     * HID_KEY_GUI_RIGHT      |  0xE7  | 7
     */
    template <typename R>
    void generate_report_modifiers(R&& modifiers_keycodes, UsbHidKeycodeReport* report)
    {
        for (auto& keycode_event : modifiers_keycodes)
        {
            // This special case taken from QMK. This coed is mixed modifiers bits and standard keycode
            // first 8bit is a code, higher 8 bits is a modifier bits (no key code of ex shift key)
            if (keycode_event.keycode.code >= std::numeric_limits<uint8_t>::max())
            {
                const auto modifier_bits = static_cast<uint8_t>(keycode_event.keycode.code >> 8);
                report->modifiers |= modifier_bits;
            }
            else
            {
                auto bit_index = keycode_event.keycode.code - HID_KEY_CONTROL_LEFT;
                report->modifiers |= 1 << bit_index;
            }
        }
    }

    template <typename R>
    uint8_t generate_report_keycodes(R&& regular_keycodes, UsbHidKeycodeReport* report,
                                     const uint8_t start_keycode_index = 0)
    {
        uint8_t keycode_index = start_keycode_index;
        for (auto& keycode_event : regular_keycodes)
        {
            // more than 6 keys, go to phantom mode
            if (keycode_index > report->keycodes.size())
            {
                report->keycodes.fill(1);
                break;
            }
            report->keycodes[keycode_index] = static_cast<uint8_t>(keycode_event.keycode.code);
            keycode_index++;
        }
        return keycode_index;
    }

    auto get_regular_keycodes(KeyboardState<switches_count>* keyboard_state, KeycodeEventPriority priority)
    {
        return this->get_keycode_events(keyboard_state, priority) |
            std::views::filter([](const auto& keycode_event)
            {
                return static_cast<uint8_t>(keycode_event.keycode.code) < HID_KEY_CONTROL_LEFT;
            });
    }

    auto get_modifiers_keycodes(KeyboardState<switches_count>* keyboard_state, KeycodeEventPriority priority)
    {
        return this->get_keycode_events(keyboard_state, priority) |
            std::views::filter([](const auto& keycode_event)
            {
                return keycode_event.keycode.code >= HID_KEY_CONTROL_LEFT;
            });
    }
};


/**
 * Customer control reports
 * Send 1 key per report
 */
template <size_t switches_count>
class UsbCcKeycodeReporter final : public UsbReporter<switches_count>
{
    using UsbReporter<switches_count>::keycode_type;

public:
    explicit UsbCcKeycodeReporter() :
        UsbReporter<switches_count>(KeycodeType::cc)
    {
    }

    void send_report_(const UsbReport* report) override
    {
        const auto cc_report = reinterpret_cast<const UsbCcKeycodeReport*>(report);
        tud_hid_report(static_cast<uint8_t>(keycode_type), &cc_report->keycode, 2);
    }

    void generate_empty_record(KeyboardState<switches_count>* keyboard_state) override
    {
        keyboard_state->usb_reports[keycode_type] = new UsbCcKeycodeReport();
    }

    void generate_report(KeyboardState<switches_count>* keyboard_state) override
    {
        const auto report = reinterpret_cast<UsbCcKeycodeReport*>(this->get_report(keyboard_state));
        report->status = UsbReportStatus::draft;
        report->keycode = 0;
        generate_report_(this->get_keycode_events(keyboard_state, KeycodeEventPriority::high), report);
        if (report->keycode == 0)
        {
            generate_report_(this->get_keycode_events(keyboard_state, KeycodeEventPriority::normal), report);
        }
        report->status = UsbReportStatus::ready;
    }

    template <typename R>
    void generate_report_(R&& keycodes_views, UsbCcKeycodeReport* report)
    {
        auto keycode_event_it = keycodes_views.begin();
        if (keycode_event_it != keycodes_views.end())
        {
            report->keycode = (*keycode_event_it).keycode.code;
        }
    }
};


} // namespace mkeybo
