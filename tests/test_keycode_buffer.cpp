#include <cassert>
#include <iostream>
#include <ranges>
#include <vector>
#include "mkeybo/components/base.hpp"
#include "mkeybo/components/KeyboardState.hpp"
#include "test_keycode_buffer.hpp"

void test_keycode_event_buffer_push()
{
    std::cout << __func__ << "...";
    mkeybo::KeycodeEventBuffer<3> buffer;
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 100});
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 101});
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 102});
    // check items
    const std::vector<uint16_t> expected_items = {100, 101, 102};
    auto index = 0;
    for (const auto& keycode_event : buffer.get_all_events())
    {
        assert(keycode_event.keycode.code == expected_items[index]);
        index++;
    }
    // check rolled out
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 103});
    const std::vector<uint16_t> expected_items_rolled = {103, 101, 102};
    index = 0;
    for (const auto& keycode_event : buffer.get_all_events())
    {
        assert(keycode_event.keycode.code == expected_items_rolled[index]);
        index++;
    }
    std::cout << "PASS" << std::endl;
}

void test_keycode_event_buffer_get_finalized_events()
{
    std::cout << __func__ << "...";
    mkeybo::KeycodeEventBuffer<3> buffer;
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 100}, 0, mkeybo::KeycodeEventType::finalized);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 101},1, mkeybo::KeycodeEventType::canceled);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::cc, 102}, 2, mkeybo::KeycodeEventType::finalized);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 103}, 3, mkeybo::KeycodeEventType::draft);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::null}, 4, mkeybo::KeycodeEventType::finalized);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::sentinel}, 5, mkeybo::KeycodeEventType::finalized);
    const std::vector<uint16_t> expected_items = {100, 102};
    for (auto index=0; const auto& keycode_event : buffer.get_finalized_events()) {
        assert(keycode_event.type == mkeybo::KeycodeEventType::finalized);
        assert(keycode_event.keycode.code == expected_items[index]);
        index++;
    }
    std::cout << "PASS" << std::endl;
}

void test_keycode_event_buffer_get_finalized_events_only_one_type()
{
    std::cout << __func__ << "...";
    mkeybo::KeycodeEventBuffer<5> buffer;
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 100}, 0, mkeybo::KeycodeEventType::finalized);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::null}, 1, mkeybo::KeycodeEventType::canceled);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::cc, 102}, 2, mkeybo::KeycodeEventType::finalized);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 103}, 3, mkeybo::KeycodeEventType::finalized);
    buffer.push(mkeybo::Keycode{mkeybo::KeycodeType::hid, 104}, 4, mkeybo::KeycodeEventType::draft);
    // all no empty keycodes
    std::vector<mkeybo::Keycode> filtered_keycodes{
        mkeybo::Keycode{mkeybo::KeycodeType::hid, 100},
        mkeybo::Keycode{mkeybo::KeycodeType::hid, 103}
    };
    for (auto index = 0; const auto& keycode_event : buffer.get_finalized_events(mkeybo::KeycodeType::hid))
    {
        assert(filtered_keycodes[index] == keycode_event.keycode);
        index++;
    }
    std::cout << "PASS" << std::endl;
}
