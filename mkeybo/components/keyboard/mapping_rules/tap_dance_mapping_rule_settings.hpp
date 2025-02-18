#pragma once

#include "mkeybo/components/keyboard/mapping_rule.hpp"
#include <map>


namespace mkeybo::keyboard::mapping_rule {

inline std::string rule_name_tap_dance = "tap_dance";

struct TapDanceMappingRuleSettings : MappingRuleSettings
{
    std::map<Keycode, std::map<uint8_t, Keycode>> actions;

    explicit TapDanceMappingRuleSettings(const std::map<Keycode, std::map<uint8_t, Keycode>>& actions) :
        actions{actions}
    {
    }
};

}