#pragma once

#include <bitset>
#include <iostream>
#include "pico/stdlib.h"
#include "SwitchReader.hpp"


/**
 * This switch reader read state column by column.
 *
 * How it works: In loop settings high state on column and next read all switches
 * state in this column.
 *
 * @tparam switches_count
 */

namespace mkeybo {

struct SwitchReaderMatrixConfig
{
    uint8_t column_start_pin;
    uint8_t column_count;
    uint8_t row_start_pin;
    uint8_t row_count;
};


template <size_t switches_count>
class SwitchReaderMatrix final : public SwitchReader<switches_count>
{
    uint8_t const column_start_pin_;
    uint8_t const column_count_;
    uint8_t const row_start_pin_;
    uint8_t const row_count_;
    uint32_t cols_bit_mask_;
    uint32_t rows_bit_mask_;

public:
    explicit SwitchReaderMatrix(
        const uint8_t column_start_pin,
        const uint8_t column_count,
        const uint8_t row_start_pin,
        const uint8_t row_count) :
        column_start_pin_(column_start_pin), column_count_(column_count),
        row_start_pin_(row_start_pin), row_count_(row_count)
    {
        cols_bit_mask_ = (1 << column_count) - 1 << column_start_pin;
        rows_bit_mask_ = (1 << row_count) - 1 << row_start_pin;
        for (auto offset = 0; offset < column_count; offset++)
        {
            const auto pin = column_start_pin + offset;
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_OUT);
            std::cout << "Column pin: " << std::to_string(pin) << " initialized OUT" << std::endl;
        }
        for (auto offset = 0; offset < row_count; offset++)
        {
            const auto pin = row_start_pin + offset;
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_IN);
            std::cout << "Row pin: " << std::to_string(pin) << " initialized IN" << std::endl;
        }
    }

    void update(std::bitset<switches_count>& switch_states) override
    {
        // iterate over columns and read rows
        uint8_t raw_buffer_mask = 1;
        uint8_t raw_buffer_index = 0;
        uint32_t column_bit_pin = 1 << column_start_pin_;
        for (uint8_t current_column = 0; current_column < column_count_; current_column++)
        {
            gpio_put_masked(cols_bit_mask_, column_bit_pin);
            busy_wait_us_32(10); // wait for column changed
            auto row_input = gpio_get_all();
            row_input &= rows_bit_mask_;
            row_input >>= row_start_pin_;
            for (uint8_t current_row = 0; current_row < row_count_; current_row++)
            {
                uint8_t switch_index = (current_row * column_count_) + current_column;
                switch_states[switch_index] = row_input & 1;
                row_input >>= 1;
                raw_buffer_mask <<= 1;
                if (raw_buffer_mask == 0)
                {
                    raw_buffer_mask = 1;
                    raw_buffer_index++;
                }
            }
            column_bit_pin <<= 1;
        }
    }
};

}
