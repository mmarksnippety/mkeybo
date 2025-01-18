#pragma once

#include "config.hpp"
#include "mkeybo/components/keyboard.hpp"
#include "mkeybo/factories.hpp"

template <size_t switches_count>
class Keyboard final : public mkeybo::Keyboard<switches_count>
{

public:
    explicit Keyboard() :
        mkeybo::Keyboard<switches_count>(mkeybo::create_keyboard_state<switches_count>(),
                                         mkeybo::create_switch_reader_matrix<switches_count>(switch_reader_config),
                                         mkeybo::create_switch_events_generator<switches_count>(),
                                         mkeybo::create_key_mapper<switches_count>(),
                                         mkeybo::create_usb_reporter_manager<switches_count>(),
                                         mkeybo::create_action_manager<switches_count>()
            )
    {
    }

    void on_led_status_update() override
    {
        auto led_status = this->state_->get_led_status();
        std::cout << "USB LED status:" << std::endl;
        std::cout << "Num Lock: " << static_cast<int>(led_status.num_lock) << std::endl;
        std::cout << "Caps Lock: " << static_cast<int>(led_status.caps_lock) << std::endl;
        std::cout << "Scroll Lock: " << static_cast<int>(led_status.scroll_lock) << std::endl;
        std::cout << "Compose: " << static_cast<int>(led_status.compose) << std::endl;
        std::cout << "Kana: " << static_cast<int>(led_status.kana) << std::endl;
        std::cout << "Reserved: " << static_cast<int>(led_status.reserved) << std::endl;
    }
};
