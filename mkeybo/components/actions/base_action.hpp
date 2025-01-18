#pragma once

#include "../base.hpp"


namespace mkeybo::actions {
template <size_t switches_count>
class BaseAction
{
public:
    virtual ~BaseAction() = default;
    virtual void execute(const Keyboard<switches_count>* keyboard, const Keycode& trigger_keycode) = 0;
};

}
