#pragma once

#include "config.hpp"
#include "mkeybo/components/keyboard.hpp"
#include "mkeybo/factories.hpp"


template <size_t switches_count, size_t keycodes_buffer_size = 20>
class Keyboard final : public mkeybo::Keyboard<switches_count, keycodes_buffer_size>
{

public:
    explicit Keyboard() :
        mkeybo::Keyboard<switches_count>(
            mkeybo::create_switch_reader_matrix<switches_count>(switch_reader_config),
            mkeybo::create_switch_events_generator<switches_count>(),
            mkeybo::create_keycode_mapping_rules<switches_count, keycodes_buffer_size>()
        )
    {
    }
};
