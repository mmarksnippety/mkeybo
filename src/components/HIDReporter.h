#pragma once

#include "pico/stdlib.h"
#include "tusb.h"
#include <algorithm>
#include <string>
#include <vector>

#include "components/CircularBuffer.h"
#include "components/KeyboardConfig.h"
#include "components/KeyboardState.h"
#include "keyboards/boomeplanck/config.h"


using namespace std;


class HIDReport {};


class HIDKeyReport : public HIDReport {
public:
    uint8_t modifier;
    array<uint8_t, 6> keycode;
    HIDKeyReport(uint8_t modifier, const array<uint8_t, 6> keycode)
        : modifier(modifier), keycode(keycode) {}
};


struct HIDCCReport : public HIDReport {
public:
    uint16_t keycode;
    explicit HIDCCReport(uint16_t keycode) : keycode(keycode) {}
};


template <uint8_t switches_count>
class HIDReporter {
public:
    KeycodeType keycode_type;

protected:
    KeyboardState<switches_count> *keyboard_state_;
    CircularBuffer<HIDReport *, 6> report_buffer_{};
    critical_section_t send_report_lock_{};

public:
    explicit HIDReporter(KeyboardState<switches_count> *keyboard_state, KeycodeType keycode_type)
        : keyboard_state_(keyboard_state), keycode_type(keycode_type) {}

    void init() { critical_section_init(&send_report_lock_); }

    void run() { blocking_enqueue_report(); }

    bool is_report_to_send() { return !report_buffer_.empty(); }

    virtual bool is_report_to_generate() { return keyboard_state_->are_keycodes_changed(keycode_type); }

    HIDReport *blocking_dequeue_report() {
        HIDReport *report = nullptr;
        critical_section_enter_blocking(&send_report_lock_);
        if (is_report_to_send()) {
            report_buffer_.remove_element(report);
        }
        critical_section_exit(&send_report_lock_);
        return report;
    }

    bool blocking_enqueue_report() {
        bool report_enqueue = false;
        critical_section_enter_blocking(&send_report_lock_);
        if (is_report_to_generate()) {
            auto report = generate_report();
            report_buffer_.add_element(report);
            report_enqueue = true;
        }
        critical_section_exit(&send_report_lock_);
        return report_enqueue;
    }

    virtual HIDReport *generate_report() { return nullptr; }
};


// HID keyboad reports


template <uint8_t switches_count>
class HIDKeyReporter : public HIDReporter<switches_count> {
    using HIDReporter<switches_count>::keyboard_state_;
    using HIDReporter<switches_count>::keycode_type;

public:
    explicit HIDKeyReporter(KeyboardState<switches_count> *keyboard_state)
        : HIDReporter<switches_count>(keyboard_state, KCT_KEY) {}

    HIDKeyReport *generate_report() {
        uint8_t modifier = 0;
        vector<uint8_t> keycodes{};
        for (auto &keycode : keyboard_state_->get_keycodes(keycode_type)) {
            if (keycode >= HID_KEY_CONTROL_LEFT) {
                modifier = modifier | MODIFIER_KEYCODE_TO_BIT[static_cast<uint8_t>(keycode)];
            } else {
                keycodes.emplace_back(static_cast<uint8_t>(keycode));
            }
        }
        if (keycodes.size() > 6) {
            return new HIDKeyReport(modifier, {1, 1, 1, 1, 1, 1});
        } else {
            auto report = new HIDKeyReport(modifier, {});
            uint8_t index = 0;
            for (auto keycode : keycodes) {
                report->keycode[index] = keycode;
                if (++index == 6) {
                    break;
                }
            }
            return report;
        }
    }
};


// CC HID report

template <uint8_t switches_count>
class HIDCCReporter : public HIDReporter<switches_count> {
    using HIDReporter<switches_count>::keyboard_state_;
    using HIDReporter<switches_count>::keycode_type;

public:
    explicit HIDCCReporter(KeyboardState<switches_count> *keyboard_state)
        : HIDReporter<switches_count>(keyboard_state, KCT_CC_KEY) {}

    HIDCCReport *generate_report() {
        auto keycodes = keyboard_state_->get_keycodes(keycode_type);
        if (keycodes.empty()) {
            return new HIDCCReport(0);
        }
        return new HIDCCReport(keycodes.front());
    }
};
