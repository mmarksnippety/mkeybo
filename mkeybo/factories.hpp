#pragma once

#include "components/mapping_rules.hpp"
#include "components/usb_reports.hpp"
#include "mkeybo/components/switch_events.hpp"
#include "mkeybo/components/switch_reader_matrix.hpp"
#include "mkeybo/components/actions.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/consts.hpp"
#include "components/actions/reboot_actions.hpp"


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
SwitchReaderMatrix<switches_count>* create_switch_reader_matrix(const SwitchReaderMatrixConfig& config)
{
    return new SwitchReaderMatrix<switches_count>(config.column_start_pin, config.column_count, config.row_start_pin,
                                                  config.row_count);
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

template <size_t switches_count, size_t keycodes_buffer_size>
ActionManager<switches_count, keycodes_buffer_size>* create_action_manager()
{
    return new ActionManager<switches_count, keycodes_buffer_size>({
        {Keycode{KeycodeType::action, key_action_reboot_to_bootloader},
         new actions::RebootToBootloaderAction<switches_count, keycodes_buffer_size>{}},
        {Keycode{KeycodeType::action, key_action_reboot},
         new actions::RebootAction<switches_count, keycodes_buffer_size>{}}
    });
}


}
