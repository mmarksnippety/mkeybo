#pragma once

#include <limits>

#include "KeyboardState.h"

/**
 * This class encapsulate state of switches
 * Remember switch depressed, tapping, holding.
 * This class operate on physical switches, not a key codes
 */

template <uint8_t switches_count>
class SwitchStateUpdater {
private:
    KeyboardSettings const *keyboard_settings_;
    KeyboardState<switches_count> *keyboard_state_;

public:
    SwitchStateUpdater(
        KeyboardSettings const *keyboard_settings, KeyboardState<switches_count> *keyboard_state)
        : keyboard_settings_(keyboard_settings), keyboard_state_(keyboard_state) {}

    void update_state() {
        for (auto switch_no = 0; switch_no < switches_count; switch_no++) {
            auto switch_pressed = keyboard_state_->switches_read_state[switch_no];
            auto &switch_state = keyboard_state_->switches_state[switch_no];
            // reset tap dance
            if (switch_state.tap_dance_end) {
                switch_state.pressed = 0;
                switch_state.released = 0;
                switch_state.tap_dance_count = 0;
                switch_state.tap_dance_end = false;
            }
            // update switch state
            auto released_interval = switch_state.released * keyboard_settings_->refresh_interval_ms;
            if (switch_pressed) {
                // key pressed after released, check tap dance interval is not too long
                if (switch_state.pressed == 0 & switch_state.released > 0) {
                    if (released_interval > keyboard_settings_->switch_tap_interval_ms) {
                        switch_state.tap_dance_end = true;
                    }
                }
                switch_state.released = 0;
                if (switch_state.pressed < numeric_limits<uint8_t>::max()) {
                    switch_state.pressed++;
                }
            } else {
                // jeśli nie ma na co czekać więcej to robimy clear
                if (released_interval > keyboard_settings_->switch_tap_interval_ms) {
                    switch_state.tap_dance_end = true;
                } else {
                    // jeśli było naciśnięte, a nie jest to zaczynamy odliczać odstęp
                    if (switch_state.pressed > 0) {
                        switch_state.pressed = 0;
                        switch_state.released = 1;
                        switch_state.tap_dance_count++;
                    } else {
                        // jeśli jesteśmy w fazie liczenia odstępu to zwiększamy licznik
                        if (switch_state.released > 0 && switch_state.released < numeric_limits<uint8_t>::max()) {
                            switch_state.released++;
                        }
                    }
                }
            }
        }
    }
};
