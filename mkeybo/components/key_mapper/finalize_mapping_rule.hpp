#pragma once

#include "../keyboard_settings.hpp"
#include "../keyboard_state.hpp"
#include "../base.hpp"
#include "base_mapping_rule.hpp"


namespace mkeybo::key_mapper {

template <size_t switches_count>
class FinalizeMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        for (auto& keycode_event : keyboard_state->get_filtered_keycode_events_draft())
        {
            conditionally_finalize_keycode_event(keyboard_state, keycode_event);
        }
        return false;
    }

    void conditionally_finalize_keycode_event(KeyboardState<switches_count>* keyboard_state,
                                              KeycodeEvent& keycode_event)
    {
        if (keycode_event.type != KeycodeEventType::draft)
        {
            return;
        }
        if (const auto& switch_event = keyboard_state->get_switch_events()[keycode_event.switch_no]; switch_event.type
            == SwitchEventType::pressed)
        {
            this->finalize_keycode_event(keyboard_state, keycode_event);
        }
    }
};

}
