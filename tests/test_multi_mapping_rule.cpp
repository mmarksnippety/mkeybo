#include "test_multi_mapping_rule.hpp"
#include <cassert>
#include <iostream>
#include "fixtures.hpp"
#include "../mkeybo/components/keyboard/keyboard_settings.hpp"
#include "mkeybo/components/keyboard_rule_settings/multi_mapping_rule_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/components/key_mapper.hpp"
#include "mkeybo/debug.hpp"


void test_multi_mapping_get_src_keycode_event()
{
    // constexpr auto switches_count = 4;
    // const auto k_state = new mkeybo::KeyboardState<switches_count>();
    // const auto k_settings = k_settings_fixture<switches_count>();
    // const std::pair<std::vector<mkeybo::Keycode>, mkeybo::Keycode> multi_mapping{{H_K(1), H_K(2)}, LAYER_K(3)};
    // k_settings->rules.insert({mkeybo::keyboard_rule_settings::rule_name_multi_mapping,
    //                           new mkeybo::keyboard_rule_settings::MultiMappingRuleSettings({multi_mapping})});
    // k_state->setup_for_settings(k_settings);
    // const auto k_rule = new mkeybo::key_mapper::MultiMappingRule<switches_count>();
    // mkeybo::print_settings_rules(k_settings->rules);
    // std::cout << __func__ << "...";
    //
    // k_state->push_keycode_event(H_K(2), 1, mkeybo::KeycodeEventType::finalized);
    // k_state->push_keycode_event(H_K(3), 1, mkeybo::KeycodeEventType::finalized);
    // k_state->push_keycode_event(H_K(1), 1, mkeybo::KeycodeEventType::finalized);
    //
    // std::cout << std::endl;
    // auto src_k_events = k_rule->get_src_keycode_event(k_state, multi_mapping.first);
    // const auto excepted_k_events_codes = std::vector<mkeybo::Keycode>{H_K(1), H_K(2)};
    // for (auto index=0; const auto& k_event : src_k_events)
    // {
    //     std::cout << "k_event: " << *k_event << std::endl;
    //     // assert(k_event->keycode == excepted_k_events_codes[index]);
    //     index++;
    // }
    //
    // //
    // // //    auto& switch_events = k_state->get_switch_events();
    // //    switch_events[0].type = mkeybo::SwitchEventType::tap_dance_end;
    // //    switch_events[0].tap_dance = 1;
    // //    k_state->push_keycode_event(H_K(1), 0);
    // //    auto k_event = k_state->get_all_keycode_events().front();
    // //    k_rule->map_event(k_state, k_event, tap_dance_map);
    // //    assert(k_event.keycode == H_K(1));
    // //    assert(k_event.type == mkeybo::KeycodeEventType::canceled);
    // //    assert(std::ranges::distance(k_state->get_filtered_keycode_events()) == 1);
    // //    const auto finalized_k_event = k_state->get_filtered_keycode_events().front();
    // //    assert(finalized_k_event.keycode == H_K(1));
    // //    assert(finalized_k_event.type == mkeybo::KeycodeEventType::finalized);
    // std::cout << "PASS" << std::endl;
    // delete k_state;
    // delete k_rule;
    // delete k_settings;
}
