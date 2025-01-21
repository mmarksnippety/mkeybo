#pragma once

#include "mkeybo/components/switch_readers/matrix_switch_reader.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/switch_readers/direct_switch_reader.hpp"


constexpr mkeybo::KeyboardConfig keyboard_config{
    .switches_count = 4 * 12,
    .keyboard_name = "boomeplanck",
    .manufactured_name = "mkeybo",
};


constexpr mkeybo::switch_reader::MatrixSwitchReaderConfig switch_reader_config{
    .column_start_pin = 10, .column_count = 12, .row_start_pin = 6, .row_count = 4};
