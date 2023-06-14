#pragma once

#include "hardware/timer.h"
#include "pico/stdlib.h"

#include "utils.h"


class LedBlinker {
private:
    uint8_t _pin;
    uint32_t _blink_interval_ms = 0;
    uint32_t _blink_start_ms = 0;
    bool _state = false;

public:
    LedBlinker(uint8_t pin) : _pin(pin) {}

    void init() {
        gpio_init(_pin);
        gpio_set_dir(_pin, GPIO_OUT);
    }

    void on() {
        _state = true;
        gpio_put(_pin, true);
    }

    void blink(uint32_t interval_ms) { _blink_interval_ms = interval_ms; }

    void off() {
        _blink_interval_ms = 0;
        _state = false;
        gpio_put(_pin, false);
    }

    void task() {
        if (!_blink_interval_ms) {
            // not blinking
            return;
        }
        auto current_ms_since_boot = get_ms_since_boot();
        if (current_ms_since_boot - _blink_start_ms < _blink_interval_ms) {
            // not enough time
            return;
        }
        _blink_start_ms = current_ms_since_boot;
        gpio_put(_pin, _state);
        _state = !_state;
    }
};
