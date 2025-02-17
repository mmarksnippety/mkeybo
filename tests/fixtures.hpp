#pragma once

#include "../mkeybo/components/keyboard/keyboard_settings.hpp"
#include "mkeybo/factories.hpp"


template <size_t switches_count>
mkeybo::KeyboardSettings<switches_count>* k_settings_fixture()
{
    auto layout = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name = "qwerty", .keycodes = {H_K(1), H_K(2), H_K(3), LAYER_K(1)}};
    auto layer_down = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name = "down", .keycodes = {H_K(10), H_K(20), H_K(30), N_K()}};
    auto layer_up = new mkeybo::KeyboardSettingsLayer<switches_count>{
        .name = "up", .keycodes = {H_K(100), H_K(200), H_K(300), N_K()}};
    return new mkeybo::KeyboardSettings<switches_count>{
        "default", {layout}, {layer_down, layer_up}, {}, 50, 50,
        100, 150};
}
