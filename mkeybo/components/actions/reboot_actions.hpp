#pragma once

#include <pico/bootrom.h>
#include <hardware/watchdog.h>
#include "../base.hpp"
#include "base_action.hpp"


namespace mkeybo::actions {


template <size_t switches_count, size_t keycodes_buffer_size>
class RebootToBootloaderAction final : public BaseAction<switches_count, keycodes_buffer_size>
{
public:
    void execute(const Keyboard<switches_count, keycodes_buffer_size>* keyboard, const Keycode& trigger_keycode) override
    {
        reset_usb_boot(0, 0);
    }
};

template <size_t switches_count, size_t keycodes_buffer_size>
class RebootAction final : public BaseAction<switches_count, keycodes_buffer_size>
{
public:
    void execute(const Keyboard<switches_count, keycodes_buffer_size>* keyboard, const Keycode& trigger_keycode) override
    {
        watchdog_reboot(0, 0, 0);
    }
};


}
