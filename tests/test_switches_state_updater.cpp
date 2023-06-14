
#include "pico/stdlib.h"
#include <iostream>

#include "components/KeyboardConfig.h"
#include "components/KeyboardState.h"
#include "components/SwitchStateUpdater.h"


void print_switch_state(SwitchState &s, int8_t cycle = -1) {
    // clang-format off
    if (cycle >= 0) {
        cout << "C" << to_string(cycle) << " | ";
    }
    cout
        << to_string(s.pressed)
        << " | " << to_string(s.released)
        << " | " << to_string(s.tap_dance_count)
        << " | " << to_string(s.tap_dance_end)
        << endl;
    // clang-format on
}


void test_switches_state_updater_empty() {
    auto keyboard_settings = new KeyboardSettings(50, 10, 50, 150, 300);;
    auto keyboard_state = new KeyboardState<3>{};
    auto switch_state_updater = new SwitchStateUpdater<3>(keyboard_settings, keyboard_state);

    cout << ". empty keyboard read state -> empty keyboard state" << endl;
    keyboard_state->switches_read_state.reset();
    switch_state_updater->update_state();
    for (auto &state : keyboard_state->switches_state) {
        assert(state.pressed == 0);
        assert(state.released == 0);
        assert(state.tap_dance_count == 0);
        assert(state.tap_dance_end == false);
    }
    cout << "ok" << endl;
}


void test_switches_state_updater_key_pressed() {
    auto keyboard_settings = new KeyboardSettings(50, 10, 50, 150, 300);;
    auto keyboard_state = new KeyboardState<3>{};
    auto switch_state_updater = new SwitchStateUpdater<3>(keyboard_settings, keyboard_state);

    cout << ". key pressed -> proper state" << endl;
    // C0 - key 1 pressed
    keyboard_state->switches_read_state.reset();
    keyboard_state->switches_read_state[1] = true;
    switch_state_updater->update_state();
    for (uint8_t switch_no = 0; switch_no < 3; switch_no++) {
        auto &state = keyboard_state->switches_state[switch_no];
        if (switch_no == 1) {
            assert(state.pressed == 1);
            assert(state.released == 0);
            assert(state.tap_dance_count == 0);
            assert(state.tap_dance_end == false);
        } else {
            assert(state.pressed == 0);
            assert(state.released == 0);
            assert(state.tap_dance_count == 0);
            assert(state.tap_dance_end == false);
        }
    }
    // C2 - key 1 pressed
    keyboard_state->switches_read_state.reset();
    keyboard_state->switches_read_state[1] = true;
    switch_state_updater->update_state();
    for (uint8_t switch_no = 0; switch_no < 3; switch_no++) {
        auto &state = keyboard_state->switches_state[switch_no];
        if (switch_no == 1) {
            assert(state.pressed == 2);
            assert(state.released == 0);
            assert(state.tap_dance_count == 0);
            assert(state.tap_dance_end == false);
        } else {
            assert(state.pressed == 0);
            assert(state.released == 0);
            assert(state.tap_dance_count == 0);
            assert(state.tap_dance_end == false);
        }
    }
    // C3 - key 1 released
    keyboard_state->switches_read_state.reset();
    switch_state_updater->update_state();
    for (uint8_t switch_no = 0; switch_no < 3; switch_no++) {
        auto &state = keyboard_state->switches_state[switch_no];
        if (switch_no == 1) {
            assert(state.pressed == 0);
            assert(state.released == 1);
            assert(state.tap_dance_count == 1);
            assert(state.tap_dance_end == false);
        } else {
            assert(state.pressed == 0);
            assert(state.released == 0);
            assert(state.tap_dance_count == 0);
            assert(state.tap_dance_end == false);
        }
    }
    // C4 key 1 released
    keyboard_state->switches_read_state.reset();
    switch_state_updater->update_state();
    for (uint8_t switch_no = 0; switch_no < 3; switch_no++) {
        auto &state = keyboard_state->switches_state[switch_no];
        if (switch_no == 1) {
            assert(state.pressed == 0);
            assert(state.released == 2);
            assert(state.tap_dance_count == 1);
            assert(state.tap_dance_end == false);
        } else {
            assert(state.pressed == 0);
            assert(state.released == 0);
            assert(state.tap_dance_count == 0);
            assert(state.tap_dance_end == false);
        }
    }
    cout << "ok" << endl;
}


