#include "test_key_mapper.hpp"
#include "fixtures.hpp"


void test_key_mapper_map_idle()
{
    constexpr auto switches_count = 4;
    const auto k_settings = k_settings_fixture<switches_count>();
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    k_state->setup_for_settings(k_settings);
    const auto k_mapper = mkeybo::create_key_mapper<switches_count>();
    std::cout << __func__ << "...";
    // idle - no keycode events
    k_mapper->map(k_settings, k_state);
    assert(std::ranges::empty(k_state->get_filtered_keycode_events()));
    delete k_mapper;
    delete k_state;
    delete k_settings;
    std::cout << "PASS" << std::endl;
}


void test_key_mapper_map_simple_key()
{
    constexpr auto switches_count = 4;
    const auto k_settings = k_settings_fixture<switches_count>();
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    k_state->setup_for_settings(k_settings);
    const auto k_mapper = mkeybo::create_key_mapper<switches_count>();
    std::cout << __func__ << "...";
    // switch 0
    auto& switch_events = k_state->get_switch_events();
    switch_events[0].type = mkeybo::SwitchEventType::pressed;
    switch_events[0].pressed = 1;
    k_mapper->map(k_settings, k_state);
    auto view = k_state->get_filtered_keycode_events();
    assert(std::ranges::distance(view) == 1);
    const auto keycode_event = view.front();
    assert(keycode_event.keycode == H_K(1));
    assert(keycode_event.type == mkeybo::KeycodeEventType::finalized);
    delete k_mapper;
    delete k_state;
    delete k_settings;
    std::cout << "PASS" << std::endl;
}

void test_key_mapper_map_with_layer_change()
{
    constexpr auto switches_count = 4;
    const auto k_settings = k_settings_fixture<switches_count>();
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    k_state->setup_for_settings(k_settings);
    const auto k_mapper = mkeybo::create_key_mapper<switches_count>();
    std::cout << __func__ << "...";
    // switch 0 and switch 3 - change layer
    k_state->reset();
    auto& switch_events = k_state->get_switch_events();
    switch_events[0].type = mkeybo::SwitchEventType::pressed;
    switch_events[0].pressed = 1;
    switch_events[3].type = mkeybo::SwitchEventType::pressed;
    switch_events[3].pressed = 1;
    k_mapper->map(k_settings, k_state);
    auto view = k_state->get_filtered_keycode_events();
    auto it = view.begin();
    const auto end = view.end();
    auto keycode_event = *it++;
    assert(keycode_event.keycode == H_K(100));
    keycode_event = *it++;
    assert(keycode_event.keycode == LAYER_K(1));
    assert(it == end);
    delete k_mapper;
    delete k_state;
    delete k_settings;
    std::cout << "PASS" << std::endl;
}