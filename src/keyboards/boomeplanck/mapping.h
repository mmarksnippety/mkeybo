#pragma once

#include "components/KeyboardConfig.h"


MapperConfig *mapper_config_factory() {
    // clang-format off
    return new MapperConfig({
        new LayerConfig(0, true, true, {
            R2(40, 43, L(3)),
            // R0, 0 ~ 11
            R(0, K(HID_KEY_TAB)), R(1, K(HID_KEY_Q)), R(2, K(HID_KEY_W)), R(3, K(HID_KEY_E)),
            R(4, K(HID_KEY_R)), R(5, K(HID_KEY_T)), R(6, K(HID_KEY_Y)), R(7, K(HID_KEY_U)),
            R(8, K(HID_KEY_I)), R(9, K(HID_KEY_O)), R(10, K(HID_KEY_P)), R(11, K(HID_KEY_BACKSPACE)),
            // R1, 12 ~ 23
            R(12, K(HID_KEY_ESCAPE)), R(13, K(HID_KEY_A)), R(14, K(HID_KEY_S)), R(15, K(HID_KEY_D)),
            R(16, K(HID_KEY_F)), R(17, K(HID_KEY_G)), R(18, K(HID_KEY_H)), R(19, K(HID_KEY_J)),
            R(20, K(HID_KEY_K)), R(21, K(HID_KEY_L)), R(22, K(HID_KEY_SEMICOLON)), R(23, K(HID_KEY_APOSTROPHE)),
            // R2, 24 ~ 35
            R(24, K(HID_KEY_SHIFT_LEFT)), R(25, K(HID_KEY_Z)), R(26, K(HID_KEY_X)), R(27, K(HID_KEY_C)),
            R(28, K(HID_KEY_V)), R(29, K(HID_KEY_B)), R(30, K(HID_KEY_N)), R(31, K(HID_KEY_M)),
            R(32, K(HID_KEY_COMMA)), R(33, K(HID_KEY_PERIOD)), R(34, K(HID_KEY_SLASH)), R(35, K(HID_KEY_ENTER)),
            // R3, 36 ~ 47
            R(36, K(HID_KEY_CONTROL_LEFT)), R(37, L(4)), R(38, K(HID_KEY_GUI_LEFT)), R(39, K(HID_KEY_ALT_LEFT)),
            R(40, L(1)), R(41, K(HID_KEY_SPACE)), R(42, K(HID_KEY_SPACE)), R(43, L(2)),
            R(44, K(HID_KEY_ARROW_LEFT)), R(45, K(HID_KEY_ARROW_DOWN)), R(46, K(HID_KEY_ARROW_UP)), R(47, K(HID_KEY_ARROW_RIGHT))
        }),
        new LayerConfig(1, false, true, {
            // 0 ~ 11
            R(0, K(M_LSHIFT(HID_KEY_GRAVE))), R(1, K(M_LSHIFT(HID_KEY_1))), R(2, K(M_LSHIFT(HID_KEY_2))), R(3, K(M_LSHIFT(HID_KEY_3))),
            R(4, K(M_LSHIFT(HID_KEY_4))), R(5, K(M_LSHIFT(HID_KEY_5))), R(6, K(M_LSHIFT(HID_KEY_6))), R(7, K(M_LSHIFT(HID_KEY_7))),
            R(8, K(M_LSHIFT(HID_KEY_8))), R(9, K(M_LSHIFT(HID_KEY_9))), R(10, K(M_LSHIFT(HID_KEY_0))),
            // 12 - 23
            R(12, K(HID_KEY_DELETE)), R(13, K(HID_KEY_F1)), R(14, K(HID_KEY_F2)), R(15, K(HID_KEY_F3)),
            R(16, K(HID_KEY_F4)), R(17, K(HID_KEY_F5)), R(18, K(HID_KEY_F6)), R(19, K(M_LSHIFT(HID_KEY_MINUS))),
            R(20, K(M_LSHIFT(HID_KEY_EQUAL))), R(21, K(M_LSHIFT(HID_KEY_BRACKET_LEFT))), R(22, K(M_LSHIFT(HID_KEY_BRACKET_RIGHT))), R(23, K(M_LSHIFT(HID_KEY_BACKSLASH))),
            // 24 ~ 35
            R(25, K(HID_KEY_F7)), R(26, K(HID_KEY_F8)), R(27, K(HID_KEY_F9)),
            R(28, K(HID_KEY_F10)), R(29, K(HID_KEY_F11)), R(30, K(HID_KEY_F12)), R(31, K(M_LSHIFT(HID_KEY_BACKSLASH))), R(32, K(HID_KEY_SLASH))
            // 36 ~ 47
        }),
        new LayerConfig(2, false, true, {
            // 0 ~ 11
            R(0, K(HID_KEY_GRAVE)), R(1, K(HID_KEY_1)), R(2, K(HID_KEY_2)), R(3, K(HID_KEY_3)),
            R(4, K(HID_KEY_4)), R(5, K(HID_KEY_5)), R(6, K(HID_KEY_6)), R(7, K(HID_KEY_7)),
            R(8, K(HID_KEY_8)), R(9, K(HID_KEY_9)), R(10, K(HID_KEY_0)),
            // 12 - 23
            R(12, K(HID_KEY_DELETE)), R(13, K(HID_KEY_F1)), R(14, K(HID_KEY_F2)), R(15, K(HID_KEY_F3)),
            R(16, K(HID_KEY_F4)), R(17, K(HID_KEY_F5)), R(18, K(HID_KEY_F6)), R(19, K(HID_KEY_MINUS)),
            R(20, K(HID_KEY_EQUAL)), R(21, K(HID_KEY_BRACKET_LEFT)), R(22, K(HID_KEY_BRACKET_RIGHT)), R(23, K(HID_KEY_BACKSLASH)),
            // 24 ~ 35
            R(25, K(HID_KEY_F7)), R(26, K(HID_KEY_F8)), R(27, K(HID_KEY_F9)),
            R(28, K(HID_KEY_F10)), R(29, K(HID_KEY_F11)), R(30, K(HID_KEY_F12)), R(31, K(HID_KEY_BACKSLASH)), R(32, K(HID_KEY_SLASH))
            // 36 ~ 47
        }),
        new LayerConfig(3, false, true, {
            R(19, KCC(HID_USAGE_CONSUMER_BRIGHTNESS_DECREMENT)), R(20, KCC(HID_USAGE_CONSUMER_BRIGHTNESS_INCREMENT)),
            // 24 ~ 35
            R(31, KCC(HID_USAGE_CONSUMER_MUTE)),
            // 36 ~ 47
            R(44, KCC(HID_USAGE_CONSUMER_AC_BACK)), R(45, KCC(HID_USAGE_CONSUMER_VOLUME_DECREMENT)),
            R(46, KCC(HID_USAGE_CONSUMER_VOLUME_INCREMENT)), R(47, KCC(HID_USAGE_CONSUMER_AC_FORWARD))
        }),
        new LayerConfig(4, false, true, {
            // 36 ~ 47
            R(36, K(HID_KEY_CONTROL_RIGHT)), R(38, K(HID_KEY_APPLICATION)), R(39, K(HID_KEY_ALT_RIGHT)),
            R(41, K(HID_KEY_BACKSPACE)), R(42, K(HID_KEY_DELETE)),
            R(44, K(HID_KEY_HOME)), R(45, K(HID_KEY_PAGE_DOWN)), R(46, K(HID_KEY_PAGE_UP)), R(47, K(HID_KEY_END))
        }),
    });
    // clang-format on
}
