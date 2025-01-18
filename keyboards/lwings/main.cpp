#include <iostream>
#include <string_view>
#include "bsp/board_api.h"
#include "config.hpp"
#include "factories.hpp"
#include "keyboard.hpp"
#include "mkeybo/debug.hpp"
#include "pico/stdlib.h"
#include "debug.hpp"
#include "display.hpp"
#include "logo.hpp"


mkeybo::Keyboard<keyboard_config.switches_count>* keyboard{};
Display* display{};

auto get_ms_since_boot() { return to_ms_since_boot(get_absolute_time()); }


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
        if (keyboard->get_state()->is_layer_changed())
        {
            display->show_keyboard_status(keyboard);
        }
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

    display = new Display(display_config);
    display->display_logo(mkeybo_logo);
    sleep_ms(2000);
    display->show_keyboard_status(keyboard);

    // main loop
    while (true)
    {
        tud_task();
        keyboard_main_task();
        keyboard_hid_task();
    }
}
