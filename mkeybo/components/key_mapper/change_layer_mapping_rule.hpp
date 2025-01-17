#pragma once

#include "../keyboard_settings.hpp"
#include "../keyboard_state.hpp"
#include "../base.hpp"
#include "base_mapping_rule.hpp"
#include <ranges>

namespace mkeybo::key_mapper {

template <size_t switches_count>
class ChangeLayerMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        bool layer_changed = false;
        for (auto& keycode_event : keyboard_state->get_filtered_keycode_events(KeycodeType::layer))
        {
            if (!keyboard_state->is_layer_active(keycode_event.keycode.code))
            {
                keyboard_state->set_active_layer(keycode_event.keycode.code);
                layer_changed = true;
            }
            keycode_event.type = KeycodeEventType::canceled;
        }
        return layer_changed;
    }
};

}
