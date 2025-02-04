/**
 * Implementation of callback defined in /external/pico-sdk/lib/tinyusb/src/device/usbd.h
 * This set of callback are connected with device handshake process
 */

#include "config.hpp"
#include "mkeybo/components/hid_controller.hpp"
#include "tusb.h"

extern mkeybo::HidController* hid_controller;


/**
 * Device descriptor
 * link to learn what a set of parameters is:
 * https://www.beyondlogic.org/usbnutshell/usb5.shtml#DeviceDescriptors
 *
 * A combination of interfaces must have a unique product id, since PC will save device driver after the
 * first plug. Same VID/PID with different interface e.g. MSC (first), then CDC (later) will possibly cause
 * system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))
#define USB_PID                                                                                                        \
    (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))
#define USB_VID 0x1209
#define USB_BCD 0x0200

constexpr tusb_desc_device_t desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = USB_BCD,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USB_VID,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = 0x01,
    .iProduct = 0x02,
    .iSerialNumber = 0x03,
    .bNumConfigurations = 0x01
};

uint8_t const* tud_descriptor_device_cb()
{
    return reinterpret_cast<uint8_t const*>(&desc_device);
}


uint8_t const* tud_hid_descriptor_report_cb(const uint8_t instance)
{
    return hid_controller->get_usb_reports_description().data();
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

#if TUD_OPT_HIGH_SPEED
// Per USB specs: high speed capable device must report device_qualifier and other_speed_configuration

// other speed configuration
uint8_t desc_other_speed_config[CONFIG_TOTAL_LEN];

// device qualifier is mostly similar to device descriptor
// since we don't change configuration based on speed
tusb_desc_device_qualifier_t const desc_device_qualifier = {
    .bLength = sizeof(tusb_desc_device_qualifier_t),
    .bDescriptorType = TUSB_DESC_DEVICE_QUALIFIER,
    .bcdUSB = USB_BCD,
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .bNumConfigurations = 0x01,
    .bReserved = 0x00
};

// Invoked when received GET DEVICE QUALIFIER DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete.
// device_qualifier descriptor describes information about a high-speed capable device that would
// change if the device were operating at the other speed. If not highspeed capable stall this request.
uint8_t const* tud_descriptor_device_qualifier_cb(void)
{
    return reinterpret_cast<uint8_t const*>(&desc_device_qualifier);
}

// Invoked when received GET OTHER SEED CONFIGURATION DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
// Configuration descriptor in the other speed e.g. if high speed then this is for full speed and vice versa
uint8_t const* tud_descriptor_other_speed_configuration_cb(uint8_t index)
{
    (void)index; // for multiple configurations
    // other speed config is basically configuration with type = OTHER_SPEED_CONFIG
    // memcpy(desc_other_speed_config, desc_configuration, CONFIG_TOTAL_LEN);
    memcpy(desc_other_speed_config, hid_controller->get_usb_configuration_description().data(), CONFIG_TOTAL_LEN);
    desc_other_speed_config[1] = TUSB_DESC_OTHER_SPEED_CONFIG;
    // this example use the same configuration for both high and full speed mode
    return desc_other_speed_config;
}

#endif // highspeed

uint8_t const* tud_descriptor_configuration_cb(const uint8_t index)
{
    return hid_controller->get_usb_configuration_description().data();
}


static uint16_t desc_str[32];

uint16_t const* tud_descriptor_string_cb(const uint8_t index, const uint16_t langid)
{
    uint8_t chr_count;
    // is supported language is English (0x0409)
    if (index == 0)
    {
        desc_str[1] = 0x09;
        desc_str[2] = 0x04;
        chr_count = 1;
    }
    else
    {
        const char* str{};
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
        switch (index)
        {
        case 1:
            str = hid_controller->get_manufactured_name().data();
            break;
        case 2:
            str = hid_controller->get_device_name().data();
            break;
        case 3:
            str = hid_controller->get_unique_id().data();
            break;
        default:
            return nullptr;
        }
        chr_count = strlen(str);
        if (chr_count > 31)
        {
            chr_count = 31;
        }
        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++)
        {
            desc_str[1 + i] = str[i];
        }
    }
    // first byte is length (including header), second byte is string type
    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
    return desc_str;
}
