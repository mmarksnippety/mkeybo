#pragma once

#include "config.hpp"
#include "mkeybo/components/keyboard.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/debug.hpp"


template <size_t switches_count, size_t keycodes_buffer_size = 20>
class Keyboard final : public mkeybo::Keyboard<switches_count, keycodes_buffer_size>
{
public:
    explicit Keyboard() :
        mkeybo::Keyboard<switches_count>(
            mkeybo::create_switch_reader_matrix<switches_count>(switch_reader_config),
            mkeybo::create_switch_events_generator<switches_count, keycodes_buffer_size>(),
            mkeybo::create_keycode_mapping_rules<switches_count, keycodes_buffer_size>())
    {
    }

    void on_update_switch_events() override
    {
        for (auto index = 0; const auto& sw_event : this->switch_events)
        {
            if (sw_event.type != mkeybo::SwitchEventType::idle)
            {
                sw_event_info(sw_event, index);
                std::cout << std::endl;
            }
            index++;
        }
    }

    void sw_event_info(mkeybo::SwitchEvent sw_event, uint8_t sw_index)
    {
        std::cout << "switch|" << std::to_string(sw_index++) << "|"
            << mkeybo::get_switch_event_type_name(sw_event.type) << "|"
            << "p=" << std::to_string(sw_event.pressed) << "|"
            << "r=" << std::to_string(sw_event.released) << "|"
            << "td=" << std::to_string(sw_event.tap_dance) << "|"
            << "h=" << std::boolalpha << sw_event.hold;
    }

};
