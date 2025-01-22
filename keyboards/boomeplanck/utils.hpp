#pragma once

#include <pico/time.h>

auto get_ms_since_boot() { return to_ms_since_boot(get_absolute_time()); }
