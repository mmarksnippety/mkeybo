#include "test_tap_dance_mapping_rule.hpp"
#include <cassert>
#include <iostream>
#include <ranges>
#include "fixtures.hpp"
#include "mkeybo/components/KeyboardSettings.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/debug.hpp"


void test_tap_dance_mapping_rule_map_event_tap_dance_end()
{
    constexpr auto switches_count = 4;
    auto k_state = new mkeybo::KeyboardState<switches_count>();
    auto k_settings = k_settings_fixture<switches_count>();
    std::map<uint8_t, mkeybo::Keycode> tap_dance_map{{2, H_K(10)}};
    k_settings->rules.insert({"tap_dance", new mkeybo::KeyboardSettingsTapDanceRuleConfig({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    auto k_rule = new mkeybo::TapDanceMappingRule<switches_count>();
    std::cout << __func__ << "...";
    k_state->switch_events[0].event_type = mkeybo::SwitchEventType::tap_dance_end;
    k_state->switch_events[0].tap_dance = 1;
    k_state->keycode_events.push(H_K(1), 0, mkeybo::KeycodeEventType::draft);
    auto k_event = k_state->keycode_events.get_all_events().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(1));
    assert(k_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_tap_dance_mapping_rule_map_event_tap_dance_end_with_action()
{
    constexpr auto switches_count = 4;
    auto k_state = new mkeybo::KeyboardState<switches_count>();
    auto k_settings = k_settings_fixture<switches_count>();
    std::map<uint8_t, mkeybo::Keycode> tap_dance_map{{2, H_K(10)}};
    k_settings->rules.insert({"tap_dance", new mkeybo::KeyboardSettingsTapDanceRuleConfig({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    auto k_rule = new mkeybo::TapDanceMappingRule<switches_count>();
    std::cout << __func__ << "...";
    k_state->switch_events[0].event_type = mkeybo::SwitchEventType::tap_dance_end;
    k_state->switch_events[0].tap_dance = 2;
    //TODO: test refrence!!!! (regresion test!)
    k_state->keycode_events.push(H_K(1), 0, mkeybo::KeycodeEventType::draft);
    auto k_event = k_state->keycode_events.get_all_events().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(10));
    assert(k_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_tap_dance_mapping_rule_map_event_press_hold()
{
    constexpr auto switches_count = 4;
    auto k_state = new mkeybo::KeyboardState<switches_count>();
    auto k_settings = k_settings_fixture<switches_count>();
    const std::map<uint8_t, mkeybo::Keycode> tap_dance_map{{2, H_K(10)}, {mkeybo::tap_dance_hold_action, H_K(20)}};
    k_settings->rules.insert({"tap_dance", new mkeybo::KeyboardSettingsTapDanceRuleConfig({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    auto k_rule = new mkeybo::TapDanceMappingRule<switches_count>();
    std::cout << __func__ << "...";
    k_state->switch_events[0].event_type = mkeybo::SwitchEventType::pressed;
    k_state->switch_events[0].hold = true;
    k_state->keycode_events.push(H_K(1), 0, mkeybo::KeycodeEventType::finalized);
    auto k_event = k_state->keycode_events.get_all_events().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(20));
    assert(k_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_tap_dance_mapping_rule_map_event_press()
{
    constexpr auto switches_count = 4;
    auto k_state = new mkeybo::KeyboardState<switches_count>();
    auto k_settings = k_settings_fixture<switches_count>();
    const std::map<uint8_t, mkeybo::Keycode> tap_dance_map{{2, H_K(10)}, {3, H_K(20)}};
    k_settings->rules.insert({"tap_dance", new mkeybo::KeyboardSettingsTapDanceRuleConfig({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    const auto k_rule = new mkeybo::TapDanceMappingRule<switches_count>();
    std::cout << __func__ << "...";
    k_state->switch_events[0].event_type = mkeybo::SwitchEventType::pressed;
    k_state->switch_events[0].hold = true;
    k_state->keycode_events.push(H_K(1), 0, mkeybo::KeycodeEventType::finalized);
    auto k_event = k_state->keycode_events.get_all_events().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(1));
    assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}
