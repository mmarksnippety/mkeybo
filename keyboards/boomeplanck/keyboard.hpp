#pragma once


#include "config.hpp"
#include "mkeybo/components/keyboard.hpp"
#include "mkeybo/factories.hpp"
#include "pwm_led.hpp"


template <size_t switches_count, size_t keycodes_buffer_size = 20>
class Keyboard final : public mkeybo::Keyboard<switches_count, keycodes_buffer_size>
{
PWMLed *right_led;
PWMLed *left_led;

public:
    explicit Keyboard() :
        mkeybo::Keyboard<switches_count>(
            mkeybo::create_switch_reader_matrix<switches_count>(switch_reader_config),
            mkeybo::create_switch_events_generator<switches_count>(),
            mkeybo::create_keycode_mapping_rules<switches_count, keycodes_buffer_size>()
        ), right_led(new PWMLed(5, 0)), left_led(new PWMLed(4, 0))
    {
    }

    ~Keyboard() override
    {
        delete right_led;
        delete left_led;
    }

    void leds_task() const
    {
        right_led->task();
        left_led->task();
    }

    void on_generate_keycodes() override
    {
        if (this->is_layer_active(3)) {
            left_led->set_duty(10);
            right_led->set_duty(10);
        } else if (this->is_layer_active(2)) {
            left_led->set_duty(100);
            right_led->set_duty(100);
        } else if (this->is_layer_active(1)) {
            left_led->set_duty(0);
            right_led->set_duty(100);
        } else if (this->is_layer_active(0)) {
            left_led->set_duty(100);
            right_led->set_duty(0);
        } else {
            left_led->set_duty(0);
            right_led->set_duty(0);
        }
    }

    void on_usb_mount() override { right_led->blink(500, 50, 3); }

    void on_usb_umount() override { right_led->blink(200, 10, 1); }

    void on_usb_suspend() override { left_led->blink(200, 10, 1); }

    void on_usb_resume() override { left_led->blink(500, 50, 3); }
};
