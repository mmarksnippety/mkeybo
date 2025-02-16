#pragma once

#include "mkeybo/components/keyboard/switch_readers/matrix_switch_reader.hpp"
#include "mkeybo/components/base.hpp"
#include "status_display_config.hpp"
#include "hardware/i2c.h"


constexpr mkeybo::KeyboardConfig keyboard_config{
    .switches_count = 40,
    .keyboard_name = "lwings",
    .manufactured_name = "mkeybo",
};


constexpr mkeybo::keyboard::switch_reader::MatrixSwitchReaderConfig switch_reader_config{
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
