#pragma once

#include "../keyboard_settings.hpp"
#include "../keyboard_state.hpp"
#include "../base.hpp"
#include "base_mapping_rule.hpp"


namespace mkeybo::key_mapper {

template <size_t switches_count>
class MultiMappingRule final : public BaseMappingRule<switches_count>
{

public:
    virtual bool map(KeyboardSettings<switches_count>* keyboard_settings, KeyboardState<switches_count>* keyboard_state)
    {
        const auto multi_settings = get_rule_settings<KeyboardSettingsMultiMappingRule>(
            keyboard_settings, keyboard_settings_multi_mapping_rule);
        if (!multi_settings)
        {
            return false;
        }
        return false;
    }

};

}
