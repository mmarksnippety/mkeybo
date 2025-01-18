#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "bsp/board_api.h"
#include "config.hpp"
#include "factories.hpp"
#include "keyboard.hpp"
#include "debug.hpp"
#include "display.hpp"
#include "logo.hpp"


mkeybo::Keyboard<keyboard_config.switches_count>* keyboard{};
Display* display{};

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
        if (keyboard->get_state()->is_layer_changed())
        {
            constexpr QueueEvnet event = {QueueEventType::layer_change};
            queue_add_blocking(&event_queue, &event);
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

[[noreturn]] void core1_entry()
{
    display->display_logo(mkeybo_logo);

    while (true)
    {
        QueueEvnet event;
        queue_remove_blocking(&event_queue, &event);
        if (event.type == QueueEventType::layer_change)
        {
            display->show_keyboard_status(keyboard);
        }
    }
}

[[noreturn]] int main()
{
    stdio_init_all();
    tud_init(0);
    print_logo();
    keyboard = new Keyboard<keyboard_config.switches_count>();
    keyboard->update_settings(create_keyboard_settings<keyboard_config.switches_count>());

    queue_init(&event_queue, sizeof(QueueEvnet), 3);

    display = new Display(display_config);
    multicore_launch_core1(core1_entry);
    sleep_ms(2000);
    constexpr QueueEvnet event = {QueueEventType::layer_change};
    queue_add_blocking(&event_queue, &event);
    // main loop
    while (true)
    {
        tud_task();
        keyboard_main_task();
        keyboard_hid_task();
    }
}
