#include "config.hpp"
#include "mkeybo/components/keyboard.hpp"
#include "mkeybo/components/base.hpp"
#include "tusb.h"

extern mkeybo::Keyboard<keyboard_config.switches_count>* keyboard;


/**
 * Implementation of callback defined in /external/pico-sdk/lib/tinyusb/src/device/usbd.h
 * This set of callback are connected with device handshake process
 */

/**
 * Device descriptor
 * link to learn what a set of parameters is:
 * https://www.beyondlogic.org/usbnutshell/usb5.shtml#DeviceDescriptors
 *
 * A combination of interfaces must have a unique product id, since PC will save device driver after the
 * first plug. Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause
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

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const* tud_descriptor_device_cb() { return reinterpret_cast<uint8_t const*>(&desc_device); }

//--------------------------------------------------------------------+
// HID Report Descriptor
//--------------------------------------------------------------------+
constexpr uint8_t desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(static_cast<uint8_t>(mkeybo::KeycodeType::hid))),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(static_cast<uint8_t>(mkeybo::KeycodeType::cc))),
};

// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// contents must exist long enough for transfer to complete
uint8_t const* tud_hid_descriptor_report_cb(const uint8_t instance) { return desc_hid_report; }

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum
{
    ITF_NUM_HID,
    ITF_NUM_TOTAL
};

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN)

#define EPNUM_HID 0x81

constexpr uint8_t desc_configuration[] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

    // Interface number, string index, protocol, report descriptor len, EP In address, size & polling
    // interval
    TUD_HID_DESCRIPTOR(ITF_NUM_HID, 0, HID_ITF_PROTOCOL_NONE, sizeof(desc_hid_report), EPNUM_HID,
                       CFG_TUD_HID_EP_BUFSIZE, 5)};

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
// Configuration descriptor in the other speed e.g if high speed then this is for full speed and vice versa
uint8_t const* tud_descriptor_other_speed_configuration_cb(uint8_t index)
{
    (void)index; // for multiple configurations
    // other speed config is basically configuration with type = OTHER_SPEED_CONFIG
    memcpy(desc_other_speed_config, desc_configuration, CONFIG_TOTAL_LEN);
    desc_other_speed_config[1] = TUSB_DESC_OTHER_SPEED_CONFIG;
    // this example use the same configuration for both high and full speed mode
    return desc_other_speed_config;
}

#endif // highspeed

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor.
// Descriptor contents must exist long enough for transfer to complete
uint8_t const* tud_descriptor_configuration_cb(const uint8_t index)
{
    // index is for multiple configurations
    // This example use the same configuration for both high and full speed mode
    return desc_configuration;
}

/**
 * String Descriptors
 * array of pointer to string descriptors:
 *
 * char const *string_desc_arr[] = {
 *     (const char[]){0x09, 0x04},  // 0: is supported language is English (0x0409)
 *     KEYBOARD_MANUFACTURER,       // 1: Manufacturer
 *     KEYBOARD_MODEL,              // 2: Product
 *     "0"                          // 3: Pico board id, load in runtime
 * };
 */
// extern char const* string_desc_arr[4];

static uint16_t desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(const uint8_t index, const uint16_t langid)
{
    // std::cout << "tud_descriptor_string_cb: " << std::to_string(index) << ", " << langid << std::endl;
    (void)langid;
    if (string_desc_arr[3] == "0")
    {
        string_desc_arr[3] = keyboard->get_unique_id().data();
    }
    uint8_t chr_count;
    if (index == 0)
    {
        memcpy(&desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    }
    else
    {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors
        if (index >= std::size(string_desc_arr))
        {
            return nullptr;
        }
        const char* str = string_desc_arr[index];
        // Cap at max char
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
