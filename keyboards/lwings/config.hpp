#pragma once

#include "mkeybo/components/switch_reader_matrix.hpp"
#include "mkeybo/components/base.hpp"
#include "display_config.hpp"
#include "hardware/i2c.h"


constexpr mkeybo::KeyboardConfig keyboard_config{
    .switches_count = 40,
    .keyboard_name = "lwings",
    .manufactured_name = "mkeybo",
};


constexpr mkeybo::SwitchReaderMatrixConfig switch_reader_config{
    .column_start_pin = 4, .column_count = 10, .row_start_pin = 16, .row_count = 4};


constexpr DisplayConfig display_config
{
    .i2c = i2c1,
    .sda_pin = 2,
    .scl_pin = 3,
    .address = 0x3c,
    .width = 128,
    .height = 64,
};


//TODO: make it better :(, move it usb_descriptor?
inline char const* string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    keyboard_config.manufactured_name.c_str(), // 1: Manufacturer
    keyboard_config.keyboard_name.c_str(), // 2: Product
    "0" // 3: Pico board id, load in runtime
};
