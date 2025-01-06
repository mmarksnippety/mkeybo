
# Make orders in code

* `KeyboardState` make it more elegant, and test it (layers!)
* `Keyboard` itis not finished, integrate it with usb

# Project at all

* Make orders in directories, move mkeybo and external to source
* Add some examples keyboard (two, free not more), 
* Make project with sample keyboard, that use mkeybo, but is not a part of it
* Check names and layouts, maybe make one header for "all you need"
* Make instruction to "make" code and load it to pico, without IDE. Remember about setup project/repository

# Logging "system"

Simple idea, needs refinement

```c++
#include <iostream>
#include <string>
#include <ctime>

#ifdef ENABLE_LOGGING
    inline std::string current_time() {
        time_t now = std::time(nullptr);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        return std::string(buf);
    }

    #define LOG_INFO(x)  do { std::cout << "[INFO]  [" << current_time() << "] " << x << std::endl; } while (0)
    #define LOG_WARN(x)  do { std::cout << "[WARN]  [" << current_time() << "] " << x << std::endl; } while (0)
    #define LOG_ERROR(x) do { std::cerr << "[ERROR] [" << current_time() << "] " << x << std::endl; } while (0)
#else
    #define LOG_INFO(x)  do { } while (0)
    #define LOG_WARN(x)  do { } while (0)
    #define LOG_ERROR(x) do { } while (0)
#endif

int main() {
    LOG_INFO("This is an info message.");
    LOG_WARN("This is a warning message.");
    LOG_ERROR("This is an error message.");
    return 0;
}
```

# Implement pico-ssd1306
https://github.com/Harbys/pico-ssd1306

# Register USB pid/vid
https://pid.codes/1209/

# Implementing snitch in test
https://github.com/snitch-org/snitch

