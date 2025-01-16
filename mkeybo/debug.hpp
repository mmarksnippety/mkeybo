#pragma once

#include <iostream>
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/keyboard_rule_settings/tap_dance_rule_settings.hpp"


inline std::ostream& operator<<(std::ostream& out, const mkeybo::Keycode& keycode);


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

template <size_t switches_count>
void print_settings(KeyboardSettings<switches_count>* settings)
{
    std::cout << "Settings:" << std::endl;
    std::cout << "switches_refresh_interval_ms: " << std::to_string(settings->switches_refresh_interval_ms) <<
        std::endl;
    std::cout << "press_min_interval_ms: " << std::to_string(settings->press_min_interval_ms) << std::endl;
    std::cout << "tap_dance_max_interval_ms: " << std::to_string(settings->tap_dance_max_interval_ms) << std::endl;
    std::cout << "hold_min_interval_ms: " << std::to_string(settings->hold_min_interval_ms) << std::endl;
    std::cout << "report_send_interval_ms: " << std::to_string(settings->report_send_interval_ms) << std::endl;
    std::cout << "press_min_interval_cycles: " << std::to_string(settings->press_min_interval_cycles) << std::endl;
    std::cout << "tap_dance_max_interval_cycles: " << std::to_string(settings->tap_dance_max_interval_cycles) <<
        std::endl;
    std::cout << "hold_min_interval_cycles: " << std::to_string(settings->hold_min_interval_cycles) << std::endl;
}

inline void print_settings_rules_tap_dance(keyboard_rule_settings::TapDanceRuleSettings* tap_dance_settings)
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

inline void print_settings_rules(std::map<std::string, keyboard_rule_settings::BaseRuleSettings*> rules)
{
    for (auto& [name, settings] : rules)
    {
        std::cout << name << std::endl;
        if (name == "tap_dance")
        {
            const auto tap_dance_settings = reinterpret_cast<keyboard_rule_settings::TapDanceRuleSettings*>(settings);
            print_settings_rules_tap_dance(tap_dance_settings);
        }
    }
}


}

inline std::ostream& operator<<(std::ostream& out, const mkeybo::Keycode& keycode)
{
    out << mkeybo::get_keycode_type_name(keycode.type) << "|" << keycode.code;
    return out;
}
