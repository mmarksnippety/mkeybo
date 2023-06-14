#pragma once

#include <iostream>

#include "CircularBuffer.h"
#include "HIDRaportGenerator.h"
#include "KeyboardConfig.h"
#include "KeyboardMapper.h"
#include "KeyboardState.h"
#include "SwitchReader.h"
#include "SwitchStateUpdater.h"


template <uint8_t switches_count>
class Keyboard {
protected:
    KeyboardSettings const *keyboard_settings_;
    KeyboardState<switches_count> *keyboard_state_;
    SwitchReader<switches_count> *switch_reader_;
    SwitchStateUpdater<switches_count> *switch_state_updater_;
    Mapper<switches_count> *mapper_;
    HIDKeyReporter<switches_count> *hid_key_reporter_;

public:
    Keyboard(
        KeyboardSettings const *keyboard_settings,
        KeyboardState<switches_count> *keyboard_state,
        SwitchReader<switches_count> *switch_reader,
        SwitchStateUpdater<switches_count> *switch_state_updater,
        Mapper<switches_count> *mapper,
        HIDKeyReporter<switches_count> *hid_key_reporter)
        : keyboard_settings_(keyboard_settings), keyboard_state_(keyboard_state),
          switch_reader_(switch_reader), switch_state_updater_(switch_state_updater), mapper_(mapper),
          hid_key_reporter_(hid_key_reporter) {}

    KeyboardSettings const *settings() { return keyboard_settings_; }

    KeyboardState<switches_count> *state() { return keyboard_state_; }

    HIDKeyReporter<switches_count> *hid_key_reporter() { return hid_key_reporter_; }

    virtual void init() {
        switch_reader_->init();
        hid_key_reporter_->init();
    }

    void refresh() {
        keyboard_state_->reset();
        switch_reader_->read_state();
        switch_state_updater_->update_state();
        mapper_->make_layer_mapping();
        on_layer_change();
        mapper_->make_mapping();
        hid_key_reporter_->blocking_enqueue_report();
    }

    virtual void task() {}

    // events handler
    virtual void on_layer_change() {}

    virtual void on_mount() {}

    virtual void on_umount() {}

    virtual void on_suspend() {}

    virtual void on_resume() {}
};
