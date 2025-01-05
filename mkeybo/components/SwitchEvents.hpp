#pragma once

#include <cstdint>
#include <limits>
#include "KeyboardConfig.hpp"
#include "KeyboardState.hpp"


namespace mkeybo {


template <uint8_t switches_count>
class SwitchEventsGenerator {
public:
    SwitchEventsGenerator() = default;
    ~SwitchEventsGenerator() = default;

    void reset_state(KeyboardConfig* keyboard_config, KeyboardState<switches_count>* keyboard_state) {
        for (uint8_t switch_no = 0; switch_no < switches_count; switch_no++) {
            keyboard_state->switch_events[switch_no].event_type = SwitchEventType::idle;
        }
    }

    void update_events(KeyboardConfig* keyboard_config, KeyboardState<switches_count>* keyboard_state) {
        for (uint8_t switch_no = 0; switch_no < switches_count; switch_no++) {
            update_event(switch_no, keyboard_config, keyboard_state);
        }
    }

    // to musi być dobrze przetestowane!!!

    void update_event(uint8_t switch_no, KeyboardConfig* keyboard_config, KeyboardState<switches_count>* keyboard_state) {
        const bool pressed = keyboard_state->switches_state[switch_no];
        auto& event = keyboard_state->switch_events[switch_no];
        if (pressed) {
            event.event_type = SwitchEventType::pressed;
            if (event.event_type == SwitchEventType::idle) { event.released = 0; }
            if (event.pressed < std::numeric_limits<uint8_t>::max()) { ++event.pressed; }
        } else {
            keyboard_state->switch_events[switch_no].event_type = SwitchEventType::released;
            if (event.event_type == SwitchEventType::idle) { event.pressed = 0; }
            if (event.released < std::numeric_limits<uint8_t>::max()) { ++event.released; }
        }
    }

};


}

