#pragma once

#include <algorithm>
#include <limits>
#include <ranges>
#include <bitset>
#include "KeyboardSettings.hpp"
#include "base.hpp"


namespace mkeybo {

template <size_t keycodes_buffer_size>
class KeycodeEventBuffer
{
    std::array<KeycodeEvent, keycodes_buffer_size> buffer_;
    uint8_t index_ = 0;

public:
    void reset()
    {
        index_ = 0;
        buffer_.fill(KeycodeEvent{});
    }

    void push(const KeycodeEvent& keycode_event)
    {
        buffer_[index_] = keycode_event;
        index_ = (index_ + 1) % keycodes_buffer_size;
    }

    void push(const Keycode& keycode, const uint8_t switch_no = std::numeric_limits<uint8_t>::max(),
              const KeycodeEventType type = KeycodeEventType::draft)
    {
        push(KeycodeEvent{type, keycode, switch_no});
    }

    auto& get_all_events() { return buffer_; }

    auto get_finalized_events()
    {
        return buffer_ |
            std::views::filter(
                   [](const auto& keycode_event)
                   {
                       return keycode_event.type == KeycodeEventType::finalized && !keycode_event.keycode.is_null() &&
                           !keycode_event.keycode.is_sentinel();
                   });
    }

    auto get_finalized_events(const KeycodeType& keycode_type)
    {
        return buffer_ |
            std::views::filter(
                   [keycode_type](const auto& keycode_event)
                   {
                       return keycode_event.keycode.type == keycode_type &&
                           keycode_event.type == KeycodeEventType::finalized;
                   });
    }
};

template <size_t switches_count, size_t keycodes_buffer_size = 20>
class KeyboardState
{
protected:
    std::vector<bool> active_layers_{};
    std::vector<bool> active_layers_prev_cycle_{};
    uint8_t active_layout_{};
    uint8_t active_layout_prev_cycle_{};
    KeycodeEventBuffer<keycodes_buffer_size> keycode_events_prev_cycle_;
    LedStatus led_status_{};

public:
    std::bitset<switches_count> switch_state;
    std::array<SwitchEvent, switches_count> switch_events;
    KeycodeEventBuffer<keycodes_buffer_size> keycode_events;
    std::map<KeycodeType, UsbReport*> usb_reports;

    KeyboardState() { reset(); }
    ~KeyboardState()
    {
        for (const auto& report : std::views::values(usb_reports))
        {
            delete report;
        }
        usb_reports.clear();
    }

    void setup_for_settings(KeyboardSettings<switches_count>* settings)
    {
        active_layers_.resize(settings->layers.size());
        active_layers_prev_cycle_.resize(settings->layers.size());
        reset();
    }

    void reset()
    {
        switch_state.reset();
        std::fill(active_layers_.begin(), active_layers_.end(), false);
        std::fill(active_layers_prev_cycle_.begin(), active_layers_prev_cycle_.end(), false);
        switch_events.fill(SwitchEvent{});
        keycode_events.reset();
        keycode_events_prev_cycle_.reset();
    }

    void next_cycle()
    {
        active_layers_prev_cycle_ = active_layers_;
        std::fill(active_layers_.begin(), active_layers_.end(), false);
        active_layout_prev_cycle_ = active_layout_;
        // copy keycodes events to prev cycle
        keycode_events_prev_cycle_.reset();
        for (auto& keycode_event : keycode_events.get_finalized_events())
        {
            keycode_events_prev_cycle_.push(keycode_event);
        }
    }

    void set_active_layer(const uint8_t layer_index, const bool active = true) { active_layers_[layer_index] = active; }
    bool is_layer_active(const uint8_t layer_index) { return active_layers_[layer_index]; }
    [[nodiscard]] bool is_layer_changed() const { return active_layers_prev_cycle_ != active_layers_; }

    void set_active_layout(const uint8_t layout_index) { active_layout_ = layout_index; }
    auto get_active_layout() { return active_layout_; }
    [[nodiscard]] bool is_layout_changed() const { return active_layout_prev_cycle_ != active_layout_; }

    // keycodes events shortcuts
    void push_keycode_event(const KeycodeEvent& keycode_event) { keycode_events.push(keycode_event); }
    void push_keycode_event(const Keycode& keycode, const uint8_t switch_no = std::numeric_limits<uint8_t>::max(),
                            const KeycodeEventType type = KeycodeEventType::draft)
    {
        keycode_events.push(keycode, switch_no, type);
    }

    auto get_all_keycode_events() { return keycode_events.get_all_events(); }

    auto get_keycode_events(KeycodeType& keycode_type) { return keycode_events.get_finalized_events(keycode_type); }

    auto get_keycode_events() { return keycode_events.get_finalized_events(); }

    [[nodiscard]] bool have_keycode_events_changed(const KeycodeType& keycode_type)
    {
        return !std::ranges::equal(keycode_events.get_finalized_events(keycode_type),
                                   keycode_events_prev_cycle_.get_finalized_events(keycode_type));
    }

    [[nodiscard]] auto& get_reports() { return usb_reports; }

    [[nodiscard]] auto& get_led_status() { return led_status_; }

    void set_led_status(const LedStatus& led_status) { led_status_ = led_status; }
};

}
