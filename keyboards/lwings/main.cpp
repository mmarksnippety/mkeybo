#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/hid_controller.hpp"
#include "config.hpp"
#include "factories.hpp"
#include "keyboard.hpp"
#include "debug.hpp"
#include "status_display.hpp"


mkeybo::HidController* hid_controller;
Keyboard<keyboard_config.switches_count>* keyboard;
StatusDisplay<keyboard_config.switches_count>* status_display;
queue_t event_queue;


[[noreturn]] void main_core_1()
{
    std::cout << "core 1 started" << std::endl;
    while (true)
    {
        hid_controller->devices_main_task();
        if (queue_try_remove(&event_queue, nullptr))
        {
            hid_controller->devices_update_state();
        }
    }
}


[[noreturn]] int main()
{
    stdio_init_all();
    print_logo();
    keyboard = create_keyboard();
    status_display = create_status_display(keyboard);
    hid_controller = create_hid_controller(keyboard, status_display);
    tud_init(0);
    queue_init(&event_queue, 8, sizeof(uint8_t));
    multicore_launch_core1(main_core_1);
    // main loop
    while (true)
    {
        tud_task();
        if (hid_controller->input_devices_main_task() && keyboard->is_layer_changed())
        {
            queue_add_blocking(&event_queue, nullptr);
        }
        hid_controller->usb_task_async();
    }
}
