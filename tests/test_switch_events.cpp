#include <cassert>
#include <iostream>
#include "components/base.hpp"
#include "../mkeybo/components/keyboard/keyboard_settings.hpp"
#include "components/switch_events.hpp"
#include "test_switch_events.hpp"


void test_update_switch_event_stable_idle()
{
    constexpr auto switches_count = 2;
    const auto k_settings = new mkeybo::KeyboardSettings<switches_count>("default", {}, {}, {});
    std::cout << __func__ << "...";
    mkeybo::SwitchEvent sw_event{};
    mkeybo::SwitchEventsGenerator<switches_count> generator;
    // 1. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::idle);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == false);
    // 2. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::idle);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == false);
    std::cout << "PASS" << std::endl;
    delete k_settings;
}

void test_update_switch_event_idle_press_release()
{
    constexpr auto switches_count = 2;
    const auto k_settings = new mkeybo::KeyboardSettings<switches_count>("default", {}, {}, {});
    std::cout << __func__ << "...";
    mkeybo::SwitchEvent sw_event{};
    mkeybo::SwitchEventsGenerator<switches_count> generator;
    // 1. cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 1);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 2. cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 2);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 3. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::released);
    assert(sw_event.released == 1);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 4. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::released);
    assert(sw_event.released == 2);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 5. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::released);
    assert(sw_event.released == 3);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 6. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::tap_dance_end);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 7. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::idle);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == false);
    std::cout << "PASS" << std::endl;
    delete k_settings;
}

void test_update_switch_event_idle_press_hold_release()
{
    constexpr auto switches_count = 2;
    const auto k_settings = new mkeybo::KeyboardSettings<switches_count>("default", {}, {}, {});
    std::cout << __func__ << "...";
    mkeybo::SwitchEvent sw_event{};
    mkeybo::SwitchEventsGenerator<switches_count> generator;
    // 1. cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 1);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 2. cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 2);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 3. cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 3);
    assert(sw_event.tap_dance == 1);
    assert(sw_event.hold == false);
    // 4. cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 4);
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == true);
    // 5. cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 5);
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == true);
    // 6. cycle
    generator.update_switch_event(k_settings, false, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::idle);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == 0);
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == false);
    std::cout << "PASS" << std::endl;
    delete k_settings;
}

void test_update_switch_event_max_pressed_counter()
{
    constexpr auto switches_count = 2;
    const auto k_settings = new mkeybo::KeyboardSettings<switches_count>("default", {}, {}, {});
    std::cout << __func__ << "...";
    mkeybo::SwitchEvent sw_event{
        .pressed = std::numeric_limits<uint8_t>::max() - 1,
        .released = 0,
        .hold = true,
        .type = mkeybo::SwitchEventType::pressed
    };
    mkeybo::SwitchEventsGenerator<switches_count> generator;
    // -1 cycle
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == std::numeric_limits<uint8_t>::max());
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == true);
    // 0 cycle, max pressed constantly
    generator.update_switch_event(k_settings, true, sw_event);
    generator.update_switch_event(k_settings, true, sw_event);
    assert(sw_event.type == mkeybo::SwitchEventType::pressed);
    assert(sw_event.released == 0);
    assert(sw_event.pressed == std::numeric_limits<uint8_t>::max());
    assert(sw_event.tap_dance == 0);
    assert(sw_event.hold == true);
    std::cout << "PASS" << std::endl;
    delete k_settings;
}

void test_update_switch_event_keyboard_state()
{
    constexpr auto switches_count = 2;
    const auto k_settings = new mkeybo::KeyboardSettings<switches_count>("default", {}, {}, {});
    std::cout << __func__ << "...";
    mkeybo::KeyboardState<switches_count> keyboard_state;
    mkeybo::SwitchEventsGenerator<switches_count> generator;
    keyboard_state.reset(); // all switches in idle state
    auto& switch_state = keyboard_state.get_switch_state();
    auto& switch_events = keyboard_state.get_switch_events();
    switch_state[1] = true; // set sw_no = 1 pressed
    assert(switch_state[0] == false);
    assert(switch_state[1] == true);
    assert(switch_events[0].type == mkeybo::SwitchEventType::idle);
    assert(switch_events[1].type == mkeybo::SwitchEventType::idle);
    // cycle 1
    generator.update(k_settings, &keyboard_state);
    assert(switch_events[0].type == mkeybo::SwitchEventType::idle);
    assert(switch_events[0].pressed == 0);
    assert(switch_events[0].released == 0);
    assert(switch_events[1].type == mkeybo::SwitchEventType::pressed);
    assert(switch_events[1].pressed == 1);
    assert(switch_events[1].released == 0);
    // cycle 2
    generator.update(k_settings, &keyboard_state);
    assert(switch_events[0].type == mkeybo::SwitchEventType::idle);
    assert(switch_events[0].pressed == 0);
    assert(switch_events[0].released == 0);
    assert(switch_events[1].type == mkeybo::SwitchEventType::pressed);
    assert(switch_events[1].pressed == 2);
    assert(switch_events[1].released == 0);
    std::cout << "PASS" << std::endl;
    delete k_settings;
}
