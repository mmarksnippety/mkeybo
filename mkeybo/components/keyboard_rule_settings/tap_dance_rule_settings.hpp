#pragma once

#include "base_rule_settings.hpp"
#include "../base.hpp"
#include <map>


namespace mkeybo::keyboard_rule_settings {

inline std::string rule_name_tap_dance = "tap_dance";

struct TapDanceRuleSettings : BaseRuleSettings
{
    std::map<Keycode, std::map<uint8_t, Keycode>> actions;

    explicit TapDanceRuleSettings(const std::map<Keycode, std::map<uint8_t, Keycode>>& actions) :
        actions{actions}
    {
    }
};

}