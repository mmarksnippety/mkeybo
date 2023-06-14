#pragma once

#include "pico/stdlib.h"
#include <limits>
#include <map>
#include <ranges>
#include <string>
#include <utility>

#include "KeyboardConfig.h"
#include "KeyboardState.h"

using namespace std;
using namespace std::ranges;


template <uint8_t switches_count>
class MapperRule {
protected:
    KeyboardSettings const *keyboard_settings_;
    KeyboardState<switches_count> *keyboard_state_;

public:
    MapperRule(KeyboardSettings const *keyboard_settings, KeyboardState<switches_count> *keyboard_state)
        : keyboard_settings_(keyboard_settings), keyboard_state_(keyboard_state) {}

    virtual bool match_key_mapping(RuleConfig *rule_config) { return false; }

    virtual bool make_key_mapping(RuleConfig *rule_config) { return false; }

    virtual bool match_layer_mapping(RuleConfig *rule_config) { return false; }

    virtual bool make_layer_mapping(RuleConfig *rule_config) { return false; }
};


template <uint8_t switches_count>
class Mapper {
private:
    MapperConfig const *mapper_config_;
    KeyboardSettings const *keyboard_settings_;
    KeyboardState<switches_count> *keyboard_state_;
    map<string, MapperRule<switches_count> *> rules_;

public:
    explicit Mapper(
        MapperConfig const *mapper_config,
        KeyboardSettings const *keyboard_settings,
        KeyboardState<switches_count> *keyboard_state,
        map<string, MapperRule<switches_count> *> const &rules)
        : mapper_config_(mapper_config), keyboard_settings_(keyboard_settings),
          keyboard_state_(keyboard_state), rules_(rules) {}

    void setup_layer_state() {
        bool is_active = true;
        for (auto layer : mapper_config_->layers) {
            auto layer_state = LayerState{.is_active = is_active};
            keyboard_state_->layers[layer->layer_no] = layer_state;
            is_active = false;
        }
    }

    void reset_layer_state() {
        if (keyboard_state_->layers.empty()) {
            setup_layer_state();
        }
        for (auto layer : mapper_config_->layers) {
            // TODO, toggled layers, one shot layer (is this usefully?)
            if (!layer->is_default) {
                keyboard_state_->layers[layer->layer_no].is_active = false;
            }
        }
    }

    bool update_layer_state() {
        for (auto &layer : views::reverse(mapper_config_->layers)) {
            if (keyboard_state_->layers[layer->layer_no].is_active) {
                for (auto &rule_config : layer->rules) {
                    auto mapper_rule = rules_[rule_config->rule_id];
                    if (mapper_rule->match_layer_mapping(rule_config)) {
                        mapper_rule->make_layer_mapping(rule_config);
                    }
                }
            }
        }
        return false;
    }

    bool update_key_state() {
        for (auto &layer : views::reverse(mapper_config_->layers)) {
            if (keyboard_state_->layers[layer->layer_no].is_active) {
                bool is_any_key_mapped = false;
                for (auto &rule_config : layer->rules) {
                    auto mapper_rule = rules_[rule_config->rule_id];
                    if (mapper_rule->match_key_mapping(rule_config)) {
                        if (mapper_rule->make_key_mapping(rule_config)) {
                            is_any_key_mapped = true;
                        }
                    }
                }
                if (is_any_key_mapped) {
                    keyboard_state_->mark_layer_used(layer->layer_no);
                }
            }
        }
        return false;
    }

    void make_layer_mapping() {
        reset_layer_state();
        update_layer_state();
    }

    void make_mapping() { update_key_state(); }
};


// implementations of detail rules


template <uint8_t switches_count>
class MapperRuleKey : public MapperRule<switches_count> {
    using MapperRule<switches_count>::keyboard_state_;
    using MapperRule<switches_count>::keyboard_settings_;

public:
    MapperRuleKey(KeyboardSettings const *keyboard_settings, KeyboardState<switches_count> *keyboard_state)
        : MapperRule<switches_count>(keyboard_settings, keyboard_state) {}

    bool match_key_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        return !keyboard_state_->is_switch_mapped(config->switch_no) &&
               keyboard_state_->is_switch_pressed(config->switch_no, keyboard_settings_);
    }

    bool make_key_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        if (config->keycode > 0) {
            keyboard_state_->mark_switch_mapped(config->switch_no);
            if (config->keycode_type == KCT_KEY && config->keycode > numeric_limits<uint8_t>::max()) {
                // base keycode
                keyboard_state_->emplace_keycode(
                    config->keycode_type, static_cast<uint8_t>(config->keycode));
                // modifiers
                auto modifier_bits = static_cast<uint8_t>(config->keycode >> 8);
                uint8_t modifier_mask = 1;
                for (auto &keycode : MODIFIER_BIT_TO_KEYCODE) {
                    if (modifier_bits & modifier_mask) {
                        keyboard_state_->emplace_keycode(config->keycode_type, keycode);
                    }
                    modifier_mask <<= 1;
                }
            } else {
                keyboard_state_->emplace_keycode(config->keycode_type, config->keycode);
            }
            return true;
        }
        return false;
    }
};


template <uint8_t switches_count>
class MapperRuleLayerSwitch : public MapperRule<switches_count> {
    using MapperRule<switches_count>::keyboard_state_;
    using MapperRule<switches_count>::keyboard_settings_;

public:
    MapperRuleLayerSwitch(
        KeyboardSettings const *keyboard_settings, KeyboardState<switches_count> *keyboard_state)
        : MapperRule<switches_count>(keyboard_settings, keyboard_state) {}

    bool match_layer_mapping(RuleConfig *rule_config) override {
        return are_switches_pressed(reinterpret_cast<RuleConfigLayerSwitch *>(rule_config));
    }

    bool make_layer_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigLayerSwitch *>(rule_config);
        mark_switches_mapped(config);
        keyboard_state_->activate_layer(reinterpret_cast<RuleConfigLayerSwitch *>(config)->layer_no);
        return true;
    }

    bool are_switches_pressed(RuleConfigLayerSwitch *rule_config) {
        for (auto &switch_no : rule_config->switches_no) {
            if (keyboard_state_->is_switch_mapped(switch_no)) {
                return false;
            }
            if (!keyboard_state_->is_switch_pressed(switch_no, keyboard_settings_)) {
                return false;
            }
        }
        return true;
    }

    void mark_switches_mapped(RuleConfigLayerSwitch *rule_config) {
        for (auto &switch_no : rule_config->switches_no) {
            keyboard_state_->mark_switch_mapped(switch_no);
        }
    }
};
