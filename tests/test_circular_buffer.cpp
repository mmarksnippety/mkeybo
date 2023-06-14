#include <iostream>
#include "pico/stdlib.h"

#include "components/CircularBuffer.h"


void test_circular_buffer() {
    CircularBuffer<uint8_t, 5> buffer;

    uint8_t el;
    bool success;

    cout << ". Add 2 elements, check size" << endl;
    el = 10;
    buffer.add_element(el);
    el = 11;
    buffer.add_element(el);
    assert(buffer.size() == 2);

    cout << ". Remove 3 elements, check if false on 3 get" << endl;
    success = buffer.remove_element(el);
    assert(success == true);
    assert(el == 10);
    success = buffer.remove_element(el);
    assert(success == true);
    assert(el == 11);
    success = buffer.remove_element(el);
    assert(success == false);
    assert(el == 11);  // el must be not changed

    cout << ". Add more elements than size, check false on overflow" << endl;
    for (auto index = 0; index < buffer.max_size(); index++) {
        el = 2;
        success = buffer.add_element(el);
        assert(success == true);
    }
    el = 3;
    success = buffer.add_element(el);
    assert(success == false);


}

void test_circular_buffer_circular_rw() {
    CircularBuffer<uint8_t, 5> buffer;
    assert(buffer.max_size() == 5);

    cout << ". Write and read circular" << endl;
    array<uint8_t, 4> els = {5, 6, 7, 8};
    bool success;
    uint8_t el;

    for (auto index = 0; index < 10; index++) {
        cout << ". cycle=" << to_string(index) << endl;
        for (auto e: els) {
            success = buffer.add_element(e);
            assert(success == true);
        }
        do {
            success = buffer.remove_element(el);
        } while (success);
        assert(buffer.empty() == true);
    }
    cout << ".  ok" << endl;
}