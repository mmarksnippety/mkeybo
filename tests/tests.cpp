#include <iostream>
#include "pico/stdlib.h"

//#include "test_keyboard_mapper.cpp"
//#include "test_keyboard_matrix.cpp"
#include "test_circular_buffer.cpp"
#include "test_switches_state_updater.cpp"
#include "test_keyboard_state.cpp"


using namespace std;


int main() {
    stdio_init_all();
    cout << endl << endl;
    cout << "Tests" << endl;
    cout << endl << endl;
//    test_keyboard_matrix_1();
//    test_keyboard_matrix_2();

    test_circular_buffer__empty();
    test_circular_buffer();
    test_circular_buffer_circular_rw();
    test_circular_buffer_pointers();

//    test_switches_state_updater_empty();
//    test_switches_state_updater_key_pressed();
//    test_switches_state_updater_tap_dance_count_2();

    test_keyboard_state__emplace_keycode();
    test_keyboard_state__keycode_reset();
    test_keyboard_state__keycodes_changed__empty();
    test_keyboard_state__keycodes_changed__prev_keycodes_empty();
    test_keyboard_state__keycodes_changed__prev_keycodes_empty_mixed_type();
    test_keyboard_state__keycodes_not_changed__prev_keycodes_not_empty_mixed_type();
    test_keyboard_state__keycodes_changed__current_keycodes_empty_mixed_type();
    test_keyboard_state__get_keycodes();
    cout << "================" << endl;
}
