#pragma once

#include "../KeyboardSettings.hpp"
#include "../KeyboardState.hpp"
#include "../base.hpp"
#include "BaseMappingRule.hpp"


namespace mkeybo::key_mapper {

template <size_t switches_count>
class FinalizeMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        for (auto& keycode_event : keyboard_state->get_all_keycode_events_draft())
        {
            if (keycode_event.type == KeycodeEventType::draft)
            {
                this->finalize_keycode_event(keyboard_state, keycode_event);
            }
        }
        return false;
    }
};

}
