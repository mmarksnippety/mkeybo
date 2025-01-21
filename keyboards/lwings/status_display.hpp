#pragma once

#include "ssd1306.h"
#include "keyboard.hpp"
#include "config.hpp"
#include "status_display_config.hpp"
#include "mkeybo/components/base.hpp"


class StatusDisplay
{
    pico_ssd1306::SSD1306 *hardware;

public:
    explicit StatusDisplay(const DisplayConfig& config);
    ~StatusDisplay();
    void display_logo(uint8_t *logo) const;
    void show_keyboard_status(mkeybo::Keyboard<keyboard_config.switches_count>* keyboard) const;
};
