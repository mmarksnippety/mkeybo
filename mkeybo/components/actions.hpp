#pragma once

#include <map>
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


class ActionManager
{
    std::map<uint16_t, ActionExecutor*> executors;
    std::array<uint16_t, 10> buffer{};
    uint8_t index = 0;

public:
    explicit ActionManager(const std::map<uint16_t, ActionExecutor*>& executors) :
        executors(executors)
    {
        buffer.fill(0);
    }

    void reset()
    {
        index = 0;
        buffer.fill(0);
    }

    bool push(const uint16_t action_id)
    {
        if (index >= buffer.size())
        {
            return false;
        }
        buffer[index] = action_id;
        index++;
        return true;
    }

    void execute()
    {
        for (const auto& action_id : buffer)
        {
            if (action_id != 0)
            {
                if (auto executor_it = executors.find(action_id); executor_it != executors.end())
                {
                    executor_it->second->execute();
                }
            }
        }
    }
};

}
