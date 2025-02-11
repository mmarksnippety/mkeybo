#pragma once

#include <algorithm>
#include <bitset>
#include <limits>
#include <ranges>
#include "base.hpp"
#include "hid_controller.hpp"
#include "input_device.hpp"
#include "keyboard_settings.hpp"
#include "keycode_event_buffer.hpp"
#include "mapping_rules/base_mapping_rule.hpp"
#include "switch_events.hpp"
#include "switch_reader.hpp"
#include "usb_reports.hpp"


namespace mkeybo {


template <size_t switches_count, size_t keycodes_buffer_size = 20>
class Keyboard : public InputDevice
{
protected:
    uint8_t start_report_id{0};
    std::vector<bool> active_layers_{};
    std::vector<bool> active_layers_prev_cycle_{};
    uint8_t active_layout_{};
    size_t layout_size_{};
    uint8_t active_layout_prev_cycle_{};
    std::bitset<switches_count> switch_state_{};
    std::array<SwitchEvent, switches_count> switch_events_{};
    KeycodeEventBuffer<keycodes_buffer_size> keycode_events_prev_cycle_{};
    KeycodeEventBuffer<keycodes_buffer_size> keycode_events_{};
    LedStatus led_status_{};
    SwitchReader<switches_count>* switch_reader_;
    SwitchEventsGenerator<switches_count>* switch_events_generator_;
    std::vector<mapping_rule::BaseMappingRule<switches_count, keycodes_buffer_size>*> keycodes_mapping_rules_;
    KeyboardSettings<switches_count>* settings_{};

public:
    Keyboard(SwitchReader<switches_count>* switch_reader,
             SwitchEventsGenerator<switches_count>* switch_events_generator,
             const std::vector<mapping_rule::BaseMappingRule<switches_count, keycodes_buffer_size>*>&
             keycode_mapping_rules
        ) :
        switch_reader_(switch_reader), switch_events_generator_(switch_events_generator),
        keycodes_mapping_rules_(keycode_mapping_rules)
    {
    };

    ~Keyboard() override
    {
        delete switch_reader_;
        delete switch_events_generator_;
        for (auto& keycodes_mapping_rule : keycodes_mapping_rules_)
        {
            delete keycodes_mapping_rule;
        }
        delete settings_;
    };

    /**
     * Settings
     */

    KeyboardSettings<switches_count>* get_settings() { return settings_; }

    void set_settings(KeyboardSettings<switches_count>* settings)
    {
        settings_ = settings;
        layout_size_ = settings->layouts.size();
        active_layers_.resize(settings->layers.size());
        active_layers_prev_cycle_.resize(settings->layers.size());
        reset();
        on_update_settings();
    }

    /**
     * Layers
     */

    void reset_active_layers() { std::fill(active_layers_.begin(), active_layers_.end(), false); }

    void set_active_layer(const uint8_t layer_index, const bool active = true)
    {
        if (layer_index >= active_layers_.size())
        {
            return;
        }
        active_layers_[layer_index] = active;
    }

    auto& get_active_layers() { return active_layers_; }

    bool is_layer_active(const uint8_t layer_index) { return active_layers_[layer_index] == true; }

    [[nodiscard]] bool is_layer_changed() const { return active_layers_prev_cycle_ != active_layers_; }

    /**
     * Layout
     */

    void reset_active_layout() { active_layout_ = 0; }

    void set_active_layout(const uint8_t layout_index)
    {
        if (layout_index >= layout_size_)
        {
            return;
        }
        active_layout_ = layout_index;
    }

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
     * Keycodes events
     */
    void reset_keycode_events() { keycode_events_.reset(); }

    void push_keycode_event(const KeycodeEvent& keycode_event) { keycode_events_.push(keycode_event); }

    void push_keycode_event(const Keycode& keycode, const uint8_t switch_no = std::numeric_limits<uint8_t>::max(),
                            const KeycodeEventType type = KeycodeEventType::draft,
                            KeycodeEventPriority priority = KeycodeEventPriority::normal)
    {
        keycode_events_.push(keycode, switch_no, type, priority);
    }

    auto get_filtered_keycode_events(const std::optional<KeycodeType> keycode_type = std::nullopt,
                                     const std::optional<KeycodeEventType> keycode_event_type = std::nullopt,
                                     const std::optional<KeycodeEventPriority> keycode_event_priority = std::nullopt)
    {
        return keycode_events_.get_filtered_events(keycode_type, keycode_event_type, keycode_event_priority);
    }

    bool have_keycode_events_changed(const KeycodeType& keycode_type)
    {
        return !std::ranges::equal(
            keycode_events_.get_filtered_events(keycode_type, KeycodeEventType::finalized),
            keycode_events_prev_cycle_.get_filtered_events(keycode_type, KeycodeEventType::finalized));
    }

    /**
     * Led status
     */

