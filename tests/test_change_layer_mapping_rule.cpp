#include "test_change_layer_mapping_rule.hpp"
#include "fixtures.hpp"
#include <cassert>
#include <iostream>
#include <ranges>
#include "mkeybo/components/keyboard_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/key_mapper.hpp"
#include "mkeybo/factories.hpp"


void test_change_layer_mapping_rule_map()
{
   
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_rule = new mkeybo::key_mapper::ChangeLayerMappingRule<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    std::cout << __func__ << "...";
    k_state->setup_for_settings(k_settings);
    // no key pressed - no key events
    auto result = k_rule->map(k_settings, k_state);
    assert(result == false);
    // assert(std::ranges::empty(k_state->keycode_events_.get_finalized_events()));
    assert(!k_state->is_layer_active(0));
    assert(!k_state->is_layer_active(1));
    // first layer with some key pressed - cycle 1
    k_state->reset();
    k_state->push_keycode_event_draft(H_K(10), 1);
    k_state->push_keycode_event_draft(LAYER_K(1), 0);
    result = k_rule->map(k_settings, k_state);
    assert(result == true); // next cycle needed
    assert(!k_state->is_layer_active(0));
    assert(k_state->is_layer_active(1));
    // cycle 2
    result = k_rule->map(k_settings, k_state);
    assert(result == false); // next cycle not needed
    assert(!k_state->is_layer_active(0));
    assert(k_state->is_layer_active(1));
    //
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}
