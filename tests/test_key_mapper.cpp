#include "test_key_mapper.hpp"
#include "fixtures.hpp"


void test_key_mapper_map()
{
    constexpr auto switches_count = 4;
    auto k_settings = k_settings_fixture<switches_count>();
    auto k_state = new mkeybo::KeyboardState<switches_count>();
    k_state->setup_for_settings(k_settings);
    auto k_mapper = mkeybo::create_key_mapper<switches_count>();
    std::cout << __func__ << "...";
    // idle - no keycode events
    k_mapper->map(k_settings, k_state);
    assert(std::ranges::empty(k_state->keycode_events.get_finalized_events()));
    // switch 0
    k_state->switch_events[0].event_type = mkeybo::SwitchEventType::pressed;
    k_state->switch_events[0].pressed = 1;
    k_mapper->map(k_settings, k_state);
    auto view = k_state->keycode_events.get_finalized_events();
    assert(std::ranges::distance(view) == 1);
    auto keycode = *std::ranges::begin(view);
    assert(keycode.keycode == H_K(1));
    // switch 0 and switch 3 - change layer
    k_state->reset();
    k_state->switch_events[0].event_type = mkeybo::SwitchEventType::pressed;
    k_state->switch_events[0].pressed = 1;
    k_state->switch_events[3].event_type = mkeybo::SwitchEventType::pressed;
    k_state->switch_events[3].pressed = 1;
    k_mapper->map(k_settings, k_state);
    view = k_state->keycode_events.get_finalized_events();
    auto it = std::ranges::begin(view);
    auto end = std::ranges::end(view);
    keycode = *it++;
    assert(keycode.keycode == H_K(100));
    keycode = *it++;
    assert(keycode.keycode == LAYER_K(1));
    assert(it == end);
    delete k_mapper;
    delete k_state;
    delete k_settings;
    std::cout << "PASS" << std::endl;
}
