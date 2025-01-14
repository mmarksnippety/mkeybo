#pragma once

#include "../keyboard_settings.hpp"
#include "../keyboard_state.hpp"
#include "../base.hpp"
#include "base_mapping_rule.hpp"


namespace mkeybo::key_mapper {

template <size_t switches_count>
class ChangeLayoutMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        bool layout_changed = false;
        for (auto& keycode_event : keyboard_state->get_filtered_keycode_events_draft(KeycodeType::layout))
        {
            if (keyboard_state->get_active_layout() != keycode_event.keycode.code)
            {
                keyboard_state->set_active_layout(keycode_event.keycode.code);
                keycode_event.type = KeycodeEventType::canceled;
                layout_changed = true;
            }
        }
        return layout_changed;
    }
};

}
