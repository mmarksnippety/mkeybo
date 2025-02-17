#include "pico/stdlib.h"
#include "mkeybo/components/hid_controller.hpp"
#include "config.hpp"
#include "keyboard_settings.hpp"
#include "keyboard.hpp"
#include "factories.hpp"
#include "status_leds.hpp"

mkeybo::HidController* hid_controller;
Keyboard<keyboard_config.switches_count>* keyboard;
StatusLeds<keyboard_config.switches_count>* status_leds;


[[noreturn]] int main()
{
    stdio_init_all();
    tud_init(0);
    keyboard = create_keyboard();
    status_leds = create_status_leds(keyboard);
    hid_controller = create_hid_controller(keyboard, status_leds);
    while (true)
    {
        tud_task();
        if (hid_controller->input_devices_main_task() && keyboard->is_layer_changed())
        {
            hid_controller->devices_update_state();
        };
        hid_controller->devices_main_task();
        hid_controller->usb_task_async();
    }
}
