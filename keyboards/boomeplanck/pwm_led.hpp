#pragma once

#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "utils.hpp"


class PWMLed {
private:
    uint8_t pwm_pin_;
    uint16_t pwm_slice_ = 0;
    uint16_t pwm_channel_ = 0;
    uint8_t pwm_duty_ = 50;
    uint16_t pwm_frequency_ = 1000;
    uint32_t pwm_divider16_ = 0;
    uint32_t pwm_wrap_ = 0;

    uint8_t blink_duty_ = 0;
    uint8_t blink_current_ = 0;
    uint32_t blink_interval_ms_ = 0;
    uint32_t blink_start_ms_ = 0;
    bool blink_state_ = false;

public:
    explicit PWMLed(const uint8_t pwm_pin, const uint8_t pwm_duty, const uint16_t pwm_frequency = 1000)
        : pwm_pin_(pwm_pin), pwm_duty_(pwm_duty), pwm_frequency_(pwm_frequency) {}

    void init() {
        gpio_init(pwm_pin_);
        gpio_set_function(pwm_pin_, GPIO_FUNC_PWM);
        pwm_slice_ = pwm_gpio_to_slice_num(pwm_pin_);
        pwm_channel_ = pwm_gpio_to_channel(pwm_pin_);
        pwm_set_chan_level(pwm_slice_, pwm_channel_, 0);
        pwm_set_enabled(pwm_slice_, true);
        pwm_setup_freq();
    }

    void pwm_setup_freq() {
        constexpr uint32_t clock = 125000000;// TODO: get automatically
        pwm_divider16_ =
            clock / pwm_frequency_ / 4096 + (clock % (pwm_frequency_ * 4096) != 0);
        if (pwm_divider16_ / 16 == 0) {
            pwm_divider16_ = 16;
        }
        pwm_wrap_ = clock * 16 / pwm_divider16_ / pwm_frequency_ - 1;
        pwm_set_clkdiv_int_frac(pwm_slice_, pwm_divider16_ / 16, pwm_divider16_ & 0xF);
        pwm_set_wrap(pwm_slice_, pwm_wrap_);
    }

    void set_duty(const uint8_t duty) {
        pwm_duty_ = duty;
        pwm_set_chan_level(pwm_slice_, pwm_channel_, pwm_wrap_ * pwm_duty_ / 100);
    }

    void blink(const uint32_t interval_ms, const uint8_t duty = 100, const uint8_t blink_max = 1) {
        blink_interval_ms_ = interval_ms;
        blink_start_ms_ = get_ms_since_boot();
        blink_current_ = blink_max * 2;
        blink_state_ = true;
        blink_duty_ = duty;
        set_duty(blink_duty_);
    }

    void task() {
        if (!blink_interval_ms_) {
            // not blinking
            return;
        }
        const auto current_ms_since_boot = get_ms_since_boot();
        if (current_ms_since_boot - blink_start_ms_ < blink_interval_ms_) {
            // not enough time
            return;
        }
        blink_start_ms_ = current_ms_since_boot;
        --blink_current_;
        if (blink_current_ == 0) {
            set_duty(0);
            blink_interval_ms_ = 0;
        } else {
            blink_state_ = !blink_state_;
            set_duty(blink_state_ ? blink_duty_ : 0);
        }
    }
};
