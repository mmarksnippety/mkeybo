#pragma once

#include <array>
#include <sstream>
#include <iostream>
#include "base.hpp"
#include "tusb.h"


namespace mkeybo {

/**
 * Usb reports, data container, description and sending.
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
class UsbReport
{
protected:
    virtual void send_report_() = 0;

public:
    uint8_t report_id;
    UsbReportStatus status = UsbReportStatus::draft;

    explicit UsbReport(const uint8_t index) :
        report_id(index)
    {
    }

    virtual ~UsbReport() = default;

    [[nodiscard]] bool is_report_ready() const { return status == UsbReportStatus::ready; }

    void send_report()
    {
        send_report_();
        status = UsbReportStatus::sent;
    }

    virtual std::vector<uint8_t> get_report_description() = 0;

    [[nodiscard]] virtual std::string to_string() const = 0;
};

/**
 * Standard keyboard report
 *
 * How to make reports:
 * https://wiki.osdev.org/USB_Human_Interface_Devices#USB_keyboard
 */
class UsbKeyboardReport final : public UsbReport
{
    void send_report_() override
    {
        tud_hid_keyboard_report(report_id, modifiers, keycodes.data());
    }

public:
    using UsbReport::status;
    uint8_t modifiers = 0;
    std::array<uint8_t, 6> keycodes{};

    explicit UsbKeyboardReport(const uint8_t index) :
        UsbReport(index)
    {
    }

    std::vector<uint8_t> get_report_description() override
    {
        std::cout << "report_id: " << std::to_string(report_id) << std::endl;
        return {TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(report_id))};
    }

    [[nodiscard]] std::string to_string() const override
    {
        std::ostringstream oss;
        oss << "UsbKeyboardReport {"
            << "report_id: " << static_cast<int>(report_id) << ", "
            << "status: " << static_cast<int>(status) << ", "
            << "modifiers: " << static_cast<int>(modifiers) << ", "
            << "keycodes: [ ";
        for (const auto& key : keycodes)
        {
            oss << std::to_string(key) << " ";
        }
        oss << "] }";
        return oss.str();
    }

};


/**
 * Customer control report.
 * Send 1 key per report
 */
class UsbCcReport final : public UsbReport
{
    void send_report_() override
    {
        tud_hid_report(report_id, &keycode, 2);
    }

public:
    using UsbReport::status;
    uint16_t keycode{};

    explicit UsbCcReport(const uint8_t index) :
        UsbReport(index)
    {
    }

    std::vector<uint8_t> get_report_description() override
    {
        std::cout << "report_id: " << std::to_string(report_id) << std::endl;
        return {TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(report_id))};
    }

    [[nodiscard]] std::string to_string() const override
    {
        std::ostringstream oss;
        oss << "UsbCcReport { "
            << "report_id: " << static_cast<int>(report_id) << ", "
            << "status: " << static_cast<int>(status) << ", "
            << "keycode: " << keycode
            << " }";
        return oss.str();
    }

};


}
