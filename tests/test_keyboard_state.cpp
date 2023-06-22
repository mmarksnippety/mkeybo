
#include "pico/stdlib.h"
#include <iostream>

#include "components/KeyboardConfig.h"
#include "components/KeyboardState.h"
#include "components/SwitchStateUpdater.h"


// void print_switch_state(SwitchState &s, int8_t cycle = -1) {
//     // clang-format off
//     if (cycle >= 0) {
//         cout << "C" << to_string(cycle) << " | ";
//     }
//     cout
//         << to_string(s.pressed)
//         << " | " << to_string(s.released)
//         << " | " << to_string(s.tap_dance_count)
//         << " | " << to_string(s.tap_dance_end)
//         << endl;
//     // clang-format on
// }


void test_keyboard_state__emplace_keycode() {
    auto keyboard_state = new KeyboardState<3>{};

    cout << "KeyboardState emplace_keycode" << endl;
    cout << ".  emplace 4 keycodes and check state" << endl;
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{30, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_CC_KEY}));

    assert(keyboard_state->prev_keycodes_index == 0);
    assert(keyboard_state->keycodes_index == 4);
    assert(keyboard_state->keycodes[0].code == 10);
    assert(keyboard_state->keycodes[0].type == KCT_KEY);
    assert(keyboard_state->keycodes[1].code == 20);
    assert(keyboard_state->keycodes[1].type == KCT_KEY);
    assert(keyboard_state->keycodes[2].code == 30);
    assert(keyboard_state->keycodes[2].type == KCT_KEY);
    assert(keyboard_state->keycodes[3].code == 10);
    assert(keyboard_state->keycodes[3].type == KCT_CC_KEY);

    cout << "ok" << endl;
}


void test_keyboard_state__keycode_reset() {
    auto keyboard_state = new KeyboardState<3>{};
    cout << "KeyboardState emplace_keycode and reset state" << endl;
    cout << ". emplace 2 keycodes and check state" << endl;
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));

    assert(keyboard_state->prev_keycodes_index == 0);
    assert(keyboard_state->keycodes_index == 2);
    assert(keyboard_state->keycodes[0].code == 10);
    assert(keyboard_state->keycodes[0].type == KCT_KEY);
    assert(keyboard_state->keycodes[1].code == 20);
    assert(keyboard_state->keycodes[1].type == KCT_KEY);

    cout << ". reset and check state" << endl;
    keyboard_state->reset();
    assert(keyboard_state->keycodes_index == 0);
    assert(keyboard_state->prev_keycodes_index == 2);
    assert(keyboard_state->prev_keycodes[0].code == 10);
    assert(keyboard_state->prev_keycodes[0].type == KCT_KEY);
    assert(keyboard_state->prev_keycodes[1].code == 20);
    assert(keyboard_state->prev_keycodes[1].type == KCT_KEY);

    cout << "ok" << endl;
}


void test_keyboard_state__keycodes_changed__empty() {
    auto keyboard_state = new KeyboardState<3>{};
    cout << "KeyboardState keycodes_changed empty state" << endl;
    assert(keyboard_state->keycodes_index == 0);
    assert(keyboard_state->prev_keycodes_index == 0);
    assert(!keyboard_state->are_keycodes_changed(KCT_KEY));
    assert(!keyboard_state->are_keycodes_changed(KCT_CC_KEY));
    cout << "ok" << endl;
}


void test_keyboard_state__keycodes_changed__prev_keycodes_empty() {
    auto keyboard_state = new KeyboardState<3>{};
    cout << "KeyboardState keycodes_changed prev_keys empty, current keys not mixed" << endl;
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));
    assert(keyboard_state->keycodes_index == 2);
    assert(keyboard_state->prev_keycodes_index == 0);
    assert(keyboard_state->are_keycodes_changed(KCT_KEY));
    assert(!keyboard_state->are_keycodes_changed(KCT_CC_KEY));
    cout << "ok" << endl;
}


void test_keyboard_state__keycodes_changed__prev_keycodes_empty_mixed_type() {
    auto keyboard_state = new KeyboardState<3>{};
    cout << "KeyboardState keycodes_changed prev_keys empty, current keys mixed" << endl;
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_CC_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));
    assert(keyboard_state->keycodes_index == 3);
    assert(keyboard_state->prev_keycodes_index == 0);
    assert(keyboard_state->are_keycodes_changed(KCT_KEY));
    assert(keyboard_state->are_keycodes_changed(KCT_CC_KEY));
    cout << "ok" << endl;
}


void test_keyboard_state__keycodes_not_changed__prev_keycodes_not_empty_mixed_type() {
    auto keyboard_state = new KeyboardState<3>{};
    cout << "KeyboardState keycodes_changed prev keycodes not empty, keys mixed" << endl;
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_CC_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));
    keyboard_state->reset();
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_CC_KEY}));
    assert(!keyboard_state->are_keycodes_changed(KCT_KEY));
    assert(!keyboard_state->are_keycodes_changed(KCT_CC_KEY));
    cout << "ok" << endl;
}


void test_keyboard_state__keycodes_changed__current_keycodes_empty_mixed_type() {
    auto keyboard_state = new KeyboardState<3>{};
    cout << "KeyboardState keycodes_changed current keycodes empty, keys mixed" << endl;
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_CC_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));
    keyboard_state->reset();
    assert(keyboard_state->are_keycodes_changed(KCT_KEY));
    assert(keyboard_state->are_keycodes_changed(KCT_CC_KEY));
    cout << "ok" << endl;
}


void test_keyboard_state__get_keycodes() {
    auto keyboard_state = new KeyboardState<3>{};
    cout << "KeyboardState keycodes_changed current keycodes empty, keys mixed" << endl;
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{10, KCT_CC_KEY}));
    assert(keyboard_state->emplace_keycode(Keycode{20, KCT_KEY}));

    auto keycodes = keyboard_state->get_keycodes(KCT_KEY);
    assert(keycodes.size() == 2);
    assert(keycodes[0] == 10);
    assert(keycodes[1] == 20);

    auto cc_keycodes = keyboard_state->get_keycodes(KCT_CC_KEY);
    assert(cc_keycodes.size() == 1);
    assert(cc_keycodes[0] == 10);

    auto internal_keycodes = keyboard_state->get_keycodes(KCT_INTERNAL);
    assert(internal_keycodes.empty());
    cout << "ok" << endl;
}
