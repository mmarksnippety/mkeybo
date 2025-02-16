#pragma once

#include "mkeybo/components/keyboard/mapping_rule.hpp"
#include <vector>


namespace mkeybo::keyboard::mapping_rule  {

inline std::string rule_name_multi_mapping = "multi_mapping";


struct MultiMappingRuleSettings : MappingRuleSettings
{
    std::vector<std::pair<std::vector<Keycode>, Keycode>> actions;

    explicit MultiMappingRuleSettings(const std::vector<std::pair<std::vector<Keycode>, Keycode>>& actions) :
        actions{actions}
    {
    }
};


}