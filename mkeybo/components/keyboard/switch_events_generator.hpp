#pragma once

#include <cstdint>
#include <limits>
#include "mkeybo/components/base.hpp"


namespace mkeybo::keyboard {

/**
 * SwitchEventGenerator, make a state of switch in timeline.
 */
template <size_t switches_count, size_t keycodes_buffer_size>
class SwitchEventsGenerator
{
public:
    SwitchEventsGenerator() = default;
    ~SwitchEventsGenerator() = default;

    void update(Keyboard<switches_count, keycodes_buffer_size>* keyboard)
    {
        auto& switch_state = keyboard->get_switch_state();
        auto& switch_events = keyboard->get_switch_events();
        for (size_t switch_no = 0; switch_no < switches_count; switch_no++)
        {
            update_switch_event(keyboard, switch_state[switch_no], switch_events[switch_no]);
        }
    }

    void update_switch_event(const Keyboard<switches_count, keycodes_buffer_size>* keyboard, const bool pressed,
                             SwitchEvent& event)
    {
        // pressed, pressed not always mean type pressed, we must check
        if (pressed)
        {
            // counters update
            if (event.released > 0)
            {
                event.type = SwitchEventType::idle;
                event.pressed = 1;
                event.released = 0;
            }
            else
            {
                if (event.pressed < std::numeric_limits<uint8_t>::max()) { ++event.pressed; }
            }
            // idle -> pressed, increase tap dance
            if (event.type == SwitchEventType::idle && event.pressed >= keyboard->get_press_min_interval_cycles())
            {
                event.type = SwitchEventType::pressed;
                ++event.tap_dance;
                return;
            }
            // hold detect. this break tap_dance cycles
            if (event.pressed >= keyboard->get_hold_min_interval_cycles())
            {
                event.hold = true;
                event.tap_dance = 0;
            }
        }
        else
        {
            // change from pressed to released
            if (event.pressed > 0)
            {
                // end of hold is end of everything
                if (event.hold)
                {
                    event.type = SwitchEventType::idle;
                    event.pressed = 0;
                    event.released = 0;
                    event.tap_dance = 0;
                    event.hold = false;
                }
                else
                {
                    event.type = SwitchEventType::released;
                    event.pressed = 0;
                    event.released = 1;
                    event.hold = false;
                }
                return;
            }
            // change to tap_dance_end or idle (after hold no tap_dance_end)
            if (event.type == SwitchEventType::released and event.released >= keyboard->
                get_tap_dance_max_interval_cycles())
            {
                event.type = event.tap_dance > 0 ? SwitchEventType::tap_dance_end : SwitchEventType::idle;
                event.released = 0;
                return;
            }
            // back from tap_dance_end to idle
            if (event.type == SwitchEventType::tap_dance_end)
            {
                event.type = SwitchEventType::idle;
                event.tap_dance = 0;
                return;
            }
            // oke just push the counter
            if (event.released > 0 && event.released < std::numeric_limits<uint8_t>::max()) { ++event.released; }
        }
    }
};


}
