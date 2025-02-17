#include <memory>
#include "factories.hpp"
#include "keyboard_settings.hpp"
#include "keyboard.hpp"


Keyboard<keyboard_config.switches_count>* create_keyboard()
{
    const auto keyboard = new Keyboard<keyboard_config.switches_count>();
    keyboard->apply_settings(create_keyboard_settings());
    return keyboard;
}


StatusLeds<keyboard_config.switches_count>* create_status_leds(Keyboard<keyboard_config.switches_count>* keyboard)
{
    return new StatusLeds<keyboard_config.switches_count>(left_led_config.pwm_pin, right_led_config.pwm_pin, keyboard);
}


std::unique_ptr<mkeybo::HidControllerSettings> create_hid_controller_settings()
{
    return std::make_unique<mkeybo::HidControllerSettings>(10);
}


mkeybo::HidController* create_hid_controller(Keyboard<keyboard_config.switches_count>* keyboard,
                                             StatusLeds<keyboard_config.switches_count>* status_leds)
{
    const auto hid_controller = new mkeybo::HidController(
        keyboard_config.keyboard_name,
        keyboard_config.manufactured_name,
        {
            {mkeybo::actions::action_reboot_id, new mkeybo::actions::ActionExecutorReboot()},
            {mkeybo::actions::action_reboot_to_bootloader_id, new mkeybo::actions::ActionExecutorRebootToBootloader()},
        },
        {keyboard},
        {status_leds},
        create_hid_controller_settings()
        );
    return hid_controller;
}
