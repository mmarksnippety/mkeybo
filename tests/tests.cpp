#include <iostream>
#include "pico/stdlib.h"

//#include "test_keyboard_mapper.cpp"
//#include "test_keyboard_matrix.cpp"
#include "test_circular_buffer.cpp"
#include "test_switches_state_updater.cpp"


using namespace std;


int main() {
    stdio_init_all();
    cout << endl << endl;
    cout << "Tests" << endl;
    cout << endl << endl;
//    test_keyboard_matrix_1();
//    test_keyboard_matrix_2();
    test_circular_buffer();
    test_circular_buffer_circular_rw();
    test_circular_buffer_pointers();
    test_switches_state_updater_empty();
    test_switches_state_updater_key_pressed();
    test_switches_state_updater_tap_dance_count_2();
    cout << "================" << endl;
}
