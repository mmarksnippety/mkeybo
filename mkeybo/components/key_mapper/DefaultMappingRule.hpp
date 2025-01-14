#pragma once

#include "../KeyboardSettings.hpp"
#include "../KeyboardState.hpp"
#include "../base.hpp"
#include "BaseMappingRule.hpp"


namespace mkeybo::key_mapper {


/**
 * Map SwitchEvent to KeycodeEvent
 *
 * SwitchEventType are two types. To mapping and not to mapping
 * "To mapping": pressed, tap_dance_end
 * "Not to mapping": release, idle
 *
 */
template <size_t switches_count>
class DefaultMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        const auto& switch_events = keyboard_state->get_switch_events();
        for (size_t switch_index = 0; switch_index < switch_events.size(); switch_index++)
        {
            if (const auto& switch_event = switch_events[switch_index];
                switch_event.type != SwitchEventType::idle)
            {
                if (auto keycode = get_keycode(keyboard_settings, keyboard_state, switch_index);
                    !keycode.is_null() && !keycode.is_sentinel())
                {
                    keyboard_state->push_keycode_event_draft(keycode, switch_index);
                }
            }
        }
        return false;
    }

    Keycode get_keycode(KeyboardSettings<switches_count>* keyboard_settings,
                        KeyboardState<switches_count>* keyboard_state, const uint8_t switch_index)
    {
        // get from layers
        for (int8_t layer_index = keyboard_settings->layers.size() - 1; layer_index >= 0; --layer_index)
        {
            if (keyboard_state->is_layer_active(layer_index))
            {
                if (auto keycode = keyboard_settings->layers[layer_index]->keycodes[switch_index]; !keycode.is_null())
                {
                    return keycode;
                }
            }
        }
        // get from layout
        return keyboard_settings->layouts[keyboard_state->get_active_layout()]->keycodes[switch_index];
    }
};

}
