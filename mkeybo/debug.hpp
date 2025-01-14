#pragma once

#include <iostream>
#include "mkeybo/components/base.hpp"


namespace mkeybo {

inline std::string_view get_keycode_type_name(const KeycodeType& type)
{
    switch (type)
    {
    case KeycodeType::null:
        return "null_key";
    case KeycodeType::hid:
        return "hid_key";
    case KeycodeType::cc:
        return "cc_key";
    case KeycodeType::mouse:
        return "mouse";
    case KeycodeType::gamepad:
        return "gamepad";
    case KeycodeType::macro:
        return "macro";
    case KeycodeType::action:
        return "action";
    case KeycodeType::layer:
        return "layer";
    case KeycodeType::layout:
        return "layout";
    case KeycodeType::sentinel:
        return "sentinel";
    default:
        return "UNKNOWN";
    }
}

inline std::string_view get_switch_event_type_name(const SwitchEventType& type)
{
    switch (type)
    {
    case SwitchEventType::idle:
        return "idle";
    case SwitchEventType::released:
        return "released";
    case SwitchEventType::pressed:
        return "pressed";
    case SwitchEventType::tap_dance_end:
        return "tap_dance_end";
    default:
        return "UNKNOWN";
    }
}

inline std::string_view get_keycode_event_type_name(const KeycodeEventType& type)
{
    switch (type)
    {
    case KeycodeEventType::draft:
        return "draft";
    case KeycodeEventType::finalized:
        return "finalized";
    case KeycodeEventType::canceled:
        return "canceled";
    default:
        return "UNKNOWN";
    }
}

} // namespace mkeybo


inline std::ostream& operator<<(std::ostream& out, const mkeybo::Keycode& keycode)
{
    out << mkeybo::get_keycode_type_name(keycode.type) << "|" << keycode.code;
    return out;
}

namespace mkeybo {


inline void print_settings_rules_tap_dance(KeyboardSettingsTapDanceRuleConfig *tap_dance_settings)
{
    for (auto& [trigger_keycode, actions] : tap_dance_settings->actions)
    {
        std::cout << "trigger key: " << trigger_keycode << std::endl;
        for (auto& [counter, action_keycode] : actions)
        {
            std::cout << "    counter|" << std::to_string(counter) << " --> " << action_keycode << std::endl;
        }
    }
}


inline void print_settings_rules(const std::map<std::string, KeyboardSettingsRule*>& rules)
{
    for (auto& [name, settings] : rules)
    {
        std::cout << name << std::endl;
        if (name == "tap_dance")
        {
            const auto tap_dance_settings = reinterpret_cast<KeyboardSettingsTapDanceRuleConfig*>(settings);
            print_settings_rules_tap_dance(tap_dance_settings);
        }
    }
}


}