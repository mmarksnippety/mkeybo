#pragma once

#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "tusb.h"

#include "components/KeyboardState.h"


using namespace std;


template <uint8_t switches_count>
class Action {
public:
    KeycodeType keycode_type;

protected:
    KeyboardState<switches_count> *keyboard_state_;

public:
    explicit Action(KeyboardState<switches_count> *keyboard_state, KeycodeType keycode_type)
        : keyboard_state_(keyboard_state), keycode_type(keycode_type) {}

    virtual void init() {}

    virtual void run() {}
};


template <uint8_t switches_count>
class InternalAction : public Action<switches_count> {
    using Action<switches_count>::keyboard_state_;
    using Action<switches_count>::keycode_type;

public:
    explicit InternalAction(KeyboardState<switches_count> *keyboard_state)
        : Action<switches_count>(keyboard_state, KCT_INTERNAL) {}

    void run() override {
        for (auto &keycode : keyboard_state_->keycodes[keycode_type]) {
            printf("Internal keycode!");
            if (keycode == INTERNAL_KEY_REBOOT) {
                // huu robimy reboot!
                reset_usb_boot(0, 0);
            }
        }
    }
};


template <uint8_t switches_count>
class MacroAction : public Action<switches_count> {
    using Action<switches_count>::keyboard_state_;
    using Action<switches_count>::keycode_type;

public:
    explicit MacroAction(KeyboardState<switches_count> *keyboard_state)
        : Action<switches_count>(keyboard_state, KCT_MACRO) {}

    void run() override {
        for (auto &keycode : keyboard_state_->keycodes[keycode_type]) {
            // TODO make macro!
        }
    }
};
