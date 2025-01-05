#ifndef __lwings_config_hpp__
#define __lwings_config_hpp__

#include "mkeybo/components/KeyboardConfig.hpp"
#include "mkeybo/components/SwitchReaderMatrix.hpp"


constexpr mkeybo::KeyboardConfig keyboard_config{
    .switches_count = 12,
    .switches_refresh_interval = 50
};

constexpr mkeybo::SwitchReaderMatrixConfig switch_reader_config{
    .column_start_pin = 0,
    .column_count = 4,
    .row_start_pin = 7,
    .row_count = 3
};


#endif // __lwings_config_hpp__
