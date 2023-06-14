#pragma once

#include <map>
#include <string>
#include <vector>
#include <utility>

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
    KCT_LAYER_CHANGE, // ?? hmm czy to może być takie fajne i proste??
    //     mouse_t,
    //     gamepad_t,
    //     internal_t,
    KCT_COUNT
};


class RuleConfig {
public:
    const string rule_id;
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
public:
    const uint8_t switch_no;
    const uint16_t keycode;
    const KeycodeType keycode_type;

    RuleConfigKey(uint8_t switch_no, uint16_t keycode, KeycodeType keycode_type)
        : RuleConfig("key"), switch_no(switch_no), keycode(keycode), keycode_type(keycode_type) {}
};

#define K(SW, KC) new RuleConfigKey(SW, KC, KCT_KEY)
#define M_(MC, KC) ((static_cast<uint16_t>(MC) << 8) | KC)
#define M_LCTRL(KC) M_(KEYBOARD_MODIFIER_LEFTCTRL, KC)
#define M_LSHIFT(KC) M_(KEYBOARD_MODIFIER_LEFTSHIFT, KC)
#define M_LALT(KC) M_(KEYBOARD_MODIFIER_LEFTALT, KC)
#define M_LGUI(KC) M_(KEYBOARD_MODIFIER_LEFTGUI, KC)
#define M_RCTRL(KC) M_(KEYBOARD_MODIFIER_RIGHTCTRL, KC)
#define M_RSHIFT(KC) M_(KEYBOARD_MODIFIER_RIGHTSHIFT, KC)
#define M_RALT(KC) M_(KEYBOARD_MODIFIER_RIGHTALT, KC)
#define M_RGUI(KC) M_(KEYBOARD_MODIFIER_RIGHTGUI, KC)


class RuleConfigLayerSwitch : public RuleConfig {
public:
    const uint8_t layer_no;
    const vector<uint8_t> switches_no;

    RuleConfigLayerSwitch(const uint8_t layer_no, const vector<uint8_t> &switches_no)
        : RuleConfig("layer_switch"), layer_no(layer_no), switches_no(switches_no) {}
};

#define LA(SW, LA_N) new RuleConfigLayerSwitch(LA_N, {SW})
#define LA_2B(SW_1, SW_2, LA_N) new RuleConfigLayerSwitch(LA_N, {SW_1, SW_2})


//class RuleConfigTapDance: public RuleConfig {
//public:
//    const uint8_t switch_no;
//    const vector<pair<KeycodeType, uint16_t>> keycode;
//};
