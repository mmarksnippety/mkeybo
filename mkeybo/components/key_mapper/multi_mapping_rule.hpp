#pragma once

#include "../keyboard_settings.hpp"
#include "../keyboard_rule_settings/multi_rule_settings.hpp"
#include "../keyboard_state.hpp"
#include "base_mapping_rule.hpp"


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
        return false;
    }

};

}
