#pragma once

#include "hardware/i2c.h"

struct DisplayConfig
{
    i2c_inst* i2c{}; //i2c1;
    uint sda_pin{}; // = 2;
    uint scl_pin{}; //= 3;
    uint16_t address{}; // = 0x3c;
    uint8_t width{}; // = 128;
    uint8_t height{}; // = 64;
};
