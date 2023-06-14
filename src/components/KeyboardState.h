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
    uint16_t refresh_interval_ms;
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
        : refresh_interval_ms(refresh_interval_ms), usb_refresh_interval_ms(usb_refresh_interval_ms),
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

    // current cycle keycodes
    array<vector<uint16_t>, KCT_COUNT> keycodes;

    // prev cycle keycodes
    array<vector<uint16_t>, KCT_COUNT> prev_keycodes;

    KeyboardState() {
        fill(begin(switches_state), end(switches_state), SwitchState{});
        for (int keycode_type = KCT_KEY; keycode_type != KCT_COUNT; keycode_type++) {
            keycodes[keycode_type] = vector<uint16_t>{};
            prev_keycodes[keycode_type] = vector<uint16_t>{};
        }
    }

    // reset state
    void reset() {
        switches_read_state.reset();
        mapped_switches.reset();
        // move current keycodes to prev keycodes, reset current keycodes
        for (int keycode_type = KCT_KEY; keycode_type != KCT_COUNT; keycode_type++) {
            prev_keycodes[keycode_type].clear();
            for (auto &keycode : keycodes[keycode_type]) {
                prev_keycodes[keycode_type].emplace_back(keycode);
            }
            keycodes[keycode_type].clear();
        }
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

    bool is_tap_dance_end(uint8_t switch_no) {
        return switches_state[switch_no].tap_dance_end;
    }

    uint8_t get_tap_dance_count(uint8_t switch_no) {
        return switches_state[switch_no].tap_dance_count;
    }

    // keycodes

    void emplace_keycode(KeycodeType const &keycode_type, uint16_t const &keycode) {
        keycodes[keycode_type].emplace_back(keycode);
    }
};
