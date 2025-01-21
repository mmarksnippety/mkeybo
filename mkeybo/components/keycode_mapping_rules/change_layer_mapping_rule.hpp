#pragma once

#include "../base.hpp"
#include "base_mapping_rule.hpp"


namespace mkeybo::keycode_mapping_rule {

template <size_t switches_count, size_t keycodes_buffer_size>
class ChangeLayerMappingRule final : public BaseMappingRule<switches_count, keycodes_buffer_size>
{
public:
    bool map(Keyboard<switches_count, keycodes_buffer_size>* keyboard) override
    {
        bool layer_changed = false;
        for (auto& keycode_event : keyboard->get_filtered_keycode_events(KeycodeType::layer))
        {
            if (!keyboard->is_layer_active(keycode_event.keycode.code))
            {
                keyboard->set_active_layer(keycode_event.keycode.code);
                layer_changed = true;
            }
            keycode_event.type = KeycodeEventType::canceled;
        }
        return layer_changed;
    }
};

}
