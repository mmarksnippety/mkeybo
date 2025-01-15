#include <iostream>
#include <ostream>
#include "pico/stdlib.h"
#include "test_change_layer_mapping_rule.hpp"
#include "test_change_layout_mapping_rule.hpp"
#include "test_default_mapping_rule.hpp"
#include "test_key_mapper.hpp"
#include "test_keycode_buffer.hpp"
#include "test_switch_events.hpp"
#include "test_tap_dance_mapping_rule.hpp"
#include "test_usb_reporter.hpp"
#include "test_finalize_mappign_rule.hpp"


int main()
{
    stdio_init_all();
    std::cout << std::endl << std::endl;
    std::cout << "++++++++++++" << std::endl;
    std::cout << "mkeybo tests" << std::endl;
    std::cout << "============" << std::endl;
    // //TODO: test KeyboardState, layer, layout, *_prev_cycle_
    test_update_switch_event_stable_idle();
    test_update_switch_event_idle_press_release();
    test_update_switch_event_idle_press_hold_release();
    test_update_switch_event_max_pressed_counter();
    test_update_switch_event_keyboard_state();
    // std::cout << "============" << std::endl;
    test_keycode_event_buffer_push();
    test_keycode_event_buffer_get_filtered_events();
    test_keycode_event_buffer_get_filtered_events_only_one_type();
    test_keycode_event_buffer_iterate_with_reference();
    std::cout << "============" << std::endl;
    test_finalize_mapping_rule_map();
    test_finalize_mapping_rule_finalize_keycode_event();
    std::cout << "============" << std::endl;
    test_default_mapping_rule_get_keycode();
    test_default_mapping_rule_map();
    std::cout << "============" << std::endl;
    test_change_layer_mapping_rule_map();
    std::cout << "============" << std::endl;
    test_change_layout_mapping_rule_map();
    std::cout << "============" << std::endl;
    test_tap_dance_mapping_rule_map_event_tap_dance_end();
    test_tap_dance_mapping_rule_map_event_tap_dance_end_with_action();
    test_tap_dance_mapping_rule_map_event_press_hold();
    test_tap_dance_mapping_rule_map_event_press();
    std::cout << "============" << std::endl;
    test_key_mapper_map_idle();
    test_key_mapper_map_simple_key();
    test_key_mapper_map_with_layer_change(); // <==============
    // std::cout << "============" << std::endl;
    // test_hid_reporter_get_regular_keycodes();
    // test_hid_reporter_get_modifier_keycodes();
    // test_hid_reporter_generate_report_modifiers();
    // test_hid_reporter_generate_report_keycodes();
    //TODO: test UsbCcReporter
    // test_reporter_manager_generate_reports();
    std::cout << "============" << std::endl;
    std::cout << "passed" << std::endl;
    return 0;
}
