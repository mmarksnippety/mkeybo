#pragma once

#include <bitset>

#include "pico/stdlib.h"

#include "components/KeyboardState.h"


using namespace std;


template <uint8_t switches_count>
class SwitchReader {
protected:
    KeyboardState<switches_count> *keyboard_state_;

public:
    explicit SwitchReader(KeyboardState<switches_count> *keyboard_state)
        : keyboard_state_(keyboard_state) {}

    virtual void read_state() {}

    virtual void init() const {}
};

