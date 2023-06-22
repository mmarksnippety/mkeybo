#pragma once

#include "pico/stdlib.h"
#include <array>
#include <bitset>
#include <map>
#include <vector>

#include "KeyboardConfig.h"


using namespace std;

/**
 * Keyboard behavior configuration data.
 */

class KeyboardSettings {
public:
    uint16_t keyboard_refresh_interval_ms;
    uint16_t usb_refresh_interval_ms;
    uint16_t switch_pressed_duration_ms;
    uint16_t switch_tap_interval_ms;
    uint16_t switch_hold_duration_ms;
    // computed
    uint8_t switch_pressed_duration_cycles;

    KeyboardSettings(
        uint16_t refresh_interval_ms,
        uint16_t usb_refresh_interval_ms,
        uint16_t switch_pressed_duration_ms,
        uint16_t switch_tap_interval_ms,
        uint16_t switch_hold_duration_ms)
        : keyboard_refresh_interval_ms(refresh_interval_ms),
          usb_refresh_interval_ms(usb_refresh_interval_ms),
          switch_pressed_duration_ms(switch_pressed_duration_ms),
          switch_tap_interval_ms(switch_tap_interval_ms), switch_hold_duration_ms(switch_hold_duration_ms) {
        switch_pressed_duration_cycles = switch_pressed_duration_ms / refresh_interval_ms;
    }
};


/**
 * One switch state
 */
struct SwitchState {
    uint8_t pressed = 0;
    uint8_t released = 0;
    uint8_t tap_dance_count = 0;
    bool tap_dance_end = false;
};


/**
 * Layer state
 */
struct LayerState {
    bool is_active = false;
    bool one_shot_key = false;
    bool is_mapped_any_key = false;
};


/**
 * Power state
 */
enum KeyboardPowerState {
    PowerOn,
    Standby,
};


template <uint8_t switches_count>
class KeyboardState {
public:
    // keyboard based states
    //    KeyboardPowerState power_state;
    map<uint8_t, LayerState> layers{};

    // over cycle based states
    array<SwitchState, switches_count> switches_state{};// logical state

    // current cycle switches states
    bitset<switches_count> switches_read_state{};// mechanical states
    bitset<switches_count> mapped_switches{};    // that switches are mapped in this cycle

    // current and prev cycle keycodes
    array<Keycode, 200> keycodes{};
    array<Keycode, 200> prev_keycodes{};
    size_t keycodes_index = 0;
    size_t prev_keycodes_index = 0;
    bool keycodes_ready = false;

    KeyboardState() {
        fill(begin(switches_state), end(switches_state), SwitchState{});
        fill(begin(keycodes), end(keycodes), Keycode{0, KCT_KEY});
        fill(begin(prev_keycodes), end(prev_keycodes), Keycode{0, KCT_KEY});
    }

    // reset state
    void reset() {
        switches_read_state.reset();
        mapped_switches.reset();
        prev_keycodes = keycodes;
        prev_keycodes_index = keycodes_index;
        fill(begin(keycodes), end(keycodes), Keycode{0, KCT_KEY});
        keycodes_index = 0;
    }

    // layers

    void activate_layer(uint8_t layer_no) { layers[layer_no].is_active = true; }

    bool is_layer_active(uint8_t layer_no) { return layers[layer_no].is_active; }

    void mark_layer_used(uint8_t layer_no) { layers[layer_no].is_mapped_any_key = true; }


    // switches

    bool is_switch_mapped(uint8_t switch_no) { return mapped_switches[switch_no] == true; }

    void mark_switch_mapped(uint8_t switch_no) { mapped_switches[switch_no] = true; }

    bool is_switch_pressed(uint8_t switch_no, KeyboardSettings const *keyboard_settings) {
        return switches_state[switch_no].pressed >= keyboard_settings->switch_pressed_duration_cycles;
    }

    // tap dance

    bool is_tap_dance_in_progres(uint8_t switch_no) {
        return switches_state[switch_no].tap_dance_count > 0;
    }

    bool is_tap_dance_end(uint8_t switch_no) { return switches_state[switch_no].tap_dance_end; }

    uint8_t get_tap_dance_count(uint8_t switch_no) { return switches_state[switch_no].tap_dance_count; }

    // keycodes

    bool emplace_keycode(Keycode const &keycode) {
        if (keycodes_index == keycodes.size()) {
            return false;
        }
        keycodes[keycodes_index] = keycode;
        keycodes_index++;
        return true;
    }

    bool are_keycodes_changed(KeycodeType const &keycode_type) {
        if (keycodes_index == 0 && prev_keycodes_index == 0) {
            return false;
        }
        size_t index = 0;
        size_t prev_index = 0;
        while ((index <= keycodes_index) && (prev_index <= prev_keycodes_index)) {
            if (keycodes_index > 0 && keycodes[index].type != keycode_type) {
                ++index;
                continue;
            }
            if (prev_keycodes_index > 0 && prev_keycodes[prev_index].type != keycode_type) {
                ++prev_index;
                continue;
            }
            if (keycodes_index > 0 && prev_keycodes_index > 0) {
                if (keycodes[index].code != prev_keycodes[prev_index].code) {
                    return true;
                }
            } else {
                return true;
            }
            ++index;
            ++prev_index;
        }
        return false;
    }

    auto get_keycodes(KeycodeType const &keycode_type) {
        std::vector<uint16_t> out_keycodes;
        for (auto index = 0; index < keycodes_index; ++index) {
            if (keycodes[index].type == keycode_type) {
                out_keycodes.emplace_back(keycodes[index].code);
            }
        }
        return out_keycodes;
    }
};
