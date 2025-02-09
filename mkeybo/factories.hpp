#pragma once

#include "components/mapping_rules/change_layer_mapping_rule.hpp"
#include "components/mapping_rules/change_layout_mapping_rule.hpp"
#include "components/mapping_rules/default_mapping_rule.hpp"
#include "components/mapping_rules/tap_dance_mapping_rule.hpp"
#include "components/mapping_rules/multi_mapping_rule.hpp"
#include "components/switch_events.hpp"
#include "components/switch_readers/matrix_switch_reader.hpp"
#include "components/actions.hpp"
#include "components/base.hpp"


/**
 * Macros to simplify creating layout and layer settings in source files
 */

// null key
#define N_K() mkeybo::Keycode{}
// hid keys
#define H_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::hid, KC}
#define M_(MC, KC) ((static_cast<uint16_t>(MC) << 8) | KC)
#define M_LCTRL(KC) M_(KEYBOARD_MODIFIER_LEFTCTRL, KC)
#define M_LSHIFT(KC) M_(KEYBOARD_MODIFIER_LEFTSHIFT, KC)
#define M_LALT(KC) M_(KEYBOARD_MODIFIER_LEFTALT, KC)
#define M_LGUI(KC) M_(KEYBOARD_MODIFIER_LEFTGUI, KC)
#define M_RCTRL(KC) M_(KEYBOARD_MODIFIER_RIGHTCTRL, KC)
#define M_RSHIFT(KC) M_(KEYBOARD_MODIFIER_RIGHTSHIFT, KC)
#define M_RALT(KC) M_(KEYBOARD_MODIFIER_RIGHTALT, KC)
#define M_RGUI(KC) M_(KEYBOARD_MODIFIER_RIGHTGUI, KC)
// customer control keys
#define CC_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::cc, KC}
// mouse
#define MOUSE_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::mouse, KC}
// gamepad
#define GAMEPAD_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::gamepad, KC}
// macro
#define MACRO_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::macro, KC}
// action
# define ACTION_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::action, KC}
// layer keys
#define LAYER_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::layer, KC}
// layout keys
#define LAYOUT_K(KC) mkeybo::Keycode{mkeybo::KeycodeType::layout, KC}


namespace mkeybo {

template <size_t switches_count>
SwitchEventsGenerator<switches_count>* create_switch_events_generator()
{
    return new SwitchEventsGenerator<switches_count>();
}

template <size_t switches_count>
switch_reader::MatrixSwitchReader<switches_count>* create_switch_reader_matrix(
    const switch_reader::MatrixSwitchReaderConfig& config)
{
    return new switch_reader::MatrixSwitchReader<switches_count>(config);
}

template <size_t switches_count, size_t keycodes_buffer_size>
std::vector<mapping_rule::BaseMappingRule<switches_count, keycodes_buffer_size>*> create_keycode_mapping_rules()
{
    return {
        new mapping_rule::DefaultMappingRule<switches_count, keycodes_buffer_size>{},
        new mapping_rule::TapDanceMappingRule<switches_count, keycodes_buffer_size>{},
        new mapping_rule::MultiMappingRule<switches_count, keycodes_buffer_size>{},
        new mapping_rule::ChangeLayoutMappingRule<switches_count, keycodes_buffer_size>{},
        new mapping_rule::ChangeLayerMappingRule<switches_count, keycodes_buffer_size>{},
    };
}

inline actions::ActionManager* create_action_manager()
{
    return new actions::ActionManager({
        {actions::action_reboot_id, new actions::ActionExecutorReboot()},
        {actions::action_reboot_to_bootloader_id, new actions::ActionExecutorRebootToBootloader()},
    });
}


}
