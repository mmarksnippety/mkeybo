#pragma once

#include "pico/stdlib.h"
#include "tusb.h"
#include <algorithm>
#include <string>
#include <vector>

#include "components/KeyboardConfig.h"
#include "components/KeyboardState.h"
#include "keyboards/boomeplanck/config.h"


using namespace std;


struct HIDKeyReport {
    uint8_t modifier;
    uint8_t keycode[6];
};


template <uint8_t switches_count>
class HIDKeyReporter {
protected:
    KeyboardState<switches_count> *keyboard_state_;
    CircularBuffer<HIDKeyReport, 6> report_buffer_{};
    critical_section_t send_report_lock_{};

public:
    explicit HIDKeyReporter(KeyboardState<switches_count> *keyboard_state)
        : keyboard_state_(keyboard_state) {}

    void init() { critical_section_init(&send_report_lock_); }

    bool is_report_to_send() { return !report_buffer_.empty(); }

    bool blocking_dequeue_report(HIDKeyReport &report) {
        critical_section_enter_blocking(&send_report_lock_);
        auto report_to_send = report_buffer_.remove_element(report);
        critical_section_exit(&send_report_lock_);
        return report_to_send;
    }

    bool blocking_enqueue_report() {
        if (is_report_to_generate()) {
            critical_section_enter_blocking(&send_report_lock_);
            auto report = generate_report();
            report_buffer_.add_element(report);
            critical_section_exit(&send_report_lock_);
            return true;
        }
        return false;
    }

    bool is_report_to_generate() {
        return keyboard_state_->keycodes[KCT_KEY] != keyboard_state_->prev_keycodes[KCT_KEY];
    }

    HIDKeyReport generate_report() {
        uint8_t modifier = 0;
        vector<uint8_t> keycodes{};
        for (auto &keycode : keyboard_state_->keycodes[KCT_KEY]) {
            if (keycode >= HID_KEY_CONTROL_LEFT) {
                modifier = modifier | MODIFIER_KEYCODE_TO_BIT[static_cast<uint8_t>(keycode)];
            } else {
                keycodes.emplace_back(static_cast<uint8_t>(keycode));
            }
        }
        if (keycodes.size() > 6) {
            return {.modifier = modifier, .keycode = {1, 1, 1, 1, 1, 1}};
        } else {
            HIDKeyReport report = {.modifier = modifier, .keycode = {}};
            uint8_t index = 0;
            for (auto keycode : keycodes) {
                report.keycode[index] = keycode;
                if (++index == 6) {
                    break;
                }
            }
            return report;
        }
    }
};
