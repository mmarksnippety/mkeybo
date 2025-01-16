#include "test_tap_dance_mapping_rule.hpp"
#include <cassert>
#include <iostream>
#include "fixtures.hpp"
#include "mkeybo/components/keyboard_settings.hpp"
#include "mkeybo/components/keyboard_rule_settings/tap_dance_rule_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/components/key_mapper.hpp"
#include "mkeybo/debug.hpp"


void test_tap_dance_mapping_rule_map_event_tap_dance_end()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    std::map<uint8_t, mkeybo::Keycode> tap_dance_map{{2, H_K(10)}};
    k_settings->rules.insert({mkeybo::keyboard_rule_settings::rule_name_tap_dance,
                              new mkeybo::keyboard_rule_settings::TapDanceRuleSettings({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    const auto k_rule = new mkeybo::key_mapper::TapDanceMappingRule<switches_count>();
    auto& switch_events = k_state->get_switch_events();
    std::cout << __func__ << "...";
    switch_events[0].type = mkeybo::SwitchEventType::tap_dance_end;
    switch_events[0].tap_dance = 1;
    k_state->push_keycode_event_draft(H_K(1), 0);
    auto k_event = k_state->get_all_keycode_events_draft().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(1));
    assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    assert(std::ranges::distance(k_state->get_filtered_keycode_events()) == 1);
    const auto finalized_k_event = k_state->get_filtered_keycode_events().front();
    assert(finalized_k_event.keycode == H_K(1));
    assert(finalized_k_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_tap_dance_mapping_rule_map_event_tap_dance_end_with_action()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    std::map<uint8_t, mkeybo::Keycode> tap_dance_map{{2, H_K(10)}};
    k_settings->rules.insert({mkeybo::keyboard_rule_settings::rule_name_tap_dance,
                              new mkeybo::keyboard_rule_settings::TapDanceRuleSettings({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    const auto k_rule = new mkeybo::key_mapper::TapDanceMappingRule<switches_count>();
    auto& switch_events = k_state->get_switch_events();
    std::cout << __func__ << "...";
    switch_events[0].type = mkeybo::SwitchEventType::tap_dance_end;
    switch_events[0].tap_dance = 2;
    k_state->push_keycode_event_draft(H_K(1), 0);
    auto k_event = k_state->get_all_keycode_events_draft().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(10));
    assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    assert(std::ranges::distance(k_state->get_filtered_keycode_events()) == 1);
    const auto finalized_k_event = k_state->get_filtered_keycode_events().front();
    assert(finalized_k_event.keycode == H_K(10));
    assert(finalized_k_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_tap_dance_mapping_rule_map_event_press_hold()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    const std::map<uint8_t, mkeybo::Keycode> tap_dance_map{
        {2, H_K(10)},
        {mkeybo::key_mapper::tap_dance_hold_action, H_K(20)}};
    k_settings->rules.insert({mkeybo::keyboard_rule_settings::rule_name_tap_dance,
                              new mkeybo::keyboard_rule_settings::TapDanceRuleSettings({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    const auto k_rule = new mkeybo::key_mapper::TapDanceMappingRule<switches_count>();
    auto& switch_events = k_state->get_switch_events();
    std::cout << __func__ << "...";
    switch_events[0].type = mkeybo::SwitchEventType::pressed;
    switch_events[0].hold = true;
    k_state->push_keycode_event_draft(H_K(1), 0);
    auto k_event = k_state->get_all_keycode_events_draft().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(20));
    assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    assert(std::ranges::distance(k_state->get_filtered_keycode_events()) == 1);
    const auto finalized_k_event = k_state->get_filtered_keycode_events().front();
    assert(finalized_k_event.keycode == H_K(20));
    assert(finalized_k_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_tap_dance_mapping_rule_map_event_press()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    const std::map<uint8_t, mkeybo::Keycode> tap_dance_map{{2, H_K(10)}, {3, H_K(20)}};
    k_settings->rules.insert({mkeybo::keyboard_rule_settings::rule_name_tap_dance,
                              new mkeybo::keyboard_rule_settings::TapDanceRuleSettings({{H_K(1), tap_dance_map}})});
    k_state->setup_for_settings(k_settings);
    const auto k_rule = new mkeybo::key_mapper::TapDanceMappingRule<switches_count>();
    auto& switch_events = k_state->get_switch_events();
    std::cout << __func__ << "...";
    switch_events[0].type = mkeybo::SwitchEventType::pressed;
    switch_events[0].hold = true;
    k_state->push_keycode_event_draft(H_K(1), 0);
    auto k_event = k_state->get_all_keycode_events_draft().front();
    k_rule->map_event(k_state, k_event, tap_dance_map);
    assert(k_event.keycode == H_K(1));
    assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    const auto finalized_k_event = k_state->get_filtered_keycode_events().front();
    assert(finalized_k_event.keycode == H_K(1));
    assert(finalized_k_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}
