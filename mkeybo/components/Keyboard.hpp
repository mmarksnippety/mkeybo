#pragma once

#include "KeyMapper.hpp"
#include "SwitchEvents.hpp"
#include "SwitchReader.hpp"
#include "UsbReporter.hpp"
#include "base.hpp"
#include "pico/unique_id.h"

namespace mkeybo {


template <size_t switch_count>
class Keyboard
{
protected:
    KeyboardState<switch_count>* state_;
    SwitchReader<switch_count>* switch_reader_;
    SwitchEventsGenerator<switch_count>* switch_events_generator_;
    KeyMapper<switch_count>* key_mapper_;
    UsbReporterManager<switch_count>* usb_reporter_manager_;
    KeyboardSettings<switch_count>* settings_{};
    std::string unique_id_{};

public:
    Keyboard(KeyboardState<switch_count>* state, SwitchReader<switch_count>* switch_reader,
             SwitchEventsGenerator<switch_count>* switch_events_generator, KeyMapper<switch_count>* key_mapper,
             UsbReporterManager<switch_count>* usb_reporter_manager) :
        state_(state), switch_reader_(switch_reader), switch_events_generator_(switch_events_generator),
        key_mapper_(key_mapper), usb_reporter_manager_(usb_reporter_manager)
    {
        usb_reporter_manager_->generate_empty_records(state_);
    };

    virtual ~Keyboard()
    {
        delete state_;
        delete switch_reader_;
        delete switch_events_generator_;
        delete key_mapper_;
        delete settings_;
    };

    KeyboardState<switch_count>* get_state() { return state_; }
    SwitchReader<switch_count>* get_switch_reader() { return switch_reader_; }
    SwitchEventsGenerator<switch_count>* get_switch_events_generator() { return switch_events_generator_; }
    KeyMapper<switch_count>* get_key_mapper() { return key_mapper_; }
    UsbReporterManager<switch_count>* get_usb_reporter_manager() { return usb_reporter_manager_; }
    KeyboardSettings<switch_count>* get_settings() { return settings_; }

    std::string_view get_unique_id()
    {
        if (unique_id_.empty())
        {
            constexpr int pico_id_len = 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1;
            char pico_id[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];
            pico_get_unique_board_id_string(pico_id, pico_id_len);
            unique_id_ = pico_id;
        }
        return unique_id_;
    }

    void update_settings(KeyboardSettings<switch_count>* settings)
    {
        settings_ = settings;
        state_->setup_for_settings(settings_);
        on_update_settings();
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
     * - Generates USB HID reports based on the current keycode events state.
     *
     * It is designed to be called repeatedly, typically within a loop, to handle continuous keyboard operations.
     */
    virtual void main_task()
    {
        this->state_->next_cycle();
        this->switch_reader_->update(this->state_->switch_state);
        this->switch_events_generator_->update(this->settings_, this->state_);
        on_switch_events_generate();
        this->key_mapper_->map(this->settings_, this->state_);
        on_key_mapping();
        this->usb_reporter_manager_->generate_reports(this->state_);
        on_usb_report_ready();
    }

    virtual void hid_task()
    {
        if (usb_reporter_manager_->is_any_report_ready(state_))
        {
            if (tud_suspended())
            {
                tud_remote_wakeup();
            }
            else
            {
                // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
                send_usb_report();
            }
        }
    }

    /**
     * This method send first ready usb report to host
     */
    void send_usb_report() { usb_reporter_manager_->send_reports(state_); }

    // some event from livecycle
    virtual void on_update_settings() {}
    virtual void on_switch_events_generate() {}
    virtual void on_key_mapping() {}
    virtual void on_led_status_update() {}
    virtual void on_usb_report_ready() {}
    // that callbacks are called by TinyUSB, see mkeybo/defaults/usb_callbacks.cpp
    virtual void on_usb_mount() {}
    virtual void on_usb_umount() {}
    virtual void on_usb_suspend() {}
    virtual void on_usb_resume() {}
    virtual void on_usb_report_receive(uint8_t const* buffer, uint16_t buffer_length)
    {
        LedStatus led_status{*buffer};
        state_->set_led_status(led_status);
        on_led_status_update();
    }
};

} // namespace mkeybo
