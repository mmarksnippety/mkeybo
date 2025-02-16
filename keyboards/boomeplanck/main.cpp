#include "pico/stdlib.h"
#include "mkeybo/components/hid_controller.hpp"
#include "config.hpp"
#include "settings.hpp"
#include "keyboard.hpp"
#include "factories.hpp"

mkeybo::HidController* hid_controller;
Keyboard<keyboard_config.switches_count>* keyboard;


[[noreturn]] int main()
{
    stdio_init_all();
    tud_init(0);
    keyboard = create_keyboard();
    hid_controller = create_hid_controller(keyboard);
    while (true)
    {
        tud_task();
        hid_controller->main_task();
        hid_controller->usb_task();
        keyboard->leds_task(); // TODO: make leds outside keyboard
    }
}
