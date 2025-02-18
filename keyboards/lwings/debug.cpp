#include "debug.hpp"
#include <array>
#include <iostream>
#include <string_view>
#include "config.hpp"
#include "keyboard.hpp"
#include "mkeybo/debug.hpp"
#include <algorithm>
#include "mkeybo/components/hid_controller.hpp"


extern Keyboard<keyboard_config.switches_count>* keyboard;
extern mkeybo::HidController* hid_controller;


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
//     bool is_any_info_to_print = false;
//     is_any_info_to_print |= std::ranges::any_of(keyboard->get_switch_events(), [](const auto& sw_event)
//     {
//         return sw_event.type != mkeybo::SwitchEventType::idle;
//     });
//     is_any_info_to_print |= !keyboard->get_filtered_keycode_events().empty();
//     is_any_info_to_print |= hid_controller->is_any_usb_report_ready();
//     return is_any_info_to_print;
    return false;
}

void sw_event_info(mkeybo::SwitchEvent sw_event, uint8_t sw_index)
{
    // std::cout << "switch|" << std::to_string(sw_index++) << "|"
    //     << mkeybo::get_switch_event_type_name(sw_event.type) << "|"
    //     << "p=" << std::to_string(sw_event.pressed) << "|"
    //     << "r=" << std::to_string(sw_event.released) << "|"
    //     << "td=" << std::to_string(sw_event.tap_dance) << "|"
    //     << "h=" << std::boolalpha << sw_event.hold;
}
//
void print_keyboard_info()
{
//     if (!is_any_info_to_print())
//         return;
//     std::cout << "New info!" << std::endl << std::endl;
//
//
//     const auto switch_events = keyboard->get_switch_events();
//     // for (auto index = 0; const auto& sw_event : switch_events)
//     // {
//     //     if (sw_event.type != mkeybo::SwitchEventType::idle)
//     //     {
//     //         sw_event_info(sw_event, index);
//     //         std::cout << std::endl;
//     //     }
//     //     index++;
//     // }
//     for (auto& key_event : keyboard->get_filtered_keycode_events())
//     {
//         std::cout << "event|" << mkeybo::get_keycode_event_type_name(key_event.type) << "|" << key_event.keycode << "||";
//         sw_event_info(switch_events[key_event.switch_no], key_event.switch_no);
//         std::cout << std::endl;
//     }
//     for (const auto report : hid_controller->get_usb_reports())
//     {
//         if (report->is_report_ready())
//         {
//             std::cout << report->to_string() << std::endl;
//         }
//     }
}
