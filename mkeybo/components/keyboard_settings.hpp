#pragma once

#include <array>
#include <map>
#include <string>
#include <vector>
#include <ranges>
#include "base.hpp"
#include "keyboard_rule_settings/base_rule_settings.hpp"


namespace mkeybo {

/**
 * Json example settings

    json_settings = {
      "default_layout": "qwerty",
      "layouts": [
        {
          "name": "qwerty",
          "keycodes": [1,2,3,4,5,6,7,8,9,10,11,12]
        },
      ],
      "layers": [
        {
          "name": "fn",
          "keycodes": [10,20,30,40,50,60,70,80,90,100,110,120]
        },
        {
          "name": "media",
          "keycodes": [0,0,0,90,91,92,0,0,0,0,0,0]
        },
      ],
      "rules": [
        {
          "type_name": "change_layer",
          "json_config": "{"keys":[10,20], "layer":1}",
        },
        {
          "type_name": "default_mapping",
          "json_config": null,
        },
      ]
    }

  JSON data like this must be parsed and instance settings object.
 */

template <size_t switches_count>
struct KeyboardSettingsLayer
{
    std::string name;
    std::array<Keycode, switches_count> keycodes;
};

template <size_t switches_count>
struct KeyboardSettings
{
    std::string default_layout;
    std::vector<KeyboardSettingsLayer<switches_count>*> layouts;
    std::vector<KeyboardSettingsLayer<switches_count>*> layers;
    std::map<std::string, keyboard_rule_settings::BaseRuleSettings*> rules;

    uint16_t switches_refresh_interval_ms{50};
    uint16_t press_min_interval_ms{50};
    uint16_t tap_dance_max_interval_ms{150};
    uint16_t hold_min_interval_ms{200};
    uint16_t report_send_interval_ms{5};

    uint8_t press_min_interval_cycles{};
    uint8_t tap_dance_max_interval_cycles{};
    uint8_t hold_min_interval_cycles{};

    explicit KeyboardSettings(std::string default_layout,
                              const std::vector<KeyboardSettingsLayer<switches_count>*>& layouts,
                              const std::vector<KeyboardSettingsLayer<switches_count>*>& layers,
                              const std::map<std::string, keyboard_rule_settings::BaseRuleSettings*>& rules,
                              const uint16_t switches_refresh_interval_ms = 50,
                              const uint16_t press_min_interval_ms = 50,
                              const uint16_t tap_dance_max_interval_ms = 150,
                              const uint16_t hold_min_interval_ms = 200,
                              const uint16_t report_send_interval_ms = 10) :
        default_layout(std::move(default_layout)), layouts(layouts), layers(layers), rules(rules),
        switches_refresh_interval_ms(switches_refresh_interval_ms), press_min_interval_ms(press_min_interval_ms),
        tap_dance_max_interval_ms(tap_dance_max_interval_ms), hold_min_interval_ms(hold_min_interval_ms),
        report_send_interval_ms(report_send_interval_ms)
    {
        update_computed_settings();
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

protected:
    void update_computed_settings()
    {
        press_min_interval_cycles = static_cast<uint8_t>(press_min_interval_ms / switches_refresh_interval_ms);
        tap_dance_max_interval_cycles = static_cast<uint8_t>(tap_dance_max_interval_ms / switches_refresh_interval_ms);
        hold_min_interval_cycles = static_cast<uint8_t>(hold_min_interval_ms / switches_refresh_interval_ms);
    }
};

template <class RuleSettings, size_t switches_count>
RuleSettings* get_rule_settings(KeyboardSettings<switches_count>* keyboard_settings, const std::string& rule_name)
{
    auto rule_settings = keyboard_settings->rules.find(rule_name);
    if (rule_settings == keyboard_settings->rules.end())
    {
        return nullptr;
    }
    return reinterpret_cast<RuleSettings*>(rule_settings->second);
}


} // namespace mkeybo
