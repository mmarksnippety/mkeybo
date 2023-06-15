#pragma once

#include "components/Keyboard.h"
#include "components/PWMLed.h"


template <uint8_t switches_count>
class BoomeplanckKeyboard : public Keyboard<switches_count> {
    using Keyboard<switches_count>::keyboard_state_;

protected:
    PWMLed *right_led_;
    PWMLed *left_led_;

public:
    BoomeplanckKeyboard(
        KeyboardSettings const *keyboard_settings,
        KeyboardState<switches_count> *keyboard_state,
        SwitchReader<switches_count> *switch_reader,
        SwitchStateUpdater<switches_count> *switch_state_updater,
        Mapper<switches_count> *mapper,
        vector<HIDReporter<switches_count> *> reporters,
        PWMLed *right_led,
        PWMLed *left_led)
        : Keyboard<switches_count>(
              keyboard_settings,
              keyboard_state,
              switch_reader,
              switch_state_updater,
              mapper,
              reporters),
          right_led_(right_led), left_led_(left_led) {}

    void init() override {
        Keyboard<switches_count>::init();
        left_led_->init();
        right_led_->init();
    }

    void task() override {
        left_led_->task();
        right_led_->task();
    }

    void on_layer_change() override {
        if (keyboard_state_->is_layer_active(4)) {
            left_led_->set_duty(10);
            right_led_->set_duty(10);
        } else if (keyboard_state_->is_layer_active(3)) {
            left_led_->set_duty(100);
            right_led_->set_duty(100);
        } else if (keyboard_state_->is_layer_active(2)) {
            left_led_->set_duty(0);
            right_led_->set_duty(100);
        } else if (keyboard_state_->is_layer_active(1)) {
            left_led_->set_duty(100);
            right_led_->set_duty(0);
        } else {
            left_led_->set_duty(0);
            right_led_->set_duty(0);
        }
    }

    void on_mount() override { right_led_->blink(500, 50, 2); }

    void on_umount() override { right_led_->blink(200, 10, 1); }

    void on_suspend() override { left_led_->blink(200, 10, 1); }

    void on_resume() override { left_led_->blink(500, 50, 2); }
};