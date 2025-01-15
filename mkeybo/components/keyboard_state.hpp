#pragma once

#include <algorithm>
#include <limits>
#include <ranges>
#include <bitset>
#include "keyboard_settings.hpp"
#include "keycode_event_buffer.hpp"
#include "base.hpp"


namespace mkeybo {

template <size_t switches_count, size_t keycodes_buffer_size = 20>
class KeyboardState
{
protected:
    std::vector<bool> active_layers_{};
    std::vector<bool> active_layers_prev_cycle_{};
    uint8_t active_layout_{};
    uint8_t active_layout_prev_cycle_{};
    std::bitset<switches_count> switch_state_{};
    std::array<SwitchEvent, switches_count> switch_events_;
    KeycodeEventBuffer<keycodes_buffer_size> keycode_events_draft_;
    KeycodeEventBuffer<keycodes_buffer_size> keycode_events_prev_cycle_;
    KeycodeEventBuffer<keycodes_buffer_size> keycode_events_;
    LedStatus led_status_{};

public:
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

    /**
     * Layers
     */

    void reset_active_layers() { std::fill(active_layers_.begin(), active_layers_.end(), false); }

    void set_active_layer(const uint8_t layer_index, const bool active = true) { active_layers_[layer_index] = active; }

    auto& get_active_layers() { return active_layers_; }

    bool is_layer_active(const uint8_t layer_index) { return active_layers_[layer_index]; }

    [[nodiscard]] bool is_layer_changed() const { return active_layers_prev_cycle_ != active_layers_; }

    /**
     * Layout
     */

    void reset_active_layout() { active_layout_ = 0; }

    void set_active_layout(const uint8_t layout_index) { active_layout_ = layout_index; }

    auto get_active_layout() { return active_layout_; }

    [[nodiscard]] bool is_layout_changed() const { return active_layout_prev_cycle_ != active_layout_; }

    /**
     * Switch state
     */

    void reset_switch_state() { switch_state_.reset(); }

    auto& get_switch_state() { return switch_state_; }


    /**
     * Switch events
     */

    void reset_switch_events() { std::fill(switch_events_.begin(), switch_events_.end(), SwitchEvent{}); }

    auto& get_switch_events() { return switch_events_; }

    /**
     * Keycodes events drafts
     */

    void reset_keycode_events_draft() { keycode_events_draft_.reset(); }

    auto& get_keycode_events_draft() { return keycode_events_draft_; }

    void push_keycode_event_draft(const KeycodeEvent& keycode_event) { keycode_events_draft_.push(keycode_event); }

    void push_keycode_event_draft(const Keycode& keycode, const uint8_t switch_no = std::numeric_limits<uint8_t>::max(),
                                  const KeycodeEventType type = KeycodeEventType::draft)
    {
        keycode_events_draft_.push(keycode, switch_no, type);
    }

    auto& get_all_keycode_events_draft() { return keycode_events_draft_.get_all_events(); }

    auto get_filtered_keycode_events_draft() { return keycode_events_draft_.get_filtered_events(); }

    auto get_filtered_keycode_events_draft(const KeycodeType keycode_type)
    {
        return keycode_events_draft_.get_filtered_events(keycode_type);
    }

    /**
     * Keycodes events
     */
    void reset_keycode_events() { keycode_events_.reset(); }

    void push_keycode_event(const KeycodeEvent& keycode_event) { keycode_events_.push(keycode_event); }

    void push_keycode_event(const Keycode& keycode, const uint8_t switch_no = std::numeric_limits<uint8_t>::max(),
                            const KeycodeEventType type = KeycodeEventType::draft)
    {
        keycode_events_.push(keycode, switch_no, type);
    }

    auto& get_all_keycode_events() { return keycode_events_.get_all_events(); }

    auto get_filtered_keycode_events(const KeycodeType keycode_type)
    {
        return keycode_events_.get_filtered_events(keycode_type);
    }

    auto get_filtered_keycode_events() { return keycode_events_.get_filtered_events(); }

    bool have_keycode_events_changed(const KeycodeType& keycode_type)
    {
        return !std::ranges::equal(keycode_events_.get_filtered_events(keycode_type),
                                   keycode_events_prev_cycle_.get_filtered_events(keycode_type));
    }


    /**
     * Usb reports
     */

    auto& get_usb_reports() { return usb_reports; }

    /**
     * Led status
     */

    auto& get_led_status() { return led_status_; }

    void set_led_status(const LedStatus& led_status) { led_status_ = led_status; }

    /**
     * Life cycle methods
     */

    void setup_for_settings(KeyboardSettings<switches_count>* settings)
    {
        active_layers_.resize(settings->layers.size());
        active_layers_prev_cycle_.resize(settings->layers.size());
        reset();
    }

    void reset()
    {
        reset_active_layers();
        reset_active_layout();
        reset_switch_state();
        reset_switch_events();
        reset_keycode_events_draft();
        reset_keycode_events();
        // reset prev cycle states
        std::fill(active_layers_prev_cycle_.begin(), active_layers_prev_cycle_.end(), false);
        keycode_events_prev_cycle_.reset();
    }

    void next_cycle()
    {
        active_layers_prev_cycle_ = active_layers_;
        active_layout_prev_cycle_ = active_layout_;
        // copy keycodes events to prev cycle
        keycode_events_prev_cycle_.reset();
        for (auto& keycode_event : keycode_events_.get_filtered_events())
        {
            keycode_events_prev_cycle_.push(keycode_event);
        }
    }
};
}
