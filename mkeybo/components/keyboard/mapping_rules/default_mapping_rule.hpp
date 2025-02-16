#pragma once

#include "mkeybo/components/base.hpp"
#include "mkeybo/components/keyboard/mapping_rule.hpp"


namespace mkeybo::keyboard::mapping_rule {


/**
 * Map SwitchEvent to KeycodeEvent
 *
 * SwitchEventType are two types. To mapping and not to mapping
 * "To mapping": pressed, tap_dance_end
 * "Not to mapping": release, idle
 *
 */
template <size_t switches_count, size_t keycodes_buffer_size>
class DefaultMappingRule final : public MappingRule<switches_count, keycodes_buffer_size>
{
public:
    bool map(Keyboard<switches_count, keycodes_buffer_size>* keyboard) override
    {
        const auto& switch_events = keyboard->get_switch_events();
        for (size_t switch_index = 0; switch_index < switch_events.size(); switch_index++)
        {
            if (const auto& switch_event = switch_events[switch_index];
                switch_event.type != SwitchEventType::idle)
            {
                if (auto keycode = get_keycode(keyboard, switch_index);
                    !keycode.is_null() && !keycode.is_sentinel())
                {
                    auto keycode_event_type = switch_event.type == SwitchEventType::pressed
                        ? KeycodeEventType::finalized
                        : KeycodeEventType::draft;
                    keyboard->push_keycode_event(keycode, switch_index, keycode_event_type);
                }
            }
        }
        return false;
    }

    Keycode get_keycode(Keyboard<switches_count, keycodes_buffer_size>* keyboard, const uint8_t switch_index)
    {
        // get from layers
        auto& layers = keyboard->get_layers();
        for (int8_t layer_index = layers.size() - 1; layer_index >= 0; --layer_index)
        {
            if (keyboard->is_layer_active(layer_index))
            {
                if (auto keycode = layers[layer_index]->keycodes[switch_index]; !keycode.is_null())
                {
                    return keycode;
                }
            }
        }
        // get from layout
        auto& layouts = keyboard->get_layouts();
        return layouts[keyboard->get_active_layout()]->keycodes[switch_index];
    }
};

}
