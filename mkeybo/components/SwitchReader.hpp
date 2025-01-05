#pragma once

#include <bitset>
#include <cstdint>


namespace mkeybo {
    template <uint8_t switches_count>
    class SwitchReader {
    public:
        virtual void read_state(std::bitset<switches_count>& switches_state) {
        }

        virtual ~SwitchReader() = default;
    };
}
