#pragma once

#include <limits>
#include <map>
#include "../keyboard_settings.hpp"
#include "../keyboard_state.hpp"
#include "../base.hpp"
#include "base_mapping_rule.hpp"


namespace mkeybo::key_mapper {


constexpr uint8_t tap_dance_hold_action = std::numeric_limits<uint8_t>::max();

template <size_t switches_count>
class TapDanceMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        auto rule_settings = keyboard_settings->rules.find("tap_dance");
        if (rule_settings == keyboard_settings->rules.end())
        {
            return false;
        }
        const auto tap_dance_settings = reinterpret_cast<KeyboardSettingsTapDanceRuleConfig*>(rule_settings->second);
        for (auto& keycode_event : keyboard_state->get_all_keycode_events_draft())
        {
            if (auto const actions = tap_dance_settings->actions.find(keycode_event.keycode);
                actions != tap_dance_settings->actions.end())
            {
                map_event(keyboard_state, keycode_event, actions->second);
            }
        }
        return false;
    }

    void map_event(KeyboardState<switches_count>* keyboard_state, KeycodeEvent& keycode_event,
                   const std::map<uint8_t, Keycode>& actions)
    {
        auto switch_event = keyboard_state->get_switch_events()[keycode_event.switch_no];
        // emit keycode at end of tapdance
        if (switch_event.type == SwitchEventType::tap_dance_end)
        {
            if (auto const action = actions.find(switch_event.tap_dance); action != actions.end())
            {
                keycode_event.keycode = action->second;
            }
            this->finalize_keycode_event(keyboard_state, keycode_event);
            return;
        }
        // pressed
        if (switch_event.type == SwitchEventType::pressed)
        {
            if (const auto hold_action = actions.find(tap_dance_hold_action); hold_action != actions.end())
            {
                // this key has configured hold...
                if (switch_event.hold)
                {
                    keycode_event.keycode = hold_action->second;
                    return;
                }
                // wait for hold!
                keycode_event.type = KeycodeEventType::canceled;
            }
        }
        // just pass this keycode event
    }
};


}
