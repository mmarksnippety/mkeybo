#pragma once

#include "KeyboardConfig.h"
#include "KeyboardState.h"
#include "pico/stdlib.h"
#include <limits>
#include <map>
#include <ranges>
#include <string>
#include <utility>


using namespace std;
using namespace std::ranges;


template <uint8_t switches_count>
class MapperRule {
public:
    const RuleConfigId rule_id;

protected:
    KeyboardSettings const *keyboard_settings_;
    KeyboardState<switches_count> *keyboard_state_;

public:
    MapperRule(
        RuleConfigId const rule_id,
        KeyboardSettings const *keyboard_settings,
        KeyboardState<switches_count> *keyboard_state)
        : rule_id(rule_id), keyboard_settings_(keyboard_settings), keyboard_state_(keyboard_state) {}

    virtual bool match_key_mapping(RuleConfig *rule_config) { return false; }

    virtual bool make_key_mapping(RuleConfig *rule_config) { return false; }

    virtual bool match_layer_mapping(RuleConfig *rule_config) { return false; }

    virtual bool make_layer_mapping(RuleConfig *rule_config) { return false; }

    bool emplace_keycode(Keycode const &keycode) {
        if (keycode.code > 0) {
            if (keycode.type == KCT_KEY && keycode.code > numeric_limits<uint8_t>::max()) {
                // base keycode
                keyboard_state_->emplace_keycode(keycode.type, static_cast<uint8_t>(keycode.code));
                // modifiers
                auto modifier_bits = static_cast<uint8_t>(keycode.code >> 8);
                uint8_t modifier_mask = 1;
                for (auto &kc : MODIFIER_BIT_TO_KEYCODE) {
                    if (modifier_bits & modifier_mask) {
                        keyboard_state_->emplace_keycode(keycode.type, kc);
                    }
                    modifier_mask <<= 1;
                }
            } else {
                keyboard_state_->emplace_keycode(keycode.type, keycode.code);
            }
            //
            //            cout << "Emplace keycode: " << to_string(keycode.type) << "|" <<
            //            to_string(keycode.code) << endl;
            //

            return true;
        }
        return false;
    }

    bool activate_layer(Keycode const &keycode) {
        keyboard_state_->activate_layer(keycode.code);
        return true;
    }
};


template <uint8_t switches_count>
class Mapper {
private:
    MapperConfig const *mapper_config_;
    KeyboardSettings const *keyboard_settings_;
    KeyboardState<switches_count> *keyboard_state_;
    map<RuleConfigId, MapperRule<switches_count> *> rules_;

public:
    explicit Mapper(
        MapperConfig const *mapper_config,
        KeyboardSettings const *keyboard_settings,
        KeyboardState<switches_count> *keyboard_state,
        vector<MapperRule<switches_count> *> const &rules_list)
        : mapper_config_(mapper_config), keyboard_settings_(keyboard_settings),
          keyboard_state_(keyboard_state) {
        for (auto &rule : rules_list) {
            rules_[rule->rule_id] = rule;
        }
    }

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
                    auto &mapper_rule = rules_[rule_config->rule_id];
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


template <uint8_t switches_count>
class MapperRuleKey : public MapperRule<switches_count> {
    using MapperRule<switches_count>::keyboard_state_;
    using MapperRule<switches_count>::keyboard_settings_;
    using MapperRule<switches_count>::emplace_keycode;
    using MapperRule<switches_count>::activate_layer;

public:
    MapperRuleKey(KeyboardSettings const *keyboard_settings, KeyboardState<switches_count> *keyboard_state)
        : MapperRule<switches_count>(RID_KEY, keyboard_settings, keyboard_state) {}

    bool match_key_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        return config->is_key_rule() && !keyboard_state_->is_switch_mapped(config->switch_no) &&
               keyboard_state_->is_switch_pressed(config->switch_no, keyboard_settings_);
    }

