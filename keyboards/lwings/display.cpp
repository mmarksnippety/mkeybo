#include "display.hpp"
#include <textRenderer/12x16_font.h>
#include <textRenderer/TextRenderer.h>
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "keyboard.hpp"
#include "mkeybo/components/base.hpp"


Display::Display(const DisplayConfig& config)
{
    i2c_init(config.i2c, 100 * 1000);
    gpio_set_function(config.sda_pin, GPIO_FUNC_I2C);
    gpio_set_function(config.scl_pin, GPIO_FUNC_I2C);
    gpio_pull_up(config.sda_pin);
    gpio_pull_up(config.scl_pin);
    hardware = new pico_ssd1306::SSD1306(
        config.i2c, config.address, pico_ssd1306::Size::W128xH64);
    hardware->setOrientation(false);
}

Display::~Display()
{
    delete hardware;
}

void Display::display_logo(uint8_t *logo) const
{
    hardware->addBitmapImage(0, 0, 128, 64, logo);
    hardware->sendBuffer();
}

void Display::show_keyboard_status(mkeybo::Keyboard<keyboard_config.switches_count>* keyboard) const
{
    constexpr uint8_t line_height = 14;
    uint8_t line_index = 64 - line_height;
    const auto font = font_12x16;
    hardware->clear();
    drawText(hardware, font, "layers", 0, 0);
    for (auto layer_index = 0; const auto& layer_settings : keyboard->get_settings()->layers)
    {
        if (keyboard->get_state()->is_layer_active(layer_index))
        {
            std::cout << "layer " << layer_settings->name << " is active" << std::endl;
            drawText(hardware, font, layer_settings->name.c_str(), 0, line_index);
            line_index -= line_height;
        }
        layer_index++;
    }
    hardware->sendBuffer();
}