    auto& get_led_status() { return led_status_; }

    void set_led_status(const LedStatus& led_status) { led_status_ = led_status; }

    /**
     * Life cycle methods
     */

    void reset()
    {
        reset_active_layers();
        reset_active_layout();
        reset_switch_state();
        reset_switch_events();
        reset_keycode_events();
        // reset prev cycle states
        std::fill(active_layers_prev_cycle_.begin(), active_layers_prev_cycle_.end(), false);
        keycode_events_prev_cycle_.reset();
    }

    /**
     * @brief Executes the main keyboard task that orchestrates the key processing cycle.
     *
     * This method is responsible for managing the primary operations during each keyboard cycle. It performs the
     * following:
     * - Advances the keyboard's internal state to the next cycle.
     * - Updates the current switch state through the switch reader.
     * - Generates events for switch state transitions using the switch events generator.
     * - Performs key mapping by translating switch events into actionable key events through the key mapper.
     *
     * It is designed to be called repeatedly, typically within a loop, to handle continuous keyboard operations.
     */
    void update_state() override
    {
        reset_state_cycle();
        update_switch_state();
        update_switch_events();
        on_update_switch_events();
        generate_keycodes();
        on_generate_keycodes();
    }

    void reset_state_cycle()
    {
        active_layers_prev_cycle_ = active_layers_;
        active_layout_prev_cycle_ = active_layout_;
        copy_keycode_events_to_prev_cycle();
    }

    void copy_keycode_events_to_prev_cycle()
    {
        keycode_events_prev_cycle_.reset();
        std::ranges::for_each(keycode_events_.get_filtered_events(std::nullopt, KeycodeEventType::finalized),
                              [this](KeycodeEvent& keycode_event)
                              {
                                  keycode_events_prev_cycle_.push(keycode_event);
                              });
    }

    void update_switch_state()
    {
        this->switch_reader_->update(switch_state_);
    }

    void update_switch_events()
    {
        this->switch_events_generator_->update(settings_, switch_state_, switch_events_);
    }

    void generate_keycodes()
    {
        bool mapped = false;
        reset_active_layers();
        while (!mapped)
        {
            reset_keycode_events();
            mapped = true;
            for (auto rule : keycodes_mapping_rules_)
            {
                if (rule->map(this))
                {
                    mapped = false;
                    break;
                }
            }
        }
    }

    /**
     *Events callbacks
     */

    /**
     * Called when new settings are apply to keyboard
     */
    virtual void on_update_settings()
    {
    }

    /**
     * Called when switch events are update.
     */
    virtual void on_update_switch_events()
    {
    }

    /**
     * Called when new keycodes are generate
     */
    virtual void on_generate_keycodes()
    {
    }

    virtual void on_led_status_update()
    {
    }

    void on_usb_report_receive(const uint8_t instance, const uint8_t report_id, const hid_report_type_t report_type,
                               const uint8_t* buffer, const uint16_t bufsize) override
    {
        if (report_id == start_report_id)
        {
            const LedStatus led_status{*buffer};
            this->set_led_status(led_status);
            on_led_status_update();
        }
    }

    /**
     * Usb reports descriptions
     */

    void setup_usb_reports(HidController* hid_controller) override
    {
        this->start_report_id = start_report_id;
        start_report_id = hid_controller->add_usb_report(new UsbKeyboardReport());
        hid_controller->add_usb_report(new UsbCcReport());
    }

    /**
     * Generate usb reports
     */
    void update_usb_reports(HidController* hid_controller) override
    {
        conditionally_generate_usb_keyboard_report(
            reinterpret_cast<UsbKeyboardReport*>(hid_controller->get_usb_report(start_report_id)));
        conditionally_generate_usb_cc_report(
            reinterpret_cast<UsbCcReport*>(hid_controller->get_usb_report(start_report_id + 1)));
    }

    void conditionally_generate_usb_keyboard_report(UsbKeyboardReport* report)
    {
        if (have_keycode_events_changed(KeycodeType::hid))
        {
            generate_usb_keyboard_report(report);
        }
    }

    void generate_usb_keyboard_report(UsbKeyboardReport* report)
    {
        report->status = UsbReportStatus::draft;
        report->modifiers = 0;
        report->keycodes.fill(0);
        generate_usb_keyboard_modifiers(get_keyboard_modifiers_keycodes(KeycodeEventPriority::high), report);
        generate_usb_keyboard_modifiers(get_keyboard_modifiers_keycodes(KeycodeEventPriority::normal), report);
        auto index = generate_usb_keyboard_report_regular(
            get_keyboard_regular_keycodes(KeycodeEventPriority::high), report);
        generate_usb_keyboard_report_regular(
            get_keyboard_regular_keycodes(KeycodeEventPriority::normal), report, index);
        report->status = UsbReportStatus::ready;
    }

