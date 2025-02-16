#pragma once

#include <limits>
#include <map>
#include <algorithm>
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/keyboard/mapping_rule.hpp"
#include "mkeybo/components/keyboard/mapping_rules/tap_dance_mapping_rule_settings.hpp"


namespace mkeybo::keyboard::mapping_rule {

constexpr uint8_t tap_dance_hold_action = std::numeric_limits<uint8_t>::max();

template <size_t switches_count, size_t keycodes_buffer_size>
class TapDanceMappingRule final : public MappingRule<switches_count, keycodes_buffer_size>
{
    std::map<Keycode, std::map<uint8_t, Keycode>> actions;

public:
    [[nodiscard]] std::string get_settings_name() const override
    {
        return rule_name_tap_dance;
    };

    void apply_settings(const std::map<std::string, MappingRuleSettings*>& rule_settings) override
    {
        if (const auto settings_it = rule_settings.find(get_settings_name()); settings_it != rule_settings.end())
        {
            const auto settings = reinterpret_cast<TapDanceRuleSettings*>(settings_it->second);
            actions = settings->actions;
        }
    }

    bool map(Keyboard<switches_count, keycodes_buffer_size>* keyboard) override
    {
        if (!actions.empty())
        {
            for (auto& keycode_event : keyboard->get_filtered_keycode_events())
            {
                if (auto const action = actions.find(keycode_event.keycode); action != actions.end())
                {
                    map_event(keyboard, keycode_event, action->second);
                }
            }
        }
        return false;
    }

    void map_event(Keyboard<switches_count, keycodes_buffer_size>* keyboard, KeycodeEvent& keycode_event,
                   const std::map<uint8_t, Keycode>& actions)
    {
        auto switch_event = keyboard->get_switch_events()[keycode_event.switch_no];
        // finalize keycode at end of tapdance
        if (switch_event.type == SwitchEventType::tap_dance_end)
        {
            if (auto const action = actions.find(switch_event.tap_dance); action != actions.end())
            {
                keycode_event.keycode = action->second;
            }
            keycode_event.type = KeycodeEventType::finalized;
            keycode_event.priority = KeycodeEventPriority::high;
            return;
        }
        const auto hold_action = actions.find(tap_dance_hold_action);
        const auto has_hold_action = hold_action != actions.end();
        // pressed
        if (switch_event.type == SwitchEventType::pressed)
        {
            if (switch_event.hold)
            {
                if (has_hold_action)
                {
                    keycode_event.keycode = hold_action->second;
                }
                keycode_event.type = KeycodeEventType::finalized;
                keycode_event.priority = KeycodeEventPriority::high;
                return;
            }
            // wait for hold or tap_dance_end, or TODO: other key_pressed
            keycode_event.type = KeycodeEventType::canceled;
            return;
        }
        if (switch_event.type == SwitchEventType::released)
        {
            bool has_only_hold_action = has_hold_action && actions.size() == 1;
            if (is_other_key_pressed(keyboard, keycode_event) || has_only_hold_action)
            {
                keycode_event.type = KeycodeEventType::finalized;
                keycode_event.priority = KeycodeEventPriority::high;
                return;
            }
            keycode_event.type = KeycodeEventType::canceled;
        }
        // just pass this keycode event
    }

    bool is_other_key_pressed(Keyboard<switches_count, keycodes_buffer_size>* keyboard, KeycodeEvent& keycode_event)
    {
        return std::ranges::any_of(
            keyboard->get_filtered_keycode_events(std::nullopt, KeycodeEventType::finalized),
            [&](auto& k_e) { return k_e != keycode_event; });
    }
};

}
