#pragma once

#include "../KeyboardSettings.hpp"
#include "../KeyboardState.hpp"
#include "BaseMappingRule.hpp"


namespace mkeybo::key_mapper {


template <size_t switches_count>
class KeyMapper
{
    std::vector<BaseMappingRule<switches_count>*> rules_;

public:
    explicit KeyMapper(const std::vector<BaseMappingRule<switches_count>*>& rules) :
        rules_(rules)
    {
    }

    ~KeyMapper() { delete_rules(); };

    void add_rule(BaseMappingRule<switches_count>* rule) { rules_.push_back(rule); }

    void delete_rules()
    {
        for (auto rule : rules_)
        {
            delete rule;
        }
        rules_.clear();
    }

    void map(KeyboardSettings<switches_count>* keyboard_settings, KeyboardState<switches_count>* keyboard_state)
    {
        bool mapped = false;
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
