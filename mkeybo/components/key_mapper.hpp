#pragma once

#include "keyboard_settings.hpp"
#include "keyboard_state.hpp"
#include "key_mapper/base_mapping_rule.hpp"


namespace mkeybo {

template <size_t switches_count>
class KeyMapper
{
    std::vector<key_mapper::BaseMappingRule<switches_count>*> rules_;

public:
    explicit KeyMapper(const std::vector<key_mapper::BaseMappingRule<switches_count>*>& rules) :
        rules_(rules)
    {
    }

    ~KeyMapper()
    {
        for (auto rule : rules_)
        {
            delete rule;
        }
    };

    void map(KeyboardSettings<switches_count>* keyboard_settings, KeyboardState<switches_count>* keyboard_state)
    {
        bool mapped = false;
        keyboard_state->reset_active_layers();
        while (!mapped)
        {
            keyboard_state->reset_keycode_events();
            keyboard_state->reset_keycode_events_draft();
            mapped = true;
            for (auto rule : rules_)
            {
                if (rule->map(keyboard_settings, keyboard_state))
                {
                    mapped = false;
                    break;
                }
            }
        }
    }
};


}
