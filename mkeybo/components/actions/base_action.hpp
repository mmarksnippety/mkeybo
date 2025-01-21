#pragma once

#include "../base.hpp"


namespace mkeybo::actions {
template <size_t switches_count, size_t keycodes_buffer_size>
class BaseAction
{
public:
    virtual ~BaseAction() = default;
    virtual void execute(const Keyboard<switches_count, keycodes_buffer_size>* keyboard, const Keycode& trigger_keycode) = 0;
};

}
