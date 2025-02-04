#include "mkeybo/components/hid_controller.hpp"

/**
 * Implementation of callback defined in /external/pico-sdk/lib/tinyusb/src/device/usbd.h
 * This set of callbacks are from keyboard (or other HID device) lifcycle
 */

extern mkeybo::HidController* hid_controller;

void tud_mount_cb()
{
    hid_controller->on_usb_mount();
}

void tud_umount_cb()
{
    hid_controller->on_usb_umount();
}

void tud_suspend_cb(const bool remote_wakeup_en)
{
    hid_controller->on_usb_suspend();
}

void tud_resume_cb()
{
    hid_controller->on_usb_resume();
}

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
    hid_controller->send_usb_report();
}

uint16_t tud_hid_get_report_cb(
    uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer,
    uint16_t reqlen)
{
    // TODO not Implemented
    // (void)instance;
    // (void)report_id;
    // (void)report_type;
    // (void)buffer;
    // (void)reqlen;
    return 0;
}

void tud_hid_set_report_cb(
    uint8_t const instance, const uint8_t report_id, const hid_report_type_t report_type,
    uint8_t const* buffer, const uint16_t bufsize)
{
    if (bufsize < 1 or report_type != HID_REPORT_TYPE_INPUT)
    {
        return;
    }
    hid_controller->on_usb_report_receive(instance, report_id, report_type, buffer, bufsize);
}
