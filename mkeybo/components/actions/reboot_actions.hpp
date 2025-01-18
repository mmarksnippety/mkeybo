#pragma once

#include <pico/bootrom.h>
#include <hardware/watchdog.h>
#include "../base.hpp"
#include "base_action.hpp"


namespace mkeybo::actions {


template <size_t switches_count>
class RebootToBootloaderAction final : public BaseAction<switches_count>
{
public:
    void execute(const Keyboard<switches_count>* keyboard, const Keycode& trigger_keycode) override
    {
        reset_usb_boot(0, 0);
    }
};

template <size_t switches_count>
class RebootAction final : public BaseAction<switches_count>
{
public:
    void execute(const Keyboard<switches_count>* keyboard, const Keycode& trigger_keycode) override
    {
        watchdog_reboot(0, 0, 0);
    }
};


}