    auto get_keyboard_modifiers_keycodes(KeycodeEventPriority priority)
    {
        return this->get_filtered_keycode_events(KeycodeType::hid, KeycodeEventType::finalized, priority) |
            std::views::filter([](const auto& keycode_event)
            {
                return keycode_event.keycode.code >= HID_KEY_CONTROL_LEFT;
            });
    }

    auto get_keyboard_regular_keycodes(KeycodeEventPriority priority)
    {
        return this->get_filtered_keycode_events(KeycodeType::hid, KeycodeEventType::finalized, priority) |
            std::views::filter([](const auto& keycode_event)
            {
                return static_cast<uint8_t>(keycode_event.keycode.code) < HID_KEY_CONTROL_LEFT;
            });
    }

    /**
     * Modifier are coded as bits.
     * Luckily all modifiers keycodes are in the same order as the bits in the report
     *
     * Modifier name          | code   | bit index
     * -----------------------+--------+----------
     * HID_KEY_CONTROL_LEFT   |  0xE0  | 0
     * HID_KEY_SHIFT_LEFT     |  0xE1  | 1
     * HID_KEY_ALT_LEFT       |  0xE2  | 2
     * HID_KEY_GUI_LEFT       |  0xE3  | 3
     * HID_KEY_CONTROL_RIGHT  |  0xE4  | 4
     * HID_KEY_SHIFT_RIGHT    |  0xE5  | 5
     * HID_KEY_ALT_RIGHT      |  0xE6  | 6
     * HID_KEY_GUI_RIGHT      |  0xE7  | 7
     */
    template <class KeycodeEvnetView>
    void generate_usb_keyboard_modifiers(KeycodeEvnetView&& modifiers_keycodes, UsbKeyboardReport* report)
    {
        for (auto& keycode_event : modifiers_keycodes)
        {
            // This special case taken from QMK. This coed is mixed modifiers bits and standard keycode
            // first 8bit is a code, higher 8 bits is a modifier bits (no key code of ex shift key)
            if (keycode_event.keycode.code >= std::numeric_limits<uint8_t>::max())
            {
                const auto modifier_bits = static_cast<uint8_t>(keycode_event.keycode.code >> 8);
                report->modifiers |= modifier_bits;
            }
            else
            {
                auto bit_index = keycode_event.keycode.code - HID_KEY_CONTROL_LEFT;
                report->modifiers |= 1 << bit_index;
            }
        }
    }

    template <class KeycodeEventView>
    uint8_t generate_usb_keyboard_report_regular(KeycodeEventView&& regular_keycodes, UsbKeyboardReport* report,
                                                 const uint8_t start_keycode_index = 0)
    {
        uint8_t keycode_index = start_keycode_index;
        for (auto& keycode_event : regular_keycodes)
        {
            // more than 6 keys, go to phantom mode
            if (keycode_index > report->keycodes.size())
            {
                report->keycodes.fill(1);
                break;
            }
            report->keycodes[keycode_index] = static_cast<uint8_t>(keycode_event.keycode.code);
            keycode_index++;
        }
        return keycode_index;
    }

    void conditionally_generate_usb_cc_report(UsbCcReport* report)
    {
        if (have_keycode_events_changed(KeycodeType::cc))
        {
            generate_usb_cc_report(report);
        }
    }

    void generate_usb_cc_report(UsbCcReport* report)
    {
        report->status = UsbReportStatus::draft;
        report->keycode = 0;
        generate_usb_cc_report_keycode(get_cc_keycodes(KeycodeEventPriority::high), report);
        if (report->keycode == 0)
        {
            generate_usb_cc_report_keycode(get_cc_keycodes(KeycodeEventPriority::normal), report);
        }
        report->status = UsbReportStatus::ready;
    }

    auto get_cc_keycodes(KeycodeEventPriority priority)
    {
        return get_filtered_keycode_events(KeycodeType::cc, KeycodeEventType::finalized, priority);
    }

    template <class KeycodeEvnetView>
    void generate_usb_cc_report_keycode(KeycodeEvnetView&& keycodes_views, UsbCcReport* report)
    {
        auto keycode_event_it = keycodes_views.begin();
        if (keycode_event_it != keycodes_views.end())
        {
            report->keycode = (*keycode_event_it).keycode.code;
        }
    }

    /**
     * Update actions
     */
    void update_actions(HidController* hid_controller) override
    {
        for (const auto& keycode_event : get_filtered_keycode_events(
                 KeycodeType::action, KeycodeEventType::finalized, KeycodeEventPriority::high))
        {
            hid_controller->push_action(keycode_event.keycode.code);
        }
        for (const auto& keycode_event : get_filtered_keycode_events(
                 KeycodeType::action, KeycodeEventType::finalized, KeycodeEventPriority::normal))
        {
            hid_controller->push_action(keycode_event.keycode.code);
        }
    }

};

} // namespace mkeybo
