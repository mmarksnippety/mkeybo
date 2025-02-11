#pragma once

#include <pico/bootrom.h>
#include <hardware/watchdog.h>


namespace mkeybo::actions {


constexpr uint16_t action_reboot_to_bootloader_id = 0x0001;
constexpr uint16_t action_reboot_id = 0x0002;


class ActionExecutor
{
public:
    virtual ~ActionExecutor() = default;
    virtual void execute() = 0;
};


class ActionExecutorRebootToBootloader final : public ActionExecutor
{
public:
    void execute() override { reset_usb_boot(0, 0); }
};


class ActionExecutorReboot final : public ActionExecutor
{
public:
    void execute() override { watchdog_reboot(0, 0, 0); }
};

}
