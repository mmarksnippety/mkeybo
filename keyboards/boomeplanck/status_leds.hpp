#pragma once

#include "mkeybo/components/input_device.hpp"
#include "pwm_led.hpp"
#include "keyboard.hpp"

/**
 * Two status leds, indicates keyboard layers, and usb mounting
 */
template <size_t switches_count, size_t keycodes_buffer_size = 20>
class StatusLeds final : public mkeybo::Device
{
    PWMLed* left_led;
    PWMLed* right_led;
    Keyboard<switches_count, keycodes_buffer_size>* keyboard;

public:
    explicit StatusLeds(const uint8_t left_led_pin, const uint8_t right_led_pin,
                        Keyboard<switches_count, keycodes_buffer_size>* keyboard) :
        left_led(new PWMLed(left_led_pin)), right_led(new PWMLed(right_led_pin)), keyboard(keyboard)
    {
    }

    ~StatusLeds() override
    {
        delete right_led;
        delete left_led;
    }

    void main_task() override
    {
        right_led->task();
        left_led->task();
    }

    void update_state() override
    {
        if (keyboard->is_layer_active(3))
        {
            left_led->set_duty(10);
            right_led->set_duty(10);
        }
        else if (keyboard->is_layer_active(2))
        {
            left_led->set_duty(100);
            right_led->set_duty(100);
        }
        else if (keyboard->is_layer_active(1))
        {
            left_led->set_duty(0);
            right_led->set_duty(100);
        }
        else if (keyboard->is_layer_active(0))
        {
            left_led->set_duty(100);
            right_led->set_duty(0);
        }
        else
        {
            left_led->set_duty(0);
            right_led->set_duty(0);
        }
    }

    void on_usb_mount() override { right_led->blink(500, 50, 3); }

    void on_usb_umount() override { right_led->blink(200, 10, 1); }

    void on_usb_suspend() override { left_led->blink(200, 10, 1); }

    void on_usb_resume() override { left_led->blink(500, 50, 3); }
};
