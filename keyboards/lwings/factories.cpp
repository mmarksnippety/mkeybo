#include "config.hpp"
#include "factories.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/consts.hpp"
#include "mkeybo/components/keyboard_settings.hpp"
#include "mkeybo/components/keyboard_rule_settings/tap_dance_rule_settings.hpp"
#include "mkeybo/components/keyboard_rule_settings/multi_mapping_rule_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "tusb.h"


template <size_t switches_count>
auto create_keyboard_settings() -> mkeybo::KeyboardSettings<switches_count>*
{
    // clang-format off
    auto layout = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name{"qwerty"},
        .keycodes{
            // 1
            H_K(HID_KEY_Q), H_K(HID_KEY_W), H_K(HID_KEY_E), H_K(HID_KEY_R), H_K(HID_KEY_T),
            H_K(HID_KEY_Y), H_K(HID_KEY_U), H_K(HID_KEY_I), H_K(HID_KEY_O), H_K(HID_KEY_P),
            // 2
            H_K(HID_KEY_A), H_K(HID_KEY_S), H_K(HID_KEY_D), H_K(HID_KEY_F), H_K(HID_KEY_G),
            H_K(HID_KEY_H), H_K(HID_KEY_J), H_K(HID_KEY_K), H_K(HID_KEY_L), H_K(HID_KEY_COMMA),
            // 3
            H_K(HID_KEY_Z), H_K(HID_KEY_X), H_K(HID_KEY_C), H_K(HID_KEY_V), H_K(HID_KEY_B),
            H_K(HID_KEY_N), H_K(HID_KEY_M), H_K(HID_KEY_PERIOD), H_K(HID_KEY_SLASH), H_K(HID_KEY_ENTER),
            // 4
            N_K(), N_K(), N_K(), LAYER_K(0), H_K(HID_KEY_BACKSPACE),
            H_K(HID_KEY_SPACE), LAYER_K(1), N_K(), N_K(), N_K(),
       }
    };
    auto layer_down = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name{"down"},
        .keycodes{
            // 1
            H_K(M_LSHIFT(HID_KEY_1)), H_K(M_LSHIFT(HID_KEY_2)), H_K(M_LSHIFT(HID_KEY_3)), H_K(M_LSHIFT(HID_KEY_4)), H_K(M_LSHIFT(HID_KEY_5)),
            H_K(M_LSHIFT(HID_KEY_6)), H_K(M_LSHIFT(HID_KEY_7)), H_K(M_LSHIFT(HID_KEY_8)), H_K(M_LSHIFT(HID_KEY_9)), H_K(M_LSHIFT(HID_KEY_0)),
            // 2
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
            // 3
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
            // 4
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
        }
    };
    auto layer_up = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name{"up"},
        .keycodes{
            // 1
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
            // 2
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
            // 3
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
            // 4
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
        }
    };
    auto layer_multi = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name{"multi"},
        .keycodes{
            // 1
            H_K(HID_KEY_1), H_K(HID_KEY_2), H_K(HID_KEY_3), H_K(HID_KEY_4), H_K(HID_KEY_5),
            H_K(HID_KEY_6), H_K(HID_KEY_7), H_K(HID_KEY_8), H_K(HID_KEY_9), H_K(HID_KEY_0),
            // 2
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
            // 3
            ACTION_K(mkeybo::key_action_reboot_to_bootloader), ACTION_K(mkeybo::key_action_reboot), N_K(), N_K(), N_K(),
            N_K(), H_K (HID_KEY_ARROW_LEFT), H_K(HID_KEY_ARROW_DOWN), H_K(HID_KEY_ARROW_UP), H_K(HID_KEY_ARROW_RIGHT),
            // 4
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
        }
    };
    auto tap_dance_config = new mkeybo::keyboard_rule_settings::TapDanceRuleSettings({
        {H_K(HID_KEY_Z), {{255, H_K(HID_KEY_SHIFT_LEFT)}}},
        {H_K(HID_KEY_P), {{3, ACTION_K(mkeybo::key_action_reboot)}}}
    });
    auto multi_layer_config = new mkeybo::keyboard_rule_settings::MultiMappingRuleSettings( {
        {{LAYER_K(0), LAYER_K(1)}, LAYER_K(2)}
    });
    // clang-format on
    return new mkeybo::KeyboardSettings<switches_count>(
        "qwerty",
        {layout},
        {layer_down, layer_up, layer_multi},
        {
            {mkeybo::keyboard_rule_settings::rule_name_tap_dance, tap_dance_config},
            {mkeybo::keyboard_rule_settings::rule_name_multi_mapping, multi_layer_config},
        },
        50, // switches_refresh_interval_ms
        50, // press_min_interval_ms
        150, // tap_dance_max_interval_ms
        200, // hold_min_interval_ms
        10 // usb report interval
        );
}


template auto create_keyboard_settings<keyboard_config.switches_count>()
    -> mkeybo::KeyboardSettings<keyboard_config.switches_count>*;
