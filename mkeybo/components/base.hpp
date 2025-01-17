#pragma once


#include <cstdint>
#include <string>
#include <array>


namespace mkeybo {

/**
 * Base types used in all components
 */

/**
 * This enum class represents types of keycodes used in this firmware.
 * Some of that types are used directly by usb reports. Other are used internally
 */
enum class KeycodeType : uint8_t
{
    null = 0, // special sentinel keycode
    // that codes are report to usb host
    hid = 1, // typical hid keycodes
    cc = 2, // customer control keycodes like volume up, volume down
    mouse = 3, // mouse emulation keycodes
    gamepad = 4, // gamepad emulation keycodes
    // that codes are not report to usb
    macro_start = 250, // start of macro keycodes
    macro = 251, // lunch macro N keycodes
    action = 252, // internal action (like reboot) keycodes
    layer = 253, // change layer to N keycodes
    layout = 254, // change layout to N keycodes
    sentinel = 255 // special sentinel keycode
};

/**
 * Represents keycode across the system
 *
 * Compare operator are needed by some containers
 */
struct Keycode
{
    KeycodeType type{KeycodeType::null};
    uint16_t code{0};

    bool operator==(const Keycode& keycode) const = default;

    bool operator<(const Keycode& other) const
    {
        return type < other.type || (type == other.type && code < other.code);
    }

    [[nodiscard]] bool is_null() const { return type == KeycodeType::null; }
    [[nodiscard]] bool is_sentinel() const { return type == KeycodeType::sentinel; }
};

enum class SwitchEventType : uint8_t
{
    idle,
    released,
    pressed,
    tap_dance_end
};

/**
 * Switch events represents physical state of keyboard switch.
 * This is just on/off switch, so two mainly parameters is press or release. But in modern keyboard
 * we must know more. So we have counters, how long switch was pressed, how long ago switch was released.
 * Next we hace tap_dance counter, how many times are key pressed without stoping "dance".
 *
 * All that counters are counted in cycles (switches refresh state interval, typically 50ms)
 * Counters pressed and released are mutually exclusive.
 * Counter tap_dance counts the number of presses consecutively at intervals no greater than in
 * KeyboardSettings.tap_dance_max_interval_ms
 * Field hold are true if pressed counter is at least than KeyboardSettings.hold_min_interval
 */
struct SwitchEvent
{
    uint8_t pressed{0};
    uint8_t released{0};
    uint8_t tap_dance{0};
    bool hold{false};
    SwitchEventType type{SwitchEventType::idle};
};

enum class KeycodeEventType : uint8_t
{
    draft,
    finalized,
    canceled,
};

enum class KeycodeEventPriority : uint8_t
{
    normal,
    high,
};

/**
 * KeycodeEvent represent Keycode with metadata, that we need to proper map keycode from switch.
 */
struct KeycodeEvent
{
    mutable KeycodeEventType type{KeycodeEventType::draft};
    mutable KeycodeEventPriority priority{KeycodeEventPriority::normal};
    Keycode keycode{};
    uint8_t switch_no{0};

    bool operator==(const KeycodeEvent& keycode_event) const
    {
        return this->type == keycode_event.type && this->keycode == keycode_event.keycode;
    }
};

/**
 * Received from usb host keyboard led status
 */
struct LedStatus
{
    uint8_t num_lock : 1; // Bit 0: Num Lock (1 bit)
    uint8_t caps_lock : 1; // Bit 1: Caps Lock (1 bit)
    uint8_t scroll_lock : 1; // Bit 2: Scroll Lock (1 bit)
    uint8_t compose : 1; // Bit 3: Compose (1 bit)
    uint8_t kana : 1; // Bit 4: Kana (1 bit)
    uint8_t reserved : 3; // Bits 5-7: Reserved (3 bits, must be zero)
};


/**
 * Low level config. This config must be declarative as constexpr in config.hpp
 */
struct KeyboardConfig
{
    size_t switches_count{};
    std::string keyboard_name{};
    std::string manufactured_name{};
};


}