    bool make_key_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        if (emplace_keycode(config->keycode)) {
            keyboard_state_->mark_switch_mapped(config->switch_no);
            return true;
        }
        return false;
    }

    bool match_layer_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        return config->is_layer_rule() && !keyboard_state_->is_switch_mapped(config->switch_no) &&
               keyboard_state_->is_switch_pressed(config->switch_no, keyboard_settings_);
    }

    bool make_layer_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        if (activate_layer(config->keycode)) {
            keyboard_state_->mark_switch_mapped(config->switch_no);
            return true;
        }
        return false;
    }
};


// Tap dance is different from "classic" mapping, we must wait some tap dance interval,
// and emit keycode when key switch is of yet.
// The consequence of this fact is that we must always check this rule, even when the switch is inactive.
template <uint8_t switches_count>
class MapperRuleTapDanceKey : public MapperRule<switches_count> {
    using MapperRule<switches_count>::keyboard_state_;
    using MapperRule<switches_count>::keyboard_settings_;
    using MapperRule<switches_count>::emplace_keycode;
    using MapperRule<switches_count>::activate_layer;

public:
    MapperRuleTapDanceKey(
        KeyboardSettings const *keyboard_settings, KeyboardState<switches_count> *keyboard_state)
        : MapperRule<switches_count>(RID_TD, keyboard_settings, keyboard_state) {}

    bool match_key_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        return config->is_key_rule() && !keyboard_state_->is_switch_mapped(config->switch_no) &&
               keyboard_state_->is_tap_dance_end(config->switch_no);
    }

    bool make_key_mapping(RuleConfig *rule_config) override {
        //        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
        //        auto td_end = keyboard_state_->is_tap_dance_end(config->switch_no);
        //        cout << to_string(td_end) << "|" << to_string(td_count) << endl;
        auto config = reinterpret_cast<RuleConfigTapDanceKey *>(rule_config);
        auto td_count = keyboard_state_->get_tap_dance_count(config->switch_no);
        if (config->keycode.contains(td_count)) {
            if (emplace_keycode(config->keycode.at(td_count))) {
                keyboard_state_->mark_switch_mapped(config->switch_no);
                return true;
            }
        }
        return false;
    }

    //    bool match_layer_mapping(RuleConfig *rule_config) override {
    //        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
    //        return config->is_layer_rule() && !keyboard_state_->is_switch_mapped(config->switch_no);
    //    }
    //
    //    bool make_layer_mapping(RuleConfig *rule_config) override {
    //        auto config = reinterpret_cast<RuleConfigKey *>(rule_config);
    //        if (activate_layer(config->keycode)) {
    //            keyboard_state_->mark_switch_mapped(config->switch_no);
    //            return true;
    //        }
    //        return false;
    //    }
};


template <uint8_t switches_count>
class MapperRuleMultiSwitchKey : public MapperRule<switches_count> {
    using MapperRule<switches_count>::keyboard_state_;
    using MapperRule<switches_count>::keyboard_settings_;
    using MapperRule<switches_count>::emplace_keycode;
    using MapperRule<switches_count>::activate_layer;

public:
    MapperRuleMultiSwitchKey(
        KeyboardSettings const *keyboard_settings, KeyboardState<switches_count> *keyboard_state)
        : MapperRule<switches_count>(RID_MS_KEY, keyboard_settings, keyboard_state) {}

    bool match_key_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigMultiSwitchKey *>(rule_config);
        return config->is_key_rule() && are_switches_pressed(config);
    }

    bool make_key_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigMultiSwitchKey *>(rule_config);
        if (emplace_keycode(config->keycode)) {
            mark_switches_mapped(config);
            return true;
        }
        return false;
    }

    bool match_layer_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigMultiSwitchKey *>(rule_config);
        return config->is_layer_rule() && are_switches_pressed(config);
    }

    bool make_layer_mapping(RuleConfig *rule_config) override {
        auto config = reinterpret_cast<RuleConfigMultiSwitchKey *>(rule_config);
        if (activate_layer(config->keycode)) {
            mark_switches_mapped(config);
            return true;
        }
        return false;
    }

    bool are_switches_pressed(RuleConfigMultiSwitchKey *rule_config) {
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

    void mark_switches_mapped(RuleConfigMultiSwitchKey *rule_config) {
        for (auto &switch_no : rule_config->switches_no) {
            keyboard_state_->mark_switch_mapped(switch_no);
        }
    }
};
