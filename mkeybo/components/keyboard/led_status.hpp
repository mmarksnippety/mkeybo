#pragma once

#include <pico/stdlib.h>


namespace mkeybo::keyboard {


/**
 * Received from usb host keyboard led status
 */
struct LedStatus
{
    uint8_t num_lock : 1; // Bit 0: Num Lock (1 bit)
    uint8_t caps_lock : 1; // Bit 1: Caps Lock (1 bit)
    uint8_t scroll_lock : 1; // Bit 2: Scroll Lock (1 bit)
    uint8_t compose : 1; // Bit 3: Compose (1 bit)
    uint8_t kana : 1; // Bit 4: Kana (1 bit)
    uint8_t reserved : 3; // Bits 5-7: Reserved (3 bits, must be zero)
};


}
