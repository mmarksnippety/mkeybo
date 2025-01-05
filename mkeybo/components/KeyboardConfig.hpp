#ifndef __mkeybo_keyboard_config_hpp__
#define __mkeybo_keyboard_config_hpp__

#include <cstdint>

namespace mkeybo
{


    struct KeyboardConfig
    {
        uint8_t switches_count;
        uint16_t switches_refresh_interval;
    };

}

#endif //__mkeybo_keyboard_config_hpp__
