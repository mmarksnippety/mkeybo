#pragma once

#include <any>
#include <limits>
#include <map>
#include "KeyboardSettings.hpp"
#include "KeyboardState.hpp"
#include "base.hpp"


namespace mkeybo {

template <size_t switches_count>
class BaseMappingRule
{

public:
    BaseMappingRule() = default;
    virtual ~BaseMappingRule() = default;

    /**
     * Method map may map some switches to keycode or remap some keycode to other keycode,
     * or event delete some mapped keycode.
     *
     * If return true, then mapping process must be restarting. Is usefully ex for layer changing.
     */
    virtual bool map(KeyboardSettings<switches_count>* keyboard_settings, KeyboardState<switches_count>* keyboard_state)
    {
        return false;
    }
};

/**
 * Map SwitchEvent to KeycodeEvent
 *
 * SwitchEventType are two types. To mapping and not to mapping
 * "To mapping": pressed, tap_dance_end
 * "Not to mapping": release, idle
 *
 */
template <size_t switches_count>
class DefaultMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        for (size_t switch_index = 0; switch_index < keyboard_state->switch_events.size(); switch_index++)
        {
            if (const auto& switch_event = keyboard_state->switch_events[switch_index];
                switch_event.event_type == SwitchEventType::pressed ||
                switch_event.event_type == SwitchEventType::tap_dance_end)
            {
                if (auto keycode = get_keycode(keyboard_settings, keyboard_state, switch_index);
                    !keycode.is_null() && !keycode.is_sentinel())
                {
                    keyboard_state->keycode_events.push(keycode, switch_index,
                                                        switch_event.event_type == SwitchEventType::pressed
                                                            ? KeycodeEventType::finalized
                                                            : KeycodeEventType::draft);
                }
            }
        }
        return false;
    }

    Keycode get_keycode(KeyboardSettings<switches_count>* keyboard_settings,
                        KeyboardState<switches_count>* keyboard_state, const uint8_t switch_index)
    {
        // get from layers
        for (int8_t layer_index = keyboard_settings->layers.size() - 1; layer_index >= 0; --layer_index)
        {
            if (keyboard_state->is_layer_active(layer_index))
            {
                if (auto keycode = keyboard_settings->layers[layer_index]->keycodes[switch_index]; !keycode.is_null())
                {
                    return keycode;
                }
            }
        }
        // get from layout
        return keyboard_settings->layouts[keyboard_state->get_active_layout()]->keycodes[switch_index];
    }
};

template <size_t switches_count>
class ChangeLayerMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        // IDEA: maybe here is good place to reset layers state? not in KeyboardState?
        bool layer_changed = false;
        for (auto& keycode_event : keyboard_state->keycode_events.get_finalized_events(KeycodeType::layer))
        {
            if (!keyboard_state->is_layer_active(keycode_event.keycode.code))
            {
                keyboard_state->set_active_layer(keycode_event.keycode.code);
                layer_changed = true;
            }
        }
        return layer_changed;
    }
};

template <size_t switches_count>
class ChangeLayoutMappingRule final : public BaseMappingRule<switches_count>
{
public:
    bool map(KeyboardSettings<switches_count>* keyboard_settings,
             KeyboardState<switches_count>* keyboard_state) override
    {
        bool layout_changed = false;
        for (auto& keycode_event : keyboard_state->keycode_events.get_finalized_events(KeycodeType::layout))
        {
            if (keyboard_state->get_active_layout() != keycode_event.keycode.code)
            {
                keyboard_state->set_active_layout(keycode_event.keycode.code);
                layout_changed = true;
            }
        }
        return layout_changed;
    }
};


constexpr uint8_t tap_dance_hold_action = std::numeric_limits<uint8_t>::max();

// TODO make settings: other_keycode_breaks_tap_dance
// is okej when just typing, and just want to prest tap_dance_button,
// this must breaks only tap_dance_end, just like hold
// maybe this is okey always, but without hold?
// To make this behavior, to KeyMaper stage must go released buttons, that have tap_dance, and release > 0
// end what about mapping order? is this must be early than normal mapping?
// mayby tap dance must be after simple mapping, next tap dance, next finalize???
// Make some graph!!!!!
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
        for (auto& keycode_event : keyboard_state->keycode_events.get_all_events())
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
        auto switch_event = keyboard_state->switch_events[keycode_event.switch_no];
        // emit keycode at end of tapdance
        if (switch_event.event_type == SwitchEventType::tap_dance_end)
        {
            if (auto const action = actions.find(switch_event.tap_dance); action != actions.end())
            {
                keycode_event.keycode = action->second;
            }
            keycode_event.type = KeycodeEventType::finalized;
            return;
        }
        // pressed
        if (switch_event.event_type == SwitchEventType::pressed)
        {
            // pressed and hold. i we don't have mapping for hold, then just do nothing
            if (switch_event.hold)
            {
                if (const auto action = actions.find(tap_dance_hold_action); action != actions.end())
                {
                    keycode_event.keycode = action->second;
                    return;
                }
            }
            keycode_event.type = KeycodeEventType::canceled;
        }
    }
};


template <size_t switches_count>
class KeyMapper
{
    std::vector<BaseMappingRule<switches_count>*> rules_;

public:
    explicit KeyMapper(const std::vector<BaseMappingRule<switches_count>*>& rules) : rules_(rules) {}
    ~KeyMapper() { delete_rules(); };

    void add_rule(BaseMappingRule<switches_count>* rule) { rules_.push_back(rule); }

    void delete_rules()
    {
        for (auto rule : rules_)
        {
            delete rule;
        }
        rules_.clear();
    }

    void map(KeyboardSettings<switches_count>* keyboard_settings, KeyboardState<switches_count>* keyboard_state)
    {
        bool mapped = false;
        while (!mapped)
        {
            keyboard_state->keycode_events.reset();
            mapped = true;
            for (auto rule : rules_)
            {
                if (rule->map(keyboard_settings, keyboard_state))
                {
                    mapped = false;
                    break;
                }
            }
        }
    }
};

} // namespace mkeybo
