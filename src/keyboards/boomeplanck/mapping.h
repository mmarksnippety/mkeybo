#pragma once

#include "components/KeyboardConfig.h"


MapperConfig *mapper_config_factory() {
    // clang-format off
    return new MapperConfig({
        new LayerConfig(0, true, true, {
            LA_2B(40, 43, 3),
            // R0, 0 ~ 11
            K(0, HID_KEY_TAB), K(1, HID_KEY_Q), K(2, HID_KEY_W), K(3, HID_KEY_E),
            K(4, HID_KEY_R), K(5, HID_KEY_T), K(6, HID_KEY_Y), K(7, HID_KEY_U),
            K(8, HID_KEY_I), K(9, HID_KEY_O), K(10, HID_KEY_P), K(11, HID_KEY_BACKSPACE),
            // R1, 12 ~ 23
            K(12, HID_KEY_ESCAPE), K(13, HID_KEY_A), K(14, HID_KEY_S), K(15, HID_KEY_D),
            K(16, HID_KEY_F), K(17, HID_KEY_G), K(18, HID_KEY_H), K(19, HID_KEY_J),
            K(20, HID_KEY_K), K(21, HID_KEY_L), K(22, HID_KEY_SEMICOLON), K(23, HID_KEY_APOSTROPHE),
            // R2, 24 ~ 35
            K(24, HID_KEY_SHIFT_LEFT), K(25, HID_KEY_Z), K(26, HID_KEY_X), K(27, HID_KEY_C),
            K(28, HID_KEY_V), K(29, HID_KEY_B), K(30, HID_KEY_N), K(31, HID_KEY_M),
            K(32, HID_KEY_COMMA), K(33, HID_KEY_PERIOD), K(34, HID_KEY_SLASH), K(35, HID_KEY_ENTER),
            // R3, 36 ~ 47
            K(36, HID_KEY_CONTROL_LEFT), LA(37, 4), K(38, HID_KEY_ALT_LEFT), K(39, HID_KEY_GUI_LEFT),
            LA(40, 1), K(41, HID_KEY_SPACE), K(42, HID_KEY_SPACE), LA(43, 2),
            K(44, HID_KEY_ARROW_LEFT), K(45, HID_KEY_ARROW_DOWN), K(46, HID_KEY_ARROW_UP), K(47, HID_KEY_ARROW_RIGHT)
        }),
        new LayerConfig(1, false, true, {
            // 0 ~ 11
            K(0, M_LSHIFT(HID_KEY_GRAVE)), K(1, M_LSHIFT(HID_KEY_1)), K(2, M_LSHIFT(HID_KEY_2)), K(3, M_LSHIFT(HID_KEY_3)),
            K(4, M_LSHIFT(HID_KEY_4)), K(5, M_LSHIFT(HID_KEY_5)), K(6, M_LSHIFT(HID_KEY_6)), K(7, M_LSHIFT(HID_KEY_7)),
            K(8, M_LSHIFT(HID_KEY_8)), K(9, M_LSHIFT(HID_KEY_9)), K(10, M_LSHIFT(HID_KEY_0)),
            // 12 - 23
            K(12, HID_KEY_DELETE), K(13, HID_KEY_F1), K(14, HID_KEY_F2), K(15, HID_KEY_F3),
            K(16, HID_KEY_F4), K(17, HID_KEY_F5), K(18, HID_KEY_F6), K(19, M_LSHIFT(HID_KEY_MINUS)),
            K(20, M_LSHIFT(HID_KEY_EQUAL)), K(21, M_LSHIFT(HID_KEY_BRACKET_LEFT)), K(22, M_LSHIFT(HID_KEY_BRACKET_RIGHT)), K(23, M_LSHIFT(HID_KEY_BACKSLASH)),
            // 24 ~ 35
            K(25, HID_KEY_F7), K(26, HID_KEY_F8), K(27, HID_KEY_F9),
            K(28, HID_KEY_F10), K(29, HID_KEY_F11), K(30, HID_KEY_F12), K(31, M_LSHIFT(HID_KEY_BACKSLASH)), K(32, HID_KEY_SLASH)
            // 36 ~ 47
        }),
        new LayerConfig(2, false, true, {
            // 0 ~ 11
            K(0, HID_KEY_GRAVE), K(1, HID_KEY_1), K(2, HID_KEY_2), K(3, HID_KEY_3),
            K(4, HID_KEY_4), K(5, HID_KEY_5), K(6, HID_KEY_6), K(7, HID_KEY_7),
            K(8, HID_KEY_8), K(9, HID_KEY_9), K(10, HID_KEY_0),
            // 12 - 23
            K(12, HID_KEY_DELETE), K(13, HID_KEY_F1), K(14, HID_KEY_F2), K(15, HID_KEY_F3),
            K(16, HID_KEY_F4), K(17, HID_KEY_F5), K(18, HID_KEY_F6), K(19, HID_KEY_MINUS),
            K(20, HID_KEY_EQUAL), K(21, HID_KEY_BRACKET_LEFT), K(22, HID_KEY_BRACKET_RIGHT), K(23, HID_KEY_BACKSLASH),
            // 24 ~ 35
            K(25, HID_KEY_F7), K(26, HID_KEY_F8), K(27, HID_KEY_F9),
            K(28, HID_KEY_F10), K(29, HID_KEY_F11), K(30, HID_KEY_F12), K(31, HID_KEY_BACKSLASH), K(32, HID_KEY_SLASH)
            // 36 ~ 47
        }),
        new LayerConfig(3, false, true, {}),
        new LayerConfig(4, false, true, {
            // 36 ~ 47
            K(36, HID_KEY_CONTROL_RIGHT), K(38, HID_KEY_ALT_RIGHT), K(39, HID_KEY_GUI_RIGHT),
            K(41, HID_KEY_BACKSPACE), K(42, HID_KEY_DELETE),
            K(44, HID_KEY_HOME), K(45, HID_KEY_PAGE_DOWN), K(46, HID_KEY_PAGE_UP), K(47, HID_KEY_END)
        }),
    });
    // clang-format on
}
