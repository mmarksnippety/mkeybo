#pragma once

#include <bitset>
#include <iostream>
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "components/KeyboardState.h"
#include "components/SwitchReader.h"


using namespace std;


/**
 * This switch reader read state column by column, settings high state on row.
 * @tparam switches_count
 */
template <uint8_t switches_count>
class MatrixSwitchReader : public SwitchReader<switches_count> {
    using SwitchReader<switches_count>::keyboard_state_;

private:
    uint8_t const col_start_pin_;
    uint8_t const cols_;
    uint8_t const row_start_pin_;
    uint8_t const rows_;
    uint32_t cols_mask_;
    uint32_t rows_mask_;

public:
    explicit MatrixSwitchReader(
        uint8_t col_start_pin,
        uint8_t cols,
        uint8_t row_start_pin,
        uint8_t rows,
        KeyboardState<switches_count> *keyboard_state)
        : SwitchReader<switches_count>(keyboard_state), col_start_pin_(col_start_pin), cols_(cols),
          row_start_pin_(row_start_pin), rows_(rows) {
        cols_mask_ = ((1 << cols_) - 1) << col_start_pin_;
        rows_mask_ = ((1 << rows_) - 1) << row_start_pin_;// Row mask is unused, allways read all gpio
    }

    void init() const {
        for (auto offset = 0; offset < cols_; offset++) {
            auto pin = col_start_pin_ + offset;
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_OUT);
            cout << "Col pin: " << to_string(pin) << " initialized OUT" << endl;
        }
        for (auto offset = 0; offset < rows_; offset++) {
            auto pin = row_start_pin_ + offset;
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_IN);
            cout << "Row pin: " << to_string(pin) << " initialized IN" << endl;
        }
    }

    void read_state() {
        uint8_t raw_buffer_mask = 1;
        uint8_t raw_buffer_index = 0;

        // iterate over columns
        uint32_t col_pin = 1 << col_start_pin_;
        for (uint8_t col = 0; col < cols_; col++) {
            gpio_put_masked(cols_mask_, col_pin);
            busy_wait_us_32(10);// wait for column changed
            auto row_input = gpio_get_all();
            row_input &= rows_mask_;
            row_input >>= row_start_pin_;
            for (uint8_t row = 0; row < rows_; row++) {
                uint8_t key_no = (row * cols_) + col;
                keyboard_state_->switches_read_state[key_no] = row_input & 1;
                row_input >>= 1;
                raw_buffer_mask <<= 1;
                if (raw_buffer_mask == 0) {
                    raw_buffer_mask = 1;
                    raw_buffer_index++;
                }
            }
            col_pin <<= 1;
        }
    }
};
