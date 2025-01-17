#pragma once

#include <array>
#include "keyboard_state.hpp"
#include "base.hpp"
#include "tusb.h"
#include <ranges>


namespace mkeybo {


/**
 * Usb reports
 */

enum class UsbReportStatus : uint8_t
{
    draft,
    ready,
    sent,
};

/**
 * Abstract base usb reporter
 */
template <size_t switches_count>
class UsbReport
{
protected:
    KeycodeType keycode_type_;
    UsbReportStatus status_ = UsbReportStatus::draft;

    virtual void send_report_() = 0;

public:
    explicit UsbReport(const KeycodeType keycode_type) :
        keycode_type_(keycode_type)
    {
    }

    virtual ~UsbReport() = default;

    auto get_keycode_events(KeyboardState<switches_count>* keyboard_state, KeycodeEventPriority priority)
    {
        return keyboard_state->get_filtered_keycode_events(keycode_type_, KeycodeEventType::finalized, priority);
    }

    virtual void generate_report(KeyboardState<switches_count>* keyboard_state) = 0;

    [[nodiscard]] bool is_report_ready() const { return status_ == UsbReportStatus::ready; }

    void conditionally_generate_report(KeyboardState<switches_count>* keyboard_state)
    {
        if (keyboard_state->have_keycode_events_changed(keycode_type_))
        {
            generate_report(keyboard_state);
        }
    }

    void send_report()
    {
        send_report_();
        status_ = UsbReportStatus::sent;
    }
};

template <size_t switches_count>
class UsbReportManager
{
    std::vector<UsbReport<switches_count>*> reports_;
    size_t report_index_ = 0;

public:
    explicit UsbReportManager(const std::vector<UsbReport<switches_count>*>& reporters) :
        reports_(reporters)
    {
    }

    ~UsbReportManager()
    {
        for (auto reporter : reports_)
        {
            delete reporter;
        }
    }

    void generate_reports(KeyboardState<switches_count>* keyboard_state)
    {
        for (auto reporter : reports_)
        {
            reporter->conditionally_generate_report(keyboard_state);
        }
    }

    bool is_any_report_ready()
    {
        return std::ranges::any_of(reports_, [](const auto& reporter) { return reporter->is_report_ready(); });
    }

    /**
     * Sends the first USB report marked as ready from the provided keyboard state
     * to one of the reporters managed by the UsbReporterManager.
     *
     * The function iterates through the usb_reports in the given keyboard state and
     * finds the first UsbReport with a status of `UsbReportStatus::ready`. It then
     * sends this report using the corresponding reporter from the reporters list and
     * updates the `last_sent_usb_report_type` in the keyboard state.
     */
    void send_reports(const bool first_report=false)
    {
        if (!tud_hid_ready())
        {
            return;
        }
        if (first_report)
        {
            report_index_ = 0;
        }
        while (report_index_ < reports_.size())
        {
            if (reports_[report_index_]->is_report_ready())
            {
                reports_[report_index_]->send_report();
                report_index_++;
                break;
            }
            report_index_++;
        }
    }
};


/**
 * Standard hid key report
 *
 * How to make reports:
 * https://wiki.osdev.org/USB_Human_Interface_Devices#USB_keyboard
 */
template <size_t switches_count>
class UsbHidKeycodeReport final : public UsbReport<switches_count>
{
    using UsbReport<switches_count>::keycode_type_;
    using UsbReport<switches_count>::status_;
    uint8_t report_modifiers_ = 0;
    std::array<uint8_t, 6> report_keycodes_{};

public:
    explicit UsbHidKeycodeReport() :
        UsbReport<switches_count>(KeycodeType::hid)
    {
    }

    void send_report_() override
    {
        tud_hid_keyboard_report(static_cast<uint8_t>(keycode_type_), report_modifiers_, report_keycodes_.data());
    }

    void generate_report(KeyboardState<switches_count>* keyboard_state) override
    {
        status_ = UsbReportStatus::draft;
        report_modifiers_ = 0;
        report_keycodes_.fill(0);
        generate_report_modifiers(get_modifiers_keycodes(keyboard_state, KeycodeEventPriority::high));
        generate_report_modifiers(get_modifiers_keycodes(keyboard_state, KeycodeEventPriority::normal));
        auto index = generate_report_keycodes(get_regular_keycodes(keyboard_state, KeycodeEventPriority::high));
        generate_report_keycodes(get_regular_keycodes(keyboard_state, KeycodeEventPriority::normal), index);
        status_ = UsbReportStatus::ready;
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
    void generate_report_modifiers(R&& modifiers_keycodes)
    {
        for (auto& keycode_event : modifiers_keycodes)
        {
            // This special case taken from QMK. This coed is mixed modifiers bits and standard keycode
            // first 8bit is a code, higher 8 bits is a modifier bits (no key code of ex shift key)
            if (keycode_event.keycode.code >= std::numeric_limits<uint8_t>::max())
            {
                const auto modifier_bits = static_cast<uint8_t>(keycode_event.keycode.code >> 8);
                report_modifiers_ |= modifier_bits;
            }
            else
            {
                auto bit_index = keycode_event.keycode.code - HID_KEY_CONTROL_LEFT;
                report_modifiers_ |= 1 << bit_index;
            }
        }
    }

    template <typename R>
    uint8_t generate_report_keycodes(R&& regular_keycodes, const uint8_t start_keycode_index = 0)
    {
        uint8_t keycode_index = start_keycode_index;
        for (auto& keycode_event : regular_keycodes)
        {
            // more than 6 keys, go to phantom mode
            if (keycode_index > report_keycodes_.size())
            {
                report_keycodes_.fill(1);
                break;
            }
            report_keycodes_[keycode_index] = static_cast<uint8_t>(keycode_event.keycode.code);
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
class UsbCcKeycodeReport final : public UsbReport<switches_count>
{
    using UsbReport<switches_count>::keycode_type_;
    using UsbReport<switches_count>::status_;
    uint16_t keycode_{};

public:
    explicit UsbCcKeycodeReport() :
        UsbReport<switches_count>(KeycodeType::cc)
    {
    }

    void send_report_() override
    {
        tud_hid_report(static_cast<uint8_t>(keycode_type_), &keycode_, 2);
    }

    void generate_report(KeyboardState<switches_count>* keyboard_state) override
    {
        status_ = UsbReportStatus::draft;
        keycode_ = 0;
        generate_report_(this->get_keycode_events(keyboard_state, KeycodeEventPriority::high));
        if (keycode_ == 0)
        {
            generate_report_(this->get_keycode_events(keyboard_state, KeycodeEventPriority::normal));
        }
        status_ = UsbReportStatus::ready;
    }

    template <typename R>
    void generate_report_(R&& keycodes_views)
    {
        auto keycode_event_it = keycodes_views.begin();
        if (keycode_event_it != keycodes_views.end())
        {
            keycode_ = (*keycode_event_it).keycode.code;
        }
    }
};


}
