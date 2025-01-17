#pragma once

#include "../keyboard_settings.hpp"
#include "../keyboard_state.hpp"
#include "../base.hpp"


namespace mkeybo::key_mapper {

template <size_t switches_count>
class BaseMappingRule
{
public:
    BaseMappingRule() = default;
    virtual ~BaseMappingRule() = default;

    /**
     * Method map may map some switches to keycode or remap some keycode to other keycode,
     * or event delete some mapped keycode.
     *
     * If return true, then mapping process must be restarting.
     * Is usefully ex for layer changing or layout changing
     */
    virtual bool map(KeyboardSettings<switches_count>* keyboard_settings, KeyboardState<switches_count>* keyboard_state)
    {
        return false;
    }

};

}
