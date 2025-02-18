#include <memory>
#include "factories.hpp"
#include "config.hpp"
#include "mkeybo/factories.hpp"
#include "mkeybo/components/keyboard/keyboard_settings.hpp"
#include "mkeybo/components/keyboard/mapping_rules/tap_dance_mapping_rule_settings.hpp"
#include "mkeybo/components/keyboard/mapping_rules/multi_mapping_rule_settings.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/actions.hpp"
#include "tusb.h"


std::unique_ptr<mkeybo::keyboard::KeyboardSettings<keyboard_config.switches_count>> create_keyboard_settings()
{
    // clang-format off
    auto layout = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
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
    auto layer_down = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
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
    auto layer_up = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
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
    auto layer_multi = new mkeybo::keyboard::KeyboardLayer<keyboard_config.switches_count>{
        .name{"multi"},
        .keycodes{
            // 1
            H_K(HID_KEY_1), H_K(HID_KEY_2), H_K(HID_KEY_3), H_K(HID_KEY_4), H_K(HID_KEY_5),
            H_K(HID_KEY_6), H_K(HID_KEY_7), H_K(HID_KEY_8), H_K(HID_KEY_9), H_K(HID_KEY_0),
            // 2
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), H_K(HID_KEY_D),
            // 3
            ACTION_K(mkeybo::actions::action_reboot_to_bootloader_id), ACTION_K(mkeybo::actions::action_reboot_id), N_K(), N_K(), N_K(),
            N_K(), H_K (HID_KEY_ARROW_LEFT), H_K(HID_KEY_ARROW_DOWN), H_K(HID_KEY_ARROW_UP), H_K(HID_KEY_ARROW_RIGHT),
            // 4
            N_K(), N_K(), N_K(), N_K(), N_K(),
            N_K(), N_K(), N_K(), N_K(), N_K(),
        }
    };
    auto tap_dance_config = new mkeybo::keyboard::mapping_rule::TapDanceMappingRuleSettings({
        {H_K(HID_KEY_Z), {{255, H_K(HID_KEY_SHIFT_LEFT)}}},
        {H_K(HID_KEY_D), {{3, ACTION_K(mkeybo::actions::action_reboot_id)}}}
    });
    auto multi_layer_config = new mkeybo::keyboard::mapping_rule::MultiMappingRuleSettings( {
        {{LAYER_K(0), LAYER_K(1)}, LAYER_K(2)}
    });
    // clang-format on
    return std::unique_ptr<mkeybo::keyboard::KeyboardSettings<keyboard_config.switches_count>>(
        new mkeybo::keyboard::KeyboardSettings<keyboard_config.switches_count>(
            "qwerty",
            {layout},
            {layer_down, layer_up, layer_multi},
            {
                {mkeybo::keyboard::mapping_rule::rule_name_tap_dance, tap_dance_config},
                {mkeybo::keyboard::mapping_rule::rule_name_multi_mapping, multi_layer_config},
            },
            50,
            50,
            150,
            200
            )
        );
}


std::unique_ptr<mkeybo::HidControllerSettings> create_hid_controller_settings()
{
    return std::make_unique<mkeybo::HidControllerSettings>(10);
}


Keyboard<keyboard_config.switches_count>* create_keyboard()
{
    const auto keyboard = new Keyboard<keyboard_config.switches_count>();
    keyboard->apply_settings(create_keyboard_settings());
    return keyboard;
}


StatusDisplay<keyboard_config.switches_count>* create_status_display(Keyboard<keyboard_config.switches_count>* keyboard)
{
    return new StatusDisplay<keyboard_config.switches_count>(display_config, keyboard);
}


mkeybo::HidController* create_hid_controller(Keyboard<keyboard_config.switches_count>* keyboard,
                                             StatusDisplay<keyboard_config.switches_count>* status_display)
{
    const auto hid_controller = new mkeybo::HidController(
        keyboard_config.keyboard_name,
        keyboard_config.manufactured_name,
        {
            {mkeybo::actions::action_reboot_id, new mkeybo::actions::ActionExecutorReboot()},
            {mkeybo::actions::action_reboot_to_bootloader_id, new mkeybo::actions::ActionExecutorRebootToBootloader()},
        },
        {keyboard},
        {status_display},
        create_hid_controller_settings()
        );
    return hid_controller;
}