void test_switches_state_updater_tap_dance_count_2() {
    auto keyboard_settings = new KeyboardSettings(50, 10, 50, 150, 300);;
    auto keyboard_state = new KeyboardState<3>{};
    auto switch_state_updater = new SwitchStateUpdater<3>(keyboard_settings, keyboard_state);

    cout << ". key pressed 2 times -> tap dance == 2" << endl;
    // C0 - tap key 1
    keyboard_state->switches_read_state.reset();
    keyboard_state->switches_read_state[1] = true;
    switch_state_updater->update_state();
    auto &state = keyboard_state->switches_state[1];
    print_switch_state(state, 0);
    assert(state.pressed == 1);
    assert(state.released == 0);
    assert(state.tap_dance_count == 0);
    assert(state.tap_dance_end == false);
    // C1 - release key 1
    keyboard_state->switches_read_state.reset();
    keyboard_state->switches_read_state[1] = false;
    switch_state_updater->update_state();
    state = keyboard_state->switches_state[1];
    print_switch_state(state, 1);
    assert(state.pressed == 0);
    assert(state.released == 1);
    assert(state.tap_dance_count == 1);
    assert(state.tap_dance_end == false);
    // C2 - tap key 1
    keyboard_state->switches_read_state.reset();
    keyboard_state->switches_read_state[1] = true;
    switch_state_updater->update_state();
    state = keyboard_state->switches_state[1];
    print_switch_state(state, 2);
    assert(state.pressed == 1);
    assert(state.released == 0);
    assert(state.tap_dance_count == 1);
    assert(state.tap_dance_end == false);
    assert(keyboard_state->is_tap_dance_in_progres(1) == true);
    assert(keyboard_state->is_tap_dance_end(1) == false);
    // C3, C4, C5 - key 1 released
    for (int8_t i = 0; i < 4; i++) {
        keyboard_state->switches_read_state.reset();
        keyboard_state->switches_read_state[1] = false;
        switch_state_updater->update_state();
        state = keyboard_state->switches_state[1];
        print_switch_state(state, i + 3);
        assert(keyboard_state->is_tap_dance_in_progres(1) == true);
        assert(keyboard_state->is_tap_dance_end(1) == false);
    }
    assert(state.pressed == 0);
    assert(state.released == 4);
    assert(state.tap_dance_count == 2);
    assert(state.tap_dance_end == false);
    assert(keyboard_state->is_tap_dance_in_progres(1) == true);
    assert(keyboard_state->is_tap_dance_end(1) == false);
    // C6 - key 1 released - last tap dance cycle
    keyboard_state->switches_read_state.reset();
    keyboard_state->switches_read_state[1] = false;
    switch_state_updater->update_state();
    state = keyboard_state->switches_state[1];
    print_switch_state(state, 6);
    assert(state.pressed == 0);
    assert(state.released == 4);
    assert(state.tap_dance_count == 2);
    assert(state.tap_dance_end == true);
    assert(keyboard_state->is_tap_dance_in_progres(1) == true);
    assert(keyboard_state->is_tap_dance_end(1) == true);
    // C7 - key release, tap dance reset
    keyboard_state->switches_read_state.reset();
    keyboard_state->switches_read_state[1] = false;
    switch_state_updater->update_state();
    state = keyboard_state->switches_state[1];
    print_switch_state(state, 7);
    assert(state.pressed == 0);
    assert(state.released == 0);
    assert(state.tap_dance_count == 0);
    assert(state.tap_dance_end == false);
    assert(keyboard_state->is_tap_dance_in_progres(1) == false);
    assert(keyboard_state->is_tap_dance_end(1) == false);
    //
    cout << "ok" << endl;
}
