#pragma once

#include "base_rule_settings.hpp"
#include "../base.hpp"
#include <vector>


namespace mkeybo::keyboard_rule_settings {

inline std::string rule_name_multi_mapping = "multi_mapping";


struct MultiMappingRuleSettings : BaseRuleSettings
{
    std::vector<std::pair<std::vector<Keycode>, Keycode>> actions;

    explicit MultiMappingRuleSettings(const std::vector<std::pair<std::vector<Keycode>, Keycode>>& actions) :
        actions{actions}
    {
    }
};

inline std::string keyboard_settings_multi_mapping_rule = "multi_mapping";

}