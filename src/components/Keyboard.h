#pragma once

#include <iostream>

#include "Actions.h"
#include "CircularBuffer.h"
#include "HIDReporter.h"
#include "KeyboardConfig.h"
#include "KeyboardMapper.h"
#include "KeyboardState.h"
#include "SwitchReader.h"
#include "SwitchStateUpdater.h"


template <uint8_t switches_count>
class Keyboard {
protected:
    KeyboardSettings const *keyboard_settings_;
    KeyboardState<switches_count> *keyboard_state_;
    SwitchReader<switches_count> *switch_reader_;
    SwitchStateUpdater<switches_count> *switch_state_updater_;
    Mapper<switches_count> *mapper_;
    map<KeycodeType, HIDReporter<switches_count> *> reporters_;
    map<KeycodeType, Action<switches_count> *> actions_;

public:
    Keyboard(
        KeyboardSettings const *keyboard_settings,
        KeyboardState<switches_count> *keyboard_state,
        SwitchReader<switches_count> *switch_reader,
        SwitchStateUpdater<switches_count> *switch_state_updater,
        Mapper<switches_count> *mapper,
        vector<HIDReporter<switches_count> *> reporters,
        vector<Action<switches_count> *> actions)
        : keyboard_settings_(keyboard_settings), keyboard_state_(keyboard_state),
          switch_reader_(switch_reader), switch_state_updater_(switch_state_updater), mapper_(mapper) {
        for (const auto &reporter : reporters) {
            reporters_[reporter->keycode_type] = reporter;
        }
        for (const auto &action : actions) {
            actions_[action->keycode_type] = action;
        }
    }

    virtual void init() {
        switch_reader_->init();
        for (const auto &[keycode_type, reporter] : reporters_) {
            reporter->init();
        }
        for (const auto &[keycode_type, action] : actions_) {
            action->init();
        }
    }

    KeyboardSettings const *settings() { return keyboard_settings_; }

    KeyboardState<switches_count> *state() { return keyboard_state_; }

    HIDReporter<switches_count> *get_hid_reporter(KeycodeType keycode_type) {
        return reporters_[keycode_type];
    }

    bool is_any_hid_raport_to_send() {
        for (const auto &[keycode_type, reporter] : reporters_) {
            if (reporter->is_report_to_send()) {
                return true;
            }
        }
        return false;
    }

    void refresh() {
        keyboard_state_->reset();
        switch_reader_->read_state();
        switch_state_updater_->update_state();
        mapper_->make_layer_mapping();
        on_layer_change();
        mapper_->make_mapping();
        generate_hid_reports();
        // actions: internal, macros, ...
        do_actions();
    }

    virtual void task() {}

    //
    void generate_hid_reports() {
        for (auto const &[keycode_type, reporter] : reporters_) {
            reporter->blocking_enqueue_report();
        }
    }

    void do_actions() {
        for (const auto &[keycode_type, action] : actions_) {
            action->run();
        }
    }

    // events handler

    virtual void on_layer_change() {}

    virtual void on_mount() {}

    virtual void on_umount() {}

    virtual void on_suspend() {}

    virtual void on_resume() {}

    virtual void on_report_receive(uint8_t const *buffer) {
        // example from pico example
        //            uint8_t const kbd_leds = buffer[0];
        //
        //            if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
        //                // Capslock On: disable blink, turn led on
        //                blink_interval_ms = 0;
        //                board_led_write(true);
        //            } else {
        //                // Caplocks Off: back to normal blink
        //                board_led_write(false);
        //                blink_interval_ms = BLINK_MOUNTED;
        //            }
    }
};
