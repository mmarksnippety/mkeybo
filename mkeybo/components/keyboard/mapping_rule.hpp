#pragma once

#include "mkeybo/components/base.hpp"
#include <map>


namespace mkeybo::keyboard {


struct MappingRuleSettings
{
};


template <size_t switches_count, size_t keycodes_buffer_size>
class MappingRule
{
public:
    MappingRule() = default;
    virtual ~MappingRule() = default;

    [[nodiscard]] virtual std::string get_settings_name() const { return ""; }

    virtual void apply_settings(const std::map<std::string, MappingRuleSettings*>& rule_settings)
    {
    }

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
