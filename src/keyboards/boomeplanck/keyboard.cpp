#include "bsp/board.h"
#include "config.h"
#include "factories.cpp"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include <iostream>


using namespace std;


char const *string_desc_arr[] = {
    (const char[]){0x09, 0x04},// 0: is supported language is English (0x0409)
    KEYBOARD_MANUFACTURER,     // 1: Manufacturer
    KEYBOARD_MODEL,            // 2: Product
    "0"                        // 3: Pico board id, load in runtime
};
Keyboard<SWITCH_MATRIX_SWITCHES_COUNT> *keyboard;
struct repeating_timer keyboard_task_timer {};
struct repeating_timer hid_task_timer {};


// Every keyboard->settings()->keyboard_refresh_interval_ms (efault 50ms),
// we will refresh switches state, make mapping and generate hid reports
bool keyboard_task(repeating_timer *t) {
    keyboard->refresh();
    return true;
}

// Send hid report
// if for given report_id no report, then we will try to send report with next id
void send_hid_report(uint8_t report_id) {
    if (!tud_hid_ready()) {
        return;
    }
    if (report_id == REPORT_ID_KEYBOARD) {
        auto reporter = reinterpret_cast<HIDKeyReporter<SWITCH_MATRIX_SWITCHES_COUNT> *>(
            keyboard->get_hid_reporter(KCT_KEY));
        if (reporter->is_report_to_send()) {
            auto report = reinterpret_cast<HIDKeyReport *>(reporter->blocking_dequeue_report());
            if (report != nullptr) {
                tud_hid_keyboard_report(REPORT_ID_KEYBOARD, report->modifier, report->keycode.data());
                return;
            }
        }
    }
    report_id = REPORT_ID_CONSUMER_CONTROL;// move forward if no report
    if (report_id == REPORT_ID_CONSUMER_CONTROL) {
        auto reporter = reinterpret_cast<HIDCCReporter<SWITCH_MATRIX_SWITCHES_COUNT> *>(
            keyboard->get_hid_reporter(KCT_CC_KEY));
        if (reporter->is_report_to_send()) {
            auto report = reinterpret_cast<HIDCCReport *>(reporter->blocking_dequeue_report());
            if (report != nullptr) {
                tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &report->keycode, 2);
                return;
            }
        }
    }
    // TODO: make other reporter - mouse, gamepad
}


// Every keyboard->settings()->usb_refresh_interval_ms (efault 10ms),
// we will send 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
bool hid_task(repeating_timer *t) {
    bool report_to_send = keyboard->is_any_hid_raport_to_send();
    if (report_to_send) {
        // now we have only keyboard tasks, but in the future we may more types.
        if (tud_suspended()) {
            // Wake up host if we are in suspend mode
            // and REMOTE_WAKEUP feature is enabled by host
            tud_remote_wakeup();
        } else {
            // Send the 1st of report chain, the rest will be sent by tud_hid_report_complete_cb()
            send_hid_report(REPORT_ID_KEYBOARD);
        }
    }
    return true;
}


int main() {
    stdio_init_all();
    board_init();
    cout << "Construct keyboard instance" << endl;
    keyboard = keyboard_factory();
    keyboard->init();
    cout << "Init USB subsystem" << endl;
    tusb_init();
    cout << "Init tasks" << endl;
    add_repeating_timer_ms(
        keyboard->settings()->keyboard_refresh_interval_ms, keyboard_task, nullptr, &keyboard_task_timer);
    add_repeating_timer_ms(
        keyboard->settings()->usb_refresh_interval_ms, hid_task, nullptr, &hid_task_timer);
    cout << "Start main loop" << endl;
    while (true) {
        tud_task();
        keyboard->task();
    }
}


//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) { keyboard->on_mount(); }

// Invoked when device is unmounted
void tud_umount_cb(void) { keyboard->on_umount(); }


// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void)remote_wakeup_en;
    keyboard->on_suspend();
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) { keyboard->on_resume(); }

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+


// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len) {
    (void)instance;
    (void)len;
    uint8_t next_report_id = report[0] + 1;
    if (next_report_id < REPORT_ID_COUNT) {
        send_hid_report(next_report_id);
    }
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(
    uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(
    uint8_t instance,
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t bufsize) {
    (void)instance;
    if (report_type == HID_REPORT_TYPE_OUTPUT) {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD) {
            // bufsize should be (at least) 1
            if (bufsize < 1) return;
            keyboard->on_report_receive(buffer);
        }
    }
}