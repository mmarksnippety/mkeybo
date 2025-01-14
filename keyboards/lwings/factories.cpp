#include "mkeybo/factories.hpp"
#include "config.hpp"
#include "factories.hpp"
#include "mkeybo/components/keyboard_settings.hpp"
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
            K_N(), K_N(), K_N(), LAYER_K(0), H_K(HID_KEY_SPACE),
            H_K(HID_KEY_BACKSPACE), LAYER_K(1), K_N(), K_N(), K_N(),
       }
    };
    auto layer_down = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name{"down"},
        .keycodes{
            // 1
            H_K(M_LSHIFT(HID_KEY_1)), H_K(M_LSHIFT(HID_KEY_2)), H_K(M_LSHIFT(HID_KEY_3)), H_K(M_LSHIFT(HID_KEY_4)), H_K(M_LSHIFT(HID_KEY_5)),
            H_K(M_LSHIFT(HID_KEY_6)), H_K(M_LSHIFT(HID_KEY_7)), H_K(M_LSHIFT(HID_KEY_8)), H_K(M_LSHIFT(HID_KEY_9)), H_K(M_LSHIFT(HID_KEY_0)),
            // 2
            K_N(), K_N(), K_N(), K_N(), K_N(),
            K_N(), K_N(), K_N(), K_N(), K_N(),
            // 3
            K_N(), K_N(), K_N(), K_N(), K_N(),
            K_N(), K_N(), K_N(), K_N(), K_N(),
        }
    };
    auto layer_up = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name{"down"},
        .keycodes{
            // 1
            H_K(HID_KEY_1), H_K(HID_KEY_2), H_K(HID_KEY_3), H_K(HID_KEY_4), H_K(HID_KEY_5),
            H_K(HID_KEY_6), H_K(HID_KEY_7), H_K(HID_KEY_8), H_K(HID_KEY_9), H_K(HID_KEY_0),
            // 2
            K_N(), K_N(), K_N(), K_N(), K_N(),
            K_N(), K_N(), K_N(), K_N(), K_N(),
            // 3
            K_N(), K_N(), K_N(), K_N(), K_N(),
            K_N(), K_N(), K_N(), K_N(), K_N(),
        }
    };
    auto tap_dance_key_z = std::map<uint8_t, mkeybo::Keycode>{{255, H_K(HID_KEY_SHIFT_LEFT)}};
    auto tap_dance_config = new mkeybo::KeyboardSettingsTapDanceRuleConfig({
        {H_K(HID_KEY_Z), {{255, H_K(HID_KEY_SHIFT_LEFT)}} }
    });
    // clang-format on
    return new mkeybo::KeyboardSettings<switches_count>("qwerty", {layout}, {layer_down, layer_up},
                                                        {{"tap_dance", tap_dance_config}});
}


template auto create_keyboard_settings<keyboard_config.switches_count>()
    -> mkeybo::KeyboardSettings<keyboard_config.switches_count>*;
