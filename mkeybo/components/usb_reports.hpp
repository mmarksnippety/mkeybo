#pragma once

#include <array>
#include "base.hpp"
#include "tusb.h"


namespace mkeybo {

/**
 * Usb reports.
 * That components are only DTO objects with send method
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
    KeycodeType keycode_type;
    UsbReportStatus status = UsbReportStatus::draft;

    explicit UsbReport(const KeycodeType keycode_type) :
        keycode_type(keycode_type)
    {
    }

    virtual ~UsbReport() = default;

    [[nodiscard]] bool is_report_ready() const { return status == UsbReportStatus::ready; }

    void send_report()
    {
        send_report_();
        status = UsbReportStatus::sent;
    }

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
        tud_hid_keyboard_report(static_cast<uint8_t>(keycode_type), modifiers, keycodes.data());
    }

public:
    using UsbReport::keycode_type;
    using UsbReport::status;
    uint8_t modifiers = 0;
    std::array<uint8_t, 6> keycodes{};

    explicit UsbKeyboardReport() :
        UsbReport(KeycodeType::hid)
    {
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
        tud_hid_report(static_cast<uint8_t>(keycode_type), &keycode, 2);
    }

public:
    using UsbReport::keycode_type;
    using UsbReport::status;
    uint16_t keycode{};

    explicit UsbCcReport() :
        UsbReport(KeycodeType::cc)
    {
    }
};


}
