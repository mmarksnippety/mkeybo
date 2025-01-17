#pragma once

#include <iostream>
#include <ostream>

#include "../keyboard_settings.hpp"
#include "../keyboard_rule_settings/multi_mapping_rule_settings.hpp"
#include "../keyboard_state.hpp"
#include "base_mapping_rule.hpp"
#include "mkeybo/debug.hpp"


namespace mkeybo::key_mapper {

template <size_t switches_count>
class MultiMappingRule final : public BaseMappingRule<switches_count>
{

public:
    virtual bool map(KeyboardSettings<switches_count>* keyboard_settings, KeyboardState<switches_count>* keyboard_state)
    {
        const auto multi_settings = get_rule_settings<keyboard_rule_settings::MultiMappingRuleSettings>(
            keyboard_settings, keyboard_rule_settings::rule_name_multi_mapping);
        if (!multi_settings)
        {
            return false;
        }
        for (const auto& [src_keycodes, keycode] : multi_settings->actions)
        {
            if (auto src_keycodes_events = get_src_keycode_event(keyboard_state, src_keycodes); src_keycodes_events.
                size() == src_keycodes.size())
            {
                std::cout <<  "Multi rule for keycode: " << keycode << std::endl;
                cancel_src_keycodes_events(src_keycodes_events);
                this->publish_keycode_event(keyboard_state, keycode);
                return false;
            }
        }
        return false;
    }

    std::vector<KeycodeEvent*> get_src_keycode_event(KeyboardState<switches_count>* keyboard_state,
                                                    const std::vector<Keycode>& src_keycodes)
    {
        std::vector<KeycodeEvent*> src_keycodes_events;
        for (const auto& src_keycode : src_keycodes)
        {
            // get from finalized
            auto keyboard_events_view = keyboard_state->get_filtered_keycode_events(src_keycode.type);
            auto src_keycode_event = std::ranges::find_if(keyboard_events_view, [&](const auto& event)
            {
                return event.keycode.code == src_keycode.code;
            });
            if (src_keycode_event != keyboard_events_view.end())
            {
                src_keycodes_events.push_back(&(*src_keycode_event));
            }
            // get from drafts
            keyboard_events_view = keyboard_state->get_filtered_keycode_events_draft(src_keycode.type);
            src_keycode_event = std::ranges::find_if(keyboard_events_view, [&](const auto& event)
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
