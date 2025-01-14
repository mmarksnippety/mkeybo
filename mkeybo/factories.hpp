#pragma once
#include "components/UsbReporter.hpp"
#include "mkeybo/components/key_mapper/key_mapper.hpp"
#include "mkeybo/components/SwitchEvents.hpp"
#include "mkeybo/components/SwitchReaderMatrix.hpp"
#include "mkeybo/components/base.hpp"
#include "tusb.h"


/**
 * Macros to simplify creating layout and layer settings in source files
 */

// null key
#define K_N() mkeybo::Keycode{}
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
KeyboardState<switches_count>* create_keyboard_state()
{
    return new KeyboardState<switches_count>();
}

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

template <size_t switches_count>
key_mapper::KeyMapper<switches_count>* create_key_mapper()
{
    return new key_mapper::KeyMapper<switches_count>({
        new key_mapper::DefaultMappingRule<switches_count>{},
        new key_mapper::TapDanceMappingRule<switches_count>{},
        new key_mapper::ChangeLayoutMappingRule<switches_count>{},
        new key_mapper::ChangeLayerMappingRule<switches_count>{},
        new key_mapper::FinalizeMappingRule<switches_count>{},
    });
}

template <size_t switches_count>
UsbReporterManager<switches_count>* create_usb_reporter_manager()
{
    return new UsbReporterManager<switches_count>({
        new UsbHidKeycodeReporter<switches_count>{},
        new UsbCcKeycodeReporter<switches_count>{}
    });
}


}
