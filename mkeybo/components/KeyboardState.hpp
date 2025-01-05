#ifndef __mkeybo_keyboard_state_hpp__
#define __mkeybo_keyboard_state_hpp__


#include <bitset>
#include <cstdint>

namespace mkeybo {

    enum class SwitchEventType {
        idle,
        pressed,
        released,
        hold,
        hold_released,
        tap_dance,
        tap_dance_released
    };

    struct SwitchEvent {
        uint8_t pressed{0};
        uint8_t released{0};
        uint8_t tap_dance_count{0};
        SwitchEventType event_type{SwitchEventType::idle};
    };

    enum class KeycodeType : uint8_t {
        KEY = 1,           // typical hid keycodes
        CC_KEY = 2,        // customer control keycodes like volume up, volume down
        MOUSE = 3,         // mouse emulation keycodes
        GAMEPAD = 4,       // gamepad emulation keycodes
        MACRO = 253,       // lunch macro N keycodes
        INTERNAL = 254,    // internal action (like reboot) keycodes
        LAYER_CHANGE = 255,// change layer to N keycodes
    };

    /**
     * Keycode, use uint16 because some types of codes (KCT_CC) are 16bits
     */
    struct Keycode {
        uint16_t code;
        KeycodeType type;
    };

    struct KeyEvent {

    };


    template <uint8_t switches_count>
    struct KeyboardState {
        std::bitset<switches_count> switches_state{};
        std::array<SwitchEvent, switches_count> switch_events{};

        KeyboardState() {
            switches_state.reset();
            std::fill(switch_events.begin(), switch_events.end(), SwitchEvent{});
        }
    };


}





#endif
