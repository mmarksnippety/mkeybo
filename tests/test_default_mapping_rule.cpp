#include "test_default_mapping_rule.hpp"
#include "fixtures.hpp"
#include <cassert>
#include <iostream>
#include <ranges>
#include "../mkeybo/components/keyboard/settings.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/factories.hpp"
#include "../mkeybo/components/key_mapper.hpp"


void test_default_mapping_rule_get_keycode()
{
    std::cout << __func__ << "...";
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_rule = new mkeybo::key_mapper::DefaultMappingRule<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    k_state->setup_for_settings(k_settings);
    // get key code from layout
    auto keycode = k_rule->get_keycode(k_settings, k_state, 1);
    // std::cout << keycode << std::endl;
    assert(keycode == H_K(2));
    // get key code from layer up
    k_state->set_active_layer(1);
    keycode = k_rule->get_keycode(k_settings, k_state, 1);
    // std::cout << keycode << std::endl;
    assert(keycode == H_K(200));
    //
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_default_mapping_rule_map()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_rule = new mkeybo::key_mapper::DefaultMappingRule<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    std::cout << __func__ << "...";
    k_state->setup_for_settings(k_settings);
    // no key pressed - no key events
    auto result = k_rule->map(k_settings, k_state);
    assert(result == false);
    // assert(std::ranges::empty(k_state->keycode_events_.get_finalized_events()));
    // some key pressed
    auto& switch_events = k_state->get_switch_events();
    switch_events[1].type = mkeybo::SwitchEventType::pressed;
    switch_events[1].pressed = 1;
    switch_events[3].type = mkeybo::SwitchEventType::pressed;
    switch_events[3].pressed = 1;
    result = k_rule->map(k_settings, k_state);
    assert(result == false);
    // check keycode_events_
    const auto expected_keycodes = std::vector{H_K(2), LAYER_K(1)};
    auto keycode_events_view = k_state->get_filtered_keycode_events();
    auto generated_keycodes = std::vector<mkeybo::Keycode>{};
    generated_keycodes.reserve(std::ranges::distance(keycode_events_view));
    for (auto& keycode_event : keycode_events_view)
    {
        generated_keycodes.push_back(keycode_event.keycode);
    }
    assert(generated_keycodes == expected_keycodes);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}
