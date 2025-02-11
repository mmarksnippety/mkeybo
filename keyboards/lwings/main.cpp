#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "bsp/board_api.h"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/hid_controller.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/utils.hpp"
#include "config.hpp"
#include "factories.hpp"
#include "keyboard.hpp"
#include "debug.hpp"
#include "status_display.hpp"
#include "logo.hpp"


mkeybo::HidController* hid_controller;
mkeybo::Keyboard<keyboard_config.switches_count>* keyboard;
StatusDisplay* status_display;
queue_t event_queue;


enum class QueueEventType
{
    idle,
    layer_change,
};

struct QueueEvnet
{
    QueueEventType type{QueueEventType::idle};
    uint8_t data[64]{}; //
};


void hid_controller_main_task()
{
    static auto last_main_task_ts = mkeybo::get_ms_since_boot();
    static auto current_ts = last_main_task_ts;
    current_ts = mkeybo::get_ms_since_boot();
    if (keyboard == nullptr)
    {
        return;
    }
    if (current_ts - last_main_task_ts >= keyboard->get_settings()->switches_refresh_interval_ms)
    {
        hid_controller->main_task();
        // print_keyboard_info();
        if (keyboard->is_layer_changed())
        {
            constexpr QueueEvnet event = {QueueEventType::layer_change};
            queue_add_blocking(&event_queue, &event);
        }

        last_main_task_ts = current_ts;
    }
}


void hid_controller_usb_task()
{
    static auto last_main_task_ts = mkeybo::get_ms_since_boot();
    static auto current_ts = last_main_task_ts;
    current_ts = mkeybo::get_ms_since_boot();
    if (keyboard == nullptr)
    {
        return;
    }
    if (current_ts - last_main_task_ts >= keyboard->get_settings()->report_send_interval_ms)
    {
        hid_controller->usb_task();
        last_main_task_ts = current_ts;
    }
}

[[noreturn]] void core1_entry()
{
    status_display->display_logo(mkeybo_logo);

    while (true)
    {
        QueueEvnet event;
        queue_remove_blocking(&event_queue, &event);
        if (event.type == QueueEventType::layer_change)
        {
            status_display->show_keyboard_status(keyboard);
        }
    }
}

[[noreturn]] int main()
{
    stdio_init_all();
    print_logo();
    keyboard = new Keyboard<keyboard_config.switches_count>();
    keyboard->set_settings(create_keyboard_settings<keyboard_config.switches_count>());
    hid_controller = new mkeybo::HidController(
        keyboard_config.keyboard_name,
        keyboard_config.manufactured_name,
        {
            {mkeybo::actions::action_reboot_id, new mkeybo::actions::ActionExecutorReboot()},
            {mkeybo::actions::action_reboot_to_bootloader_id, new mkeybo::actions::ActionExecutorRebootToBootloader()},
        },
        {keyboard}
        );
    status_display = new StatusDisplay(display_config);
    tud_init(0);
    queue_init(&event_queue, sizeof(QueueEvnet), 3);
    multicore_launch_core1(core1_entry);
    sleep_ms(2000);
    constexpr QueueEvnet event = {QueueEventType::layer_change};
    queue_add_blocking(&event_queue, &event);
    // main loop
    while (true)
    {
        tud_task();
        hid_controller_main_task();
        hid_controller_usb_task();
    }
}
