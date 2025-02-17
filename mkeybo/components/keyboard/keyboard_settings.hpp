#pragma once

#include <map>
#include <string>
#include <vector>
#include <ranges>
#include "mkeybo/components/input_device_settings.hpp"
#include "mkeybo/components/keyboard/mapping_rule.hpp"
#include "layer.hpp"


namespace mkeybo::keyboard {


template <size_t switches_count>
class KeyboardSettings : public InputDeviceSettings
{
public:
    std::string default_layout;
    std::vector<KeyboardLayer<switches_count>*> layouts;
    std::vector<KeyboardLayer<switches_count>*> layers;
    std::map<std::string, keyboard::MappingRuleSettings*> rules;

    uint16_t press_min_interval_ms{50};
    uint16_t tap_dance_max_interval_ms{150};
    uint16_t hold_min_interval_ms{200};

    explicit KeyboardSettings(std::string default_layout,
                              const std::vector<KeyboardLayer<switches_count>*>& layouts,
                              const std::vector<KeyboardLayer<switches_count>*>& layers,
                              const std::map<std::string, keyboard::MappingRuleSettings*>& rules,
                              const uint16_t update_state_interval_ms = 50,
                              const uint16_t press_min_interval_ms = 50,
                              const uint16_t tap_dance_max_interval_ms = 150,
                              const uint16_t hold_min_interval_ms = 200) :
        InputDeviceSettings(update_state_interval_ms), default_layout(std::move(default_layout)), layouts(layouts),
        layers(layers), rules(rules), press_min_interval_ms(press_min_interval_ms),
        tap_dance_max_interval_ms(tap_dance_max_interval_ms), hold_min_interval_ms(hold_min_interval_ms)
    {
    }

    ~KeyboardSettings()
    {
        for (const auto& layout : layouts)
        {
            delete layout;
        }
        for (const auto& layer : layers)
        {
            delete layer;
        }
        for (const auto& rule : std::views::values(rules))
        {
            delete rule;
        }
    }

};

}
