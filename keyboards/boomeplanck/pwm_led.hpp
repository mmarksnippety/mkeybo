#pragma once

#include "hardware/pwm.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "mkeybo/utils.hpp"


struct PWMLedConfig
{
    uint8_t pwm_pin;
};


class PWMLed {
    uint8_t pwm_pin;
    uint16_t pwm_slice = 0;
    uint16_t pwm_channel = 0;
    uint8_t pwm_duty = 0;
    uint16_t pwm_frequency = 1000;
    uint32_t pwm_divider16 = 0;
    uint32_t pwm_wrap = 0;

    uint8_t blink_duty = 0;
    uint8_t blink_current = 0;
    uint32_t blink_interval_ms = 0;
    uint32_t blink_start_ms = 0;
    bool blink_state = false;

public:
    explicit PWMLed(const uint8_t pwm_pin)
        : pwm_pin(pwm_pin)
    {
        gpio_init(pwm_pin);
        gpio_set_function(pwm_pin, GPIO_FUNC_PWM);
        pwm_slice = pwm_gpio_to_slice_num(pwm_pin);
        pwm_channel = pwm_gpio_to_channel(pwm_pin);
        pwm_set_chan_level(pwm_slice, pwm_channel, 0);
        pwm_set_enabled(pwm_slice, true);
        pwm_setup_freq();
    }

    void pwm_setup_freq() {
        constexpr uint32_t clock = 125000000;// TODO: get automatically
        pwm_divider16 =
            clock / pwm_frequency / 4096 + (clock % (pwm_frequency * 4096) != 0);
        if (pwm_divider16 / 16 == 0) {
            pwm_divider16 = 16;
        }
        pwm_wrap = clock * 16 / pwm_divider16 / pwm_frequency - 1;
        pwm_set_clkdiv_int_frac(pwm_slice, pwm_divider16 / 16, pwm_divider16 & 0xF);
        pwm_set_wrap(pwm_slice, pwm_wrap);
    }

    void set_duty(const uint8_t duty) {
        pwm_duty = duty;
        pwm_set_chan_level(pwm_slice, pwm_channel, pwm_wrap * pwm_duty / 100);
    }

    void blink(const uint32_t interval_ms, const uint8_t duty = 100, const uint8_t blink_max = 1) {
        blink_interval_ms = interval_ms;
        blink_start_ms = mkeybo::get_ms_since_boot();
        blink_current = blink_max * 2;
        blink_state = true;
        blink_duty = duty;
        set_duty(blink_duty);
    }

    void task() {
        if (!blink_interval_ms) {
            // not blinking
            return;
        }
        const auto current_ms_since_boot = mkeybo::get_ms_since_boot();
        if (current_ms_since_boot - blink_start_ms < blink_interval_ms) {
            // not enough time
            return;
        }
        blink_start_ms = current_ms_since_boot;
        --blink_current;
        if (blink_current == 0) {
            set_duty(0);
            blink_interval_ms = 0;
        } else {
            blink_state = !blink_state;
            set_duty(blink_state ? blink_duty : 0);
        }
    }
};
