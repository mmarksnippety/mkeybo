#include "test_change_layout_mapping_rule.hpp"
#include "fixtures.hpp"
#include <cassert>
#include <iostream>
#include <ranges>
#include "mkeybo/components/keyboard_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "../mkeybo/components/key_mapper.hpp"
#include "mkeybo/factories.hpp"


void test_change_layout_mapping_rule_map()
{
    constexpr auto switches_count = 4;
    auto k_state = new mkeybo::KeyboardState<switches_count>();
    auto k_rule = new mkeybo::key_mapper::ChangeLayoutMappingRule<switches_count>();
    auto k_settings = k_settings_fixture<switches_count>();
    k_state->setup_for_settings(k_settings);
    std::cout << __func__ << "...";
    // no key pressed - no key events
    auto result = k_rule->map(k_settings, k_state);
    assert(result == false);
    // assert(std::ranges::empty(k_state->keycode_events_.get_finalized_events()));
    assert(!k_state->is_layer_active(1));
    assert(!k_state->is_layer_active(2));
    assert(k_state->get_active_layout() == 0);
    // layout 1 with some key pressed - cycle 1
    k_state->reset();
    k_state->push_keycode_event(H_K(2), 1);
    k_state->push_keycode_event(LAYOUT_K(1), 0);
    result = k_rule->map(k_settings, k_state);
    assert(result == true); // cycle 2 is needed
    assert(k_state->get_active_layout() == 1);
    assert(!k_state->is_layer_active(1));
    assert(!k_state->is_layer_active(2));
    // layout 1 with some key pressed - cycle 2
    result = k_rule->map(k_settings, k_state);
    assert(result == false); // no more cycles is needed
    assert(k_state->get_active_layout() == 1);
    assert(!k_state->is_layer_active(1));
    assert(!k_state->is_layer_active(2));
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}
