#pragma once


#include "../base.hpp"


namespace mkeybo::mapping_rule {

template <size_t switches_count, size_t keycodes_buffer_size>
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
    virtual bool map(Keyboard<switches_count, keycodes_buffer_size>* keyboard)
    {
        return false;
    }

};

}
