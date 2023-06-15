#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "pico/stdlib.h"

#include "tusb.h"


using namespace std;


/**
 * Usefully map of modifiers
 */

const uint8_t MODIFIER_BIT_TO_KEYCODE[] = {
    HID_KEY_CONTROL_LEFT,
    HID_KEY_SHIFT_LEFT,
    HID_KEY_ALT_LEFT,
    HID_KEY_GUI_LEFT,
    HID_KEY_CONTROL_RIGHT,
    HID_KEY_SHIFT_RIGHT,
    HID_KEY_ALT_RIGHT,
    HID_KEY_GUI_RIGHT,
};


map<uint8_t, uint8_t> MODIFIER_KEYCODE_TO_BIT = {
    {HID_KEY_CONTROL_LEFT, KEYBOARD_MODIFIER_LEFTCTRL},
    {HID_KEY_SHIFT_LEFT, KEYBOARD_MODIFIER_LEFTSHIFT},
    {HID_KEY_ALT_LEFT, KEYBOARD_MODIFIER_LEFTALT},
    {HID_KEY_GUI_LEFT, KEYBOARD_MODIFIER_LEFTGUI},
    {HID_KEY_CONTROL_RIGHT, KEYBOARD_MODIFIER_RIGHTCTRL},
    {HID_KEY_SHIFT_RIGHT, KEYBOARD_MODIFIER_RIGHTSHIFT},
    {HID_KEY_ALT_RIGHT, KEYBOARD_MODIFIER_RIGHTALT},
    {HID_KEY_GUI_RIGHT, KEYBOARD_MODIFIER_RIGHTGUI},
};


/**
 * Configuration structs and class
 */

enum KeycodeType {
    KCT_KEY,
    KCT_CC_KEY,
    KCT_LAYER_CHANGE,
    //     mouse_t,
    //     gamepad_t,
    //     internal_t,
    KCT_COUNT
};


struct Keycode {
    uint16_t code;
    KeycodeType type;
};


enum RuleConfigType {
    RCT_MIX = 1,
    RCT_KEY,
    RCT_LAYER,
};

enum RuleConfigId {
    RID_KEY = 1,
    RID_MS_KEY,
};

class RuleConfig {
public:
    const RuleConfigId rule_id;
    RuleConfigType type;

    explicit RuleConfig(RuleConfigId rule_id, RuleConfigType type = RCT_MIX)
        : rule_id(rule_id), type(type) {}

    [[nodiscard]] bool is_layer_rule() const { return type != RCT_KEY; }

    [[nodiscard]] bool is_key_rule() const { return type != RCT_LAYER; }
};


class LayerConfig {
public:
    uint8_t layer_no;
    bool is_default;
    bool is_default_transparent;
    vector<RuleConfig *> rules;

    LayerConfig(
        uint8_t layer_no, bool is_default, bool is_default_transparent, vector<RuleConfig *> const &rules)
        : layer_no(layer_no), is_default(is_default), is_default_transparent(is_default_transparent),
          rules(rules) {}
};


class MapperConfig {
public:
    vector<LayerConfig *> layers;

    explicit MapperConfig(vector<LayerConfig *> const &layers) : layers(layers) {}
};


/**
 * Specific rule
 */

class RuleConfigKey : public RuleConfig {
    using RuleConfig::type;

public:
    const uint8_t switch_no;
    const Keycode keycode;

    RuleConfigKey(uint8_t switch_no, Keycode keycode)
        : RuleConfig(RID_KEY, keycode.type == KCT_LAYER_CHANGE ? RCT_LAYER : RCT_KEY), switch_no(switch_no),
          keycode(keycode) {}
};


class RuleConfigMultiSwitchKey : public RuleConfig {
public:
    const vector<uint8_t> switches_no;
    const Keycode keycode;

    RuleConfigMultiSwitchKey(const vector<uint8_t> &switches_no, Keycode keycode)
        : RuleConfig(RID_MS_KEY, keycode.type == KCT_LAYER_CHANGE ? RCT_LAYER : RCT_KEY),
          switches_no(switches_no), keycode(keycode) {}
};


// class RuleConfigTapDance: public RuleConfig {
// public:
//     const uint8_t switch_no;
//     const vector<pair<KeycodeType, uint16_t>> keycode;
// };

// clang-format off
// rules definitions
// rules
#define R(SW, KC) new RuleConfigKey(SW, KC)
#define R2(SW_0, SW_1, KC) new RuleConfigMultiSwitchKey({SW_0, SW_1}, KC)
// standard hid keycodes
#define K(KC) Keycode { .code = KC, .type = KCT_KEY }
#define M_(MC, KC) ((static_cast<uint16_t>(MC) << 8) | KC)
#define M_LCTRL(KC) M_(KEYBOARD_MODIFIER_LEFTCTRL, KC)
#define M_LSHIFT(KC) M_(KEYBOARD_MODIFIER_LEFTSHIFT, KC)
#define M_LALT(KC) M_(KEYBOARD_MODIFIER_LEFTALT, KC)
#define M_LGUI(KC) M_(KEYBOARD_MODIFIER_LEFTGUI, KC)
#define M_RCTRL(KC) M_(KEYBOARD_MODIFIER_RIGHTCTRL, KC)
#define M_RSHIFT(KC) M_(KEYBOARD_MODIFIER_RIGHTSHIFT, KC)
#define M_RALT(KC) M_(KEYBOARD_MODIFIER_RIGHTALT, KC)
#define M_RGUI(KC) M_(KEYBOARD_MODIFIER_RIGHTGUI, KC)
// customer control keycodes
#define KCC(KC) Keycode { .code = KC, .type = KCT_CC_KEY }
// layer change keycode
#define L(KC)  Keycode { .code = KC, .type = KCT_LAYER_CHANGE }
// clang-format on
