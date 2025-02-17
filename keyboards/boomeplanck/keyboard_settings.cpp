#include <memory>
#include "config.hpp"
#include "keyboard_settings.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/components/actions.hpp"
#include "mkeybo/components/keyboard/keyboard_settings.hpp"
#include "mkeybo/components/keyboard/mapping_rules/tap_dance_mapping_rule_settings.hpp"
#include "mkeybo/components/keyboard/mapping_rules/multi_mapping_rule_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "tusb.h"


std::unique_ptr<mkeybo::keyboard::KeyboardSettings<keyboard_config.switches_count>> create_keyboard_settings()
{
    // clang-format off
    auto layout = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
        .name{"qwerty"},
        .keycodes{
            // 0 ~ 11
            H_K(HID_KEY_ESCAPE), H_K(HID_KEY_Q), H_K(HID_KEY_W), H_K(HID_KEY_E),
            H_K(HID_KEY_R), H_K(HID_KEY_T), H_K(HID_KEY_Y), H_K(HID_KEY_U),
            H_K(HID_KEY_I), H_K(HID_KEY_O), H_K(HID_KEY_P), H_K(HID_KEY_BACKSPACE),
            // 12 ~ 23
            H_K(HID_KEY_TAB), H_K(HID_KEY_A), H_K(HID_KEY_S), H_K(HID_KEY_D),
            H_K(HID_KEY_F), H_K(HID_KEY_G), H_K(HID_KEY_H), H_K(HID_KEY_J),
            H_K(HID_KEY_K), H_K(HID_KEY_L), H_K(HID_KEY_SEMICOLON), H_K(HID_KEY_APOSTROPHE),
            // 24 ~ 35
            H_K(HID_KEY_SHIFT_LEFT), H_K(HID_KEY_Z), H_K(HID_KEY_X), H_K(HID_KEY_C),
            H_K(HID_KEY_V), H_K(HID_KEY_B), H_K(HID_KEY_N), H_K(HID_KEY_M),
            H_K(HID_KEY_COMMA), H_K(HID_KEY_PERIOD), H_K(HID_KEY_SLASH), H_K(HID_KEY_ENTER),
            // 36 ~ 47
            H_K(HID_KEY_CONTROL_LEFT), LAYER_K(3), H_K(HID_KEY_GUI_LEFT), H_K(HID_KEY_ALT_LEFT),
            LAYER_K(0), H_K(HID_KEY_SPACE), H_K(HID_KEY_SPACE), LAYER_K(1),
            H_K(HID_KEY_ARROW_LEFT), H_K(HID_KEY_ARROW_DOWN), H_K(HID_KEY_ARROW_UP), H_K(HID_KEY_ARROW_RIGHT)
        }
    };
    auto layer_down = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
        .name{"down"},
        .keycodes{
            // 0 ~ 11
            H_K(M_LSHIFT(HID_KEY_GRAVE)), H_K(M_LSHIFT(HID_KEY_1)), H_K(M_LSHIFT(HID_KEY_2)), H_K(M_LSHIFT(HID_KEY_3)),
            H_K(M_LSHIFT(HID_KEY_4)), H_K(M_LSHIFT(HID_KEY_5)), H_K(M_LSHIFT(HID_KEY_6)), H_K(M_LSHIFT(HID_KEY_7)),
            H_K(M_LSHIFT(HID_KEY_8)), H_K(M_LSHIFT(HID_KEY_9)), H_K(M_LSHIFT(HID_KEY_0)), H_K(HID_KEY_DELETE),
            // 12 - 23
            H_K(HID_KEY_DELETE), H_K(HID_KEY_F1), H_K(HID_KEY_F2), H_K(HID_KEY_F3),
            H_K(HID_KEY_F4), H_K(HID_KEY_F5), H_K(HID_KEY_F6), H_K(M_LSHIFT(HID_KEY_MINUS)),
            H_K(M_LSHIFT(HID_KEY_EQUAL)), H_K(M_LSHIFT(HID_KEY_BRACKET_LEFT)), H_K(M_LSHIFT(HID_KEY_BRACKET_RIGHT)), H_K(M_LSHIFT(HID_KEY_BACKSLASH)),
            // 24 ~ 35
            N_K(), H_K(HID_KEY_F7), H_K(HID_KEY_F8), H_K(HID_KEY_F9),
            H_K(HID_KEY_F10), H_K(HID_KEY_F11), H_K(HID_KEY_F12), H_K(M_LSHIFT(HID_KEY_BACKSLASH)),
            H_K(HID_KEY_SLASH), N_K(), N_K(), N_K(),
            // 36 ~ 47
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K()
        }
    };
    auto layer_up = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
        .name{"up"},
        .keycodes{
            // 0 ~ 11
            H_K(HID_KEY_GRAVE), H_K(HID_KEY_1), H_K(HID_KEY_2), H_K(HID_KEY_3),
            H_K(HID_KEY_4), H_K(HID_KEY_5), H_K(HID_KEY_6), H_K(HID_KEY_7),
            H_K(HID_KEY_8), H_K(HID_KEY_9), H_K(HID_KEY_0), N_K(),
            // 12 - 23
            H_K(HID_KEY_DELETE), H_K(HID_KEY_F1), H_K(HID_KEY_F2), H_K(HID_KEY_F3),
            H_K(HID_KEY_F4), H_K(HID_KEY_F5), H_K(HID_KEY_F6), H_K(HID_KEY_MINUS),
            H_K(HID_KEY_EQUAL), H_K(HID_KEY_BRACKET_LEFT), H_K(HID_KEY_BRACKET_RIGHT), H_K(HID_KEY_BACKSLASH),
            // 24 ~ 35
            N_K(), H_K(HID_KEY_F7), H_K(HID_KEY_F8), H_K(HID_KEY_F9),
            H_K(HID_KEY_F10), H_K(HID_KEY_F11), H_K(HID_KEY_F12), H_K(HID_KEY_BACKSLASH),
            H_K(HID_KEY_SLASH), N_K(), N_K(), N_K(),
            // 36 ~ 47
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K()
        }
    };
    auto layer_navi = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
        .name{"navi"},
        .keycodes{
            // 0 ~ 11
            ACTION_K(mkeybo::actions::action_reboot_to_bootloader_id), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), ACTION_K(mkeybo::actions::action_reboot_id),
            // 12 - 23
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), CC_K(HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT),
            CC_K(HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT), N_K(), N_K(), N_K(),
            // 24 ~ 35
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), CC_K(HID_USAGE_CONSUMER_MUTE),
            N_K(), N_K(), N_K(), N_K(),
            // 36 ~ 47
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            CC_K(HID_USAGE_CONSUMER_SCAN_PREVIOUS), CC_K(HID_USAGE_CONSUMER_VOLUME_DECREMENT),
            CC_K(HID_USAGE_CONSUMER_VOLUME_INCREMENT), CC_K(HID_USAGE_CONSUMER_SCAN_NEXT)
        }
    };
    auto layer_spec = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
        .name{"spec"},
        .keycodes{
            // 0 ~ 11
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            // 12 - 23
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            // 24 ~ 35
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(),
            // 36 ~ 47
            H_K(HID_KEY_CONTROL_RIGHT), N_K(), H_K(HID_KEY_APPLICATION), H_K(HID_KEY_ALT_RIGHT),
            N_K(), H_K(HID_KEY_BACKSPACE), H_K(HID_KEY_DELETE), N_K(),
            H_K(HID_KEY_HOME), H_K(HID_KEY_PAGE_DOWN), H_K(HID_KEY_PAGE_UP), H_K(HID_KEY_END)
        }
    };
    auto multi_map_config = new mkeybo::keyboard::mapping_rule::MultiMappingRuleSettings( {
        {{LAYER_K(0), LAYER_K(1)}, LAYER_K(2)}
    });
    // clang-format on
    return std::unique_ptr<mkeybo::keyboard::KeyboardSettings<keyboard_config.switches_count>>(
        new mkeybo::keyboard::KeyboardSettings<keyboard_config.switches_count>(
        "qwerty",
        {layout},
        {layer_down, layer_up, layer_navi, layer_spec},
        {
            {mkeybo::keyboard::mapping_rule::rule_name_multi_mapping, multi_map_config},
        },
        50, // switches_refresh_interval_ms
        50, // press_min_interval_ms
        150, // tap_dance_max_interval_ms
        200 // hold_min_interval_ms
    ));

}

