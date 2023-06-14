#pragma once


#include "pico/stdlib.h"


auto get_ms_since_boot() { return to_ms_since_boot(get_absolute_time()); }
