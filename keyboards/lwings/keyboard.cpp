#include <iostream>
#include <ostream>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#include "mkeybo/components/SwitchReaderMatrix.hpp"
#include "config.hpp"


int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}


int main()
{
    stdio_init_all();

    // Timer example code - This example fires off the callback after 2000ms
    add_alarm_in_ms(2000, alarm_callback, nullptr, false);

    std::cout << "mkeybo" << std::endl;
    std::cout << "configuration" << std::endl;
    std::cout << "switches_count: " << std::to_string(keyboard_config.switches_count) << std::endl;
    std::cout << "switches_refresh_interval: " << std::to_string(keyboard_config.switches_refresh_interval) << std::endl;

    auto reader = mkeybo::SwitchReaderMatrix<keyboard_config.switches_count>(
        switch_reader_config.column_start_pin,
        switch_reader_config.column_count,
        switch_reader_config.row_start_pin,
        switch_reader_config.row_count
    );

    return 0;
}

