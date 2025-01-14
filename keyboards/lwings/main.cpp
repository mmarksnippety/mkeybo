#include <array>
#include <iostream>
#include <numeric>
#include <string_view>
#include "bsp/board_api.h"
#include "config.hpp"
#include "factories.hpp"
#include "keyboard.hpp"
#include "mkeybo/debug.hpp"
#include "pico/stdlib.h"


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


void print_keyboard_info()
{
    for (auto& sw_event : keyboard->get_state()->get_switch_events())
    {
        if (sw_event.type == mkeybo::SwitchEventType::idle)
        {
            continue;
        }
        // std::cout << "switch|"
        //     << mkeybo::get_switch_event_type_name(sw_event.type) << "|"
        //     << std::to_string(sw_event.pressed) << "|"
        //     << std::to_string(sw_event.released{0};
        //     << std::to_string(sw_event.tap_dance{0};
        // bool hold{false};
        // << sw_event.switch_no << std::endl;
    }
    const bool print_status = !std::ranges::empty(keyboard->get_state()->get_filtered_keycode_events()) ||
        keyboard->get_usb_reporter_manager()->is_any_report_ready(keyboard->get_state());
    if (print_status)
    {
        std::cout << "--------------------------" << std::endl;
    }
    for (auto& key_event : keyboard->get_state()->get_filtered_keycode_events())
    {
        std::cout << "event|" << mkeybo::get_keycode_event_type_name(key_event.type) << "|" << key_event.keycode
                  << std::endl;
    }
    for (const auto& [keycode_type, report] : keyboard->get_state()->get_reports())
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
    if (print_status)
    {
        std::cout << "==========================" << std::endl;
    }
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
    std::cout << "Settings:" << std::endl;
    print_settings_rules(keyboard->get_settings()->rules);
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
