#pragma once

#include "ssd1306.h"
#include "keyboard.hpp"
#include "status_display_config.hpp"
#include "status_display.hpp"
#include <textRenderer/12x16_font.h>
#include <textRenderer/TextRenderer.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "mkeybo/components/input_device.hpp"
#include "logo.hpp"


enum class StatusDisplayState
{
    boot,
    idle,
};


template <size_t switches_count, size_t keycodes_buffer_size = 20>
class StatusDisplay final : public mkeybo::Device
{
    pico_ssd1306::SSD1306* hardware;
    Keyboard<switches_count, keycodes_buffer_size>* keyboard;
    StatusDisplayState state = StatusDisplayState::boot;

public:
    explicit StatusDisplay(const DisplayConfig& config, Keyboard<switches_count, keycodes_buffer_size>* keyboard):
        mkeybo::Device(), keyboard(keyboard)
    {
        i2c_init(config.i2c, 100 * 1000);
        gpio_set_function(config.sda_pin, GPIO_FUNC_I2C);
        gpio_set_function(config.scl_pin, GPIO_FUNC_I2C);
        gpio_pull_up(config.sda_pin);
        gpio_pull_up(config.scl_pin);
        hardware = new pico_ssd1306::SSD1306(config.i2c, config.address, pico_ssd1306::Size::W128xH64);
        hardware->setOrientation(false);
    }

    ~StatusDisplay() override
    {
        delete hardware;
    }

    void main_task() override
    {
        if (state == StatusDisplayState::boot)
        {
            display_logo();
            state = StatusDisplayState::idle;
        }
    }

    void update_state() override
    {
        if (state == StatusDisplayState::idle)
        {
            show_keyboard_status();
        }
    }

    void display_logo() const
    {
        hardware->addBitmapImage(0, 0, 128, 64, mkeybo_logo);
        hardware->sendBuffer();
    }

    void show_keyboard_status() const
    {
        constexpr uint8_t line_height = 14;
        uint8_t line_index = 64 - line_height;
        const auto font = font_12x16;
        hardware->clear();
        drawText(hardware, font, "layers", 0, 0);
        for (auto layer_index = 0; const auto& layer_settings : keyboard->get_layers())
        {
            if (keyboard->is_layer_active(layer_index))
            {
                drawText(hardware, font, layer_settings->name.c_str(), 0, line_index);
                line_index -= line_height;
            }
            layer_index++;
        }
        hardware->sendBuffer();
    }

};
