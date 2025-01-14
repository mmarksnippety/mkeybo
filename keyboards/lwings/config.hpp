#pragma once

#include "mkeybo/components/switch_reader_matrix.hpp"
#include "mkeybo/components/base.hpp"


constexpr mkeybo::KeyboardConfig keyboard_config{
    .switches_count = 40,
    .keyboard_name = "lwings",
    .manufactured_name = "mkeybo",
};

constexpr mkeybo::SwitchReaderMatrixConfig switch_reader_config{
    .column_start_pin = 4, .column_count = 10, .row_start_pin = 16, .row_count = 4};

//TODO: make it better :(, move it usb_descriptor?
inline char const* string_desc_arr[] = {
    (const char[]){0x09, 0x04}, // 0: is supported language is English (0x0409)
    keyboard_config.manufactured_name.c_str(), // 1: Manufacturer
    keyboard_config.keyboard_name.c_str(), // 2: Product
    "0" // 3: Pico board id, load in runtime
};

//TODO: display config copied from old code, make order
// i2c_inst *statusDisplayI2C = i2c1;
// const uint statusDisplaySDAPin = 2;
// const uint statusDisplaySCLPin = 3;
// const uint16_t statusDisplayAddress = 0x3c;
// const uint8_t statusDisplayWidth = 128;
// const uint8_t statusDisplayHeight = 64;

