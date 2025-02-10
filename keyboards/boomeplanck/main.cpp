#include "pico/stdlib.h"
#include "config.hpp"
#include "settings.hpp"
#include "keyboard.hpp"
#include "utils.hpp"
#include "pwm_led.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/hid_controller.hpp"
#include "mkeybo/factories.hpp"


mkeybo::HidController* hid_controller;
mkeybo::Keyboard<keyboard_config.switches_count>* keyboard;


void hid_controller_main_task()
{
    static auto last_main_task_ts = get_ms_since_boot();
    static auto current_ts = last_main_task_ts;
    current_ts = get_ms_since_boot();
    if (current_ts - last_main_task_ts >= keyboard->get_settings()->switches_refresh_interval_ms)
    {
        hid_controller->main_task();
        last_main_task_ts = current_ts;
    }
}


void hid_controller_usb_task()
{
    static auto last_main_task_ts = get_ms_since_boot();
    static auto current_ts = last_main_task_ts;
    current_ts = get_ms_since_boot();
    if (current_ts - last_main_task_ts >= keyboard->get_settings()->report_send_interval_ms)
    {
        hid_controller->usb_task();
        last_main_task_ts = current_ts;
    }
}


[[noreturn]] int main()
{
    stdio_init_all();
    tud_init(0);
    keyboard = new Keyboard<keyboard_config.switches_count>();
    keyboard->set_settings(create_keyboard_settings<keyboard_config.switches_count>());
    hid_controller = new mkeybo::HidController(
        keyboard_config.keyboard_name,
        keyboard_config.manufactured_name,
        {keyboard},
        mkeybo::create_action_manager()
        );
    while (true)
    {
        tud_task();
        hid_controller_main_task();
        hid_controller_usb_task();
        reinterpret_cast<Keyboard<keyboard_config.switches_count>*>(keyboard)->leds_task();
    }
}
