#include "test_finalize_mappign_rule.hpp"
#include "fixtures.hpp"
#include <cassert>
#include <iostream>
#include "mkeybo/components/keyboard_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/key_mapper.hpp"
#include "mkeybo/factories.hpp"


void test_finalize_mapping_rule_map()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_rule = new mkeybo::key_mapper::FinalizeMappingRule<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    k_state->setup_for_settings(k_settings);
    std::cout << __func__ << "...";
    k_state->push_keycode_event_draft(H_K(10), 1);
    k_state->push_keycode_event_draft(H_K(20), 1, mkeybo::KeycodeEventType::canceled);
    k_state->push_keycode_event_draft(H_K(30), 1);
    k_rule->map(k_settings, k_state);
    for (const auto& k_event : k_state->get_filtered_keycode_events_draft())
    {
        assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    }
    for (const auto& k_event : k_state->get_filtered_keycode_events())
    {
        assert(k_event.type == mkeybo::KeycodeEventType::finalized);
    }
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}

void test_finalize_mapping_rule_finalize_keycode_event()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto k_rule = new mkeybo::key_mapper::FinalizeMappingRule<switches_count>();
    const auto k_settings = k_settings_fixture<switches_count>();
    k_state->setup_for_settings(k_settings);
    std::cout << __func__ << "...";
    auto k_event = mkeybo::KeycodeEvent{mkeybo::KeycodeEventType::canceled, H_K(10), 0};
    k_rule->finalize_keycode_event(k_state, k_event);
    assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    const auto k_finalized_event = k_state->get_filtered_keycode_events().front();
    assert(k_finalized_event.keycode == H_K(10));
    assert(k_finalized_event.type == mkeybo::KeycodeEventType::finalized);
    std::cout << "PASS" << std::endl;
    delete k_state;
    delete k_rule;
    delete k_settings;
}
