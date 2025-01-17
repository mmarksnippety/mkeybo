#include <array>
#include <iostream>
#include <string_view>
#include "bsp/board_api.h"
#include "config.hpp"
#include "factories.hpp"
#include "keyboard.hpp"
#include "mkeybo/debug.hpp"
#include "pico/stdlib.h"
#include <algorithm>


mkeybo::Keyboard<keyboard_config.switches_count>* keyboard{};


auto get_ms_since_boot() { return to_ms_since_boot(get_absolute_time()); }


void print_logo()
{
    // clang-format off
     constexpr auto logo = std::array<std::string_view,8>{
        R"(           _              _           )",
        R"(          | |            | |          )",
        R"( _ __ ___ | | _____ _   _| |__   ___  )",
        R"(| '_ ` _ \| |/ / _ \ | | | '_ \ / _ \ )",
        R"(| | | | | |   <  __/ |_| | |_) | (_) |)",
        R"(|_| |_| |_|_|\_\___|\__, |_.__/ \___/ )",
        R"(                     __/ |            )",
        R"(                    |___/             )",
    };
    // clang-format on
    std::cout << std::endl;
    std::cout << std::endl;
    for (auto& line : logo)
        std::cout << line << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;
}


bool is_any_info_to_print()
{
    bool is_any_info_to_print = false;
    is_any_info_to_print |= std::ranges::any_of(keyboard->get_state()->get_switch_events(), [](const auto& sw_event)
    {
        return sw_event.type != mkeybo::SwitchEventType::idle;
    });
    is_any_info_to_print |= !keyboard->get_state()->get_filtered_keycode_events().empty();
    return is_any_info_to_print;
}

void print_keyboard_info()
{
    if (!is_any_info_to_print()) return;
    std::cout << std::endl << std::endl;
    for (auto index = 0; const auto& sw_event : keyboard->get_state()->get_switch_events())
    {
        if (sw_event.type != mkeybo::SwitchEventType::idle)
        {
            std::cout << "switch|" << std::to_string(index++) << "|"
                << mkeybo::get_switch_event_type_name(sw_event.type) << "|"
                << "p=" << std::to_string(sw_event.pressed) << "|"
                << "r=" << std::to_string(sw_event.released) << "|"
                << "td=" << std::to_string(sw_event.tap_dance) << "|"
                << "h=" << std::boolalpha << sw_event.hold
                << std::endl;
        }
        index++;
    }
    for (auto& key_event : keyboard->get_state()->get_filtered_keycode_events())
    {
        std::cout << "event|" << mkeybo::get_keycode_event_type_name(key_event.type) << "|" << key_event.keycode
            << std::endl;
    }
    /*
    for (const auto& [keycode_type, report] : keyboard->get_state()->get_usb_reports())
    {
        if (keycode_type == mkeybo::KeycodeType::hid && report->status == mkeybo::UsbReportStatus::ready)
        {
            const auto hid_report = reinterpret_cast<const mkeybo::UsbHidKeycodeReport*>(report);
            std::cout << "report|" << std::to_string(hid_report->modifiers) << "| ";
            for (auto& keycode : hid_report->keycodes)
            {
                std::cout << std::to_string(keycode) << " ";
            }
            std::cout << "|" << std::endl;
        }
    }
    */
}


void keyboard_main_task()
{
    static auto last_main_task_ts = get_ms_since_boot();
    static auto current_ts = last_main_task_ts;
    current_ts = get_ms_since_boot();
    if (keyboard == nullptr)
    {
        return;
    }
    if (current_ts - last_main_task_ts >= keyboard->get_settings()->switches_refresh_interval_ms)
    {
        keyboard->main_task(); // idle < 200us, typically << 300us
        print_keyboard_info();
        last_main_task_ts = current_ts;
    }
}


void keyboard_hid_task()
{
    static auto last_main_task_ts = get_ms_since_boot();
    static auto current_ts = last_main_task_ts;
    current_ts = get_ms_since_boot();
    if (keyboard == nullptr)
    {
        return;
    }
    if (current_ts - last_main_task_ts >= keyboard->get_settings()->report_send_interval_ms)
    {
        keyboard->hid_task();
        last_main_task_ts = current_ts;
    }
}


[[noreturn]] int main()
{
    stdio_init_all();
    tud_init(0);
    print_logo();
    // initialize keyboard, and load default settings
    keyboard = new Keyboard<keyboard_config.switches_count>();
    keyboard->update_settings(create_keyboard_settings<keyboard_config.switches_count>());
    std::cout << "UniqueID: " << keyboard->get_unique_id() << std::endl;
    mkeybo::print_settings(keyboard->get_settings());
    mkeybo::print_settings_rules(keyboard->get_settings()->rules);
    std::cout << std::endl;
    std::cout << std::endl;
    // main loop
    while (true)
    {
        tud_task();
        keyboard_main_task();
        keyboard_hid_task();
    }
}
