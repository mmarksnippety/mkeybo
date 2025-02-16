#pragma once

#include <bitset>


namespace mkeybo::keyboard {

template <size_t switches_count>
class SwitchReader
{
public:
    virtual ~SwitchReader() = default;
    virtual void update(std::bitset<switches_count>& switches_state) = 0;
};

}
