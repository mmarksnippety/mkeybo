#include "config.hpp"
#include "mkeybo/components/keyboard.hpp"

/**
 * Implementation of callback defined in /external/pico-sdk/lib/tinyusb/src/device/usbd.h
 * This set of callbacks are from keyboard (or other HID device) lifcycle
 */


extern mkeybo::Keyboard<keyboard_config.switches_count>* keyboard;


void tud_mount_cb() { keyboard->on_usb_mount(); }

void tud_umount_cb() { keyboard->on_usb_umount(); }

void tud_suspend_cb(const bool remote_wakeup_en) { keyboard->on_usb_suspend(); }

void tud_resume_cb() { keyboard->on_usb_resume(); }

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len) { keyboard->send_usb_report(); }

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer,
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

void tud_hid_set_report_cb(uint8_t const instance, const uint8_t report_id, const hid_report_type_t report_type,
                           uint8_t const* buffer, const uint16_t bufsize)
{
    if (report_type == HID_REPORT_TYPE_OUTPUT)
    {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == static_cast<uint8_t>(mkeybo::KeycodeType::hid))
        {
            if (bufsize < 1)
            {
                return;
            }
            keyboard->on_usb_report_receive(buffer, bufsize);
        }
    }
}
