#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "bsp/board_api.h"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/hid_controller.hpp"
#include "config.hpp"
#include "factories.hpp"
#include "keyboard.hpp"
#include "debug.hpp"
#include "status_display.hpp"


enum class QueueEventType
{
    input_device_update_state,
};

struct QueueEvnet
{
    QueueEventType type;
};


mkeybo::HidController* hid_controller;
Keyboard<keyboard_config.switches_count>* keyboard;
StatusDisplay<keyboard_config.switches_count>* status_display;
queue_t event_queue;


[[noreturn]] void main_core_1()
{
    std::cout << "core 1 started" << std::endl;
    auto *event = new QueueEvnet();
    while (true)
    {
        hid_controller->devices_main_task();
        if (queue_try_remove(&event_queue, event))
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
    queue_init(&event_queue, sizeof(QueueEvnet), 3);
    tud_init(0);
    multicore_launch_core1(main_core_1);

    const auto *event = new QueueEvnet();

    // main loop
    while (true)
    {
        tud_task();
        if (hid_controller->input_devices_main_task())
        {
            // update display state only if something happens
            if (keyboard->is_layer_changed())
            {
                queue_add_blocking(&event_queue, event);
            }
        }
        hid_controller->usb_task_async();
    }
}
