#pragma once

#include "../keyboard_settings.hpp"
#include "../keyboard_rule_settings/multi_mapping_rule_settings.hpp"
#include "base_mapping_rule.hpp"


namespace mkeybo::mapping_rule {

template <size_t switches_count, size_t keycodes_buffer_size>
class MultiMappingRule final : public BaseMappingRule<switches_count, keycodes_buffer_size>
{

public:
    bool map(Keyboard<switches_count, keycodes_buffer_size>* keyboard) override
    {
        const auto multi_settings = get_rule_settings<keyboard_rule_settings::MultiMappingRuleSettings>(
            keyboard->get_settings(), keyboard_rule_settings::rule_name_multi_mapping);
        if (!multi_settings)
        {
            return false;
        }
        for (const auto& [src_keycodes, keycode] : multi_settings->actions)
        {
            if (auto src_keycodes_events = get_src_keycode_event(keyboard, src_keycodes); src_keycodes_events.
                size() >= src_keycodes.size())
            {
                cancel_src_keycodes_events(src_keycodes_events);
                keyboard->push_keycode_event(keycode, std::numeric_limits<uint8_t>::max(),
                                                   KeycodeEventType::finalized);
                return false;
            }
        }
        return false;
    }

    std::vector<KeycodeEvent*> get_src_keycode_event(Keyboard<switches_count, keycodes_buffer_size>* keyboard,
                                                     const std::vector<Keycode>& src_keycodes)
    {
        std::vector<KeycodeEvent*> src_keycodes_events;
        for (const auto& src_keycode : src_keycodes)
        {
            auto keyboard_events_view = keyboard->get_filtered_keycode_events(src_keycode.type);
            auto src_keycode_event = std::ranges::find_if(keyboard_events_view, [&](const auto& event)
            {
                return event.keycode.code == src_keycode.code;
            });
            if (src_keycode_event != keyboard_events_view.end())
            {
                src_keycodes_events.push_back(&(*src_keycode_event));
            }
        }
        return src_keycodes_events;
    }

    void cancel_src_keycodes_events(const std::vector<KeycodeEvent*>& src_keycodes_events)
    {
        std::ranges::for_each(src_keycodes_events, [](auto& keycode_event)
        {
            keycode_event->type = KeycodeEventType::canceled;
        });
    }
};

}
