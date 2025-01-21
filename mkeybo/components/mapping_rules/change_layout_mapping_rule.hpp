#pragma once

#include "../base.hpp"
#include "base_mapping_rule.hpp"


namespace mkeybo::mapping_rule {

template <size_t switches_count, size_t keycodes_buffer_size>
class ChangeLayoutMappingRule final : public BaseMappingRule<switches_count, keycodes_buffer_size>
{
public:
    bool map(Keyboard<switches_count, keycodes_buffer_size>* keyboard) override
    {
        bool layout_changed = false;
        for (auto& keycode_event : keyboard->get_filtered_keycode_events(KeycodeType::layout))
        {
            if (keyboard->get_active_layout() != keycode_event.keycode.code)
            {
                keyboard->set_active_layout(keycode_event.keycode.code);
                layout_changed = true;
            }
            keycode_event.type = KeycodeEventType::canceled;
        }
        return layout_changed;
    }
};

}
