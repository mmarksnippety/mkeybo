#include "test_usb_reporter.hpp"
#include <iostream>
#include <bitset>
#include <cassert>
#include "mkeybo/components/keyboard_state.hpp"
#include "mkeybo/components/usb_reporter.hpp"
#include "mkeybo/components/base.hpp"
#include "mkeybo/factories.hpp"


void test_hid_reporter_get_regular_keycodes()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto hid_reporter = new mkeybo::UsbHidKeycodeReporter<switches_count>();
    std::cout << __func__ << "...";
    k_state->push_keycode_event(H_K(HID_KEY_A), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_B), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_SHIFT_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_Q), 0, mkeybo::KeycodeEventType::finalized);
    std::vector<uint8_t> expected_keycodes = {HID_KEY_A, HID_KEY_B, HID_KEY_Q};
    auto keycodes_view = hid_reporter->get_regular_keycodes(k_state, mkeybo::KeycodeEventPriority::normal);
    auto keycodes_view_it = keycodes_view.begin();
    const auto keycodes_view_end = keycodes_view.end();
    for (const auto& expected_keycode : expected_keycodes)
    {
        assert(keycodes_view_it->keycode.code == expected_keycode);
        ++keycodes_view_it;
    }
    assert(keycodes_view_it == keycodes_view_end);
    std::cout << "PASS" << std::endl;
    delete hid_reporter;
    delete k_state;
}

void test_hid_reporter_get_modifier_keycodes()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto hid_reporter = new mkeybo::UsbHidKeycodeReporter<switches_count>();
    std::cout << __func__ << "...";
    k_state->push_keycode_event(H_K(HID_KEY_GUI_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_B), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_SHIFT_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_Q), 0, mkeybo::KeycodeEventType::finalized);
    const std::vector<uint8_t> expected_keycodes = {HID_KEY_GUI_LEFT, HID_KEY_SHIFT_LEFT};
    auto keycodes_view = hid_reporter->get_modifiers_keycodes(k_state, mkeybo::KeycodeEventPriority::normal);
    auto keycodes_view_it = keycodes_view.begin();
    const auto keycodes_view_end = keycodes_view.end();
    for (const auto& expected_keycode : expected_keycodes)
    {
        assert(keycodes_view_it->keycode.code == expected_keycode);
        ++keycodes_view_it;
    }
    assert(keycodes_view_it == keycodes_view_end);
    std::cout << "PASS" << std::endl;
    delete hid_reporter;
    delete k_state;
}

void test_hid_reporter_generate_report_modifiers()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto hid_reporter = new mkeybo::UsbHidKeycodeReporter<switches_count>();
    std::cout << __func__ << "...";
    k_state->push_keycode_event(H_K(HID_KEY_GUI_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_B), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_SHIFT_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_Q), 0, mkeybo::KeycodeEventType::finalized);
    const auto report = new mkeybo::UsbHidKeycodeReport();
    hid_reporter->generate_report_modifiers(hid_reporter->get_modifiers_keycodes(k_state, mkeybo::KeycodeEventPriority::normal), report);
    assert(report->modifiers == 0b00001010);
    std::cout << "PASS" << std::endl;
    delete hid_reporter;
    delete k_state;
    delete report;
}

void test_hid_reporter_generate_report_keycodes()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto hid_reporter = new mkeybo::UsbHidKeycodeReporter<switches_count>();
    std::cout << __func__ << "...";
    k_state->push_keycode_event(H_K(HID_KEY_GUI_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_B), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_SHIFT_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_Q), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_W), 0, mkeybo::KeycodeEventType::finalized);
    const auto report = new mkeybo::UsbHidKeycodeReport();
    hid_reporter->generate_report_keycodes(hid_reporter->get_regular_keycodes(k_state, mkeybo::KeycodeEventPriority::normal), report);
    assert(report->keycodes[0] == HID_KEY_B);
    assert(report->keycodes[1] == HID_KEY_Q);
    assert(report->keycodes[2] == HID_KEY_W);
    assert(report->keycodes[3] == 0);
    assert(report->keycodes[4] == 0);
    assert(report->keycodes[5] == 0);
    std::cout << "PASS" << std::endl;
    delete hid_reporter;
    delete k_state;
    delete report;
}


void test_reporter_manager_generate_reports()
{
    constexpr auto switches_count = 4;
    const auto k_state = new mkeybo::KeyboardState<switches_count>();
    const auto reporter_manager = mkeybo::create_usb_reporter_manager<switches_count>();
    reporter_manager->generate_empty_records(k_state);
    assert(k_state->get_usb_reports().size() == 2);
    std::cout << __func__ << "...";
    k_state->push_keycode_event(H_K(HID_KEY_GUI_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_B), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_SHIFT_LEFT), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_Q), 0, mkeybo::KeycodeEventType::finalized);
    k_state->push_keycode_event(H_K(HID_KEY_W), 0, mkeybo::KeycodeEventType::finalized);
    reporter_manager->generate_reports(k_state);
    const auto usb_hid_report = reinterpret_cast<mkeybo::UsbHidKeycodeReport*>(k_state->get_usb_reports().at(mkeybo::KeycodeType::hid));
    assert(usb_hid_report->status == mkeybo::UsbReportStatus::ready);
    assert(usb_hid_report->modifiers == 0b00001010);
    assert(usb_hid_report->keycodes[0] == HID_KEY_B);
    assert(usb_hid_report->keycodes[1] == HID_KEY_Q);
    assert(usb_hid_report->keycodes[2] == HID_KEY_W);
    assert(usb_hid_report->keycodes[3] == 0);
    assert(usb_hid_report->keycodes[4] == 0);
    assert(usb_hid_report->keycodes[5] == 0);
    const auto usb_cc_report = reinterpret_cast<mkeybo::UsbCcKeycodeReport*>(k_state->get_usb_reports().at(mkeybo::KeycodeType::cc));
    assert(usb_cc_report->status == mkeybo::UsbReportStatus::draft);
    assert(usb_cc_report->keycode == 0);
    std::cout << "PASS" << std::endl;
    delete reporter_manager;
    delete k_state;
}


