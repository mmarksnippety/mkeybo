#pragma once

#include "mkeybo/components/base.hpp"
#include <array>
#include <limits>
#include <ranges>


namespace mkeybo::keyboard {

template <size_t keycodes_buffer_size>
class KeycodeEventBuffer
{
    std::array<KeycodeEvent, keycodes_buffer_size> buffer;
    uint8_t index = 0;

public:
    void reset()
    {
        index = 0;
        buffer.fill(KeycodeEvent{});
    }

    bool push(const KeycodeEvent& keycode_event)
    {
        if (index >= keycodes_buffer_size)
        {
            return false;
        }
        buffer[index] = keycode_event;
        index++;
        return true;
    }

    void push(const Keycode& keycode, const uint8_t switch_no = std::numeric_limits<uint8_t>::max(),
              const KeycodeEventType type = KeycodeEventType::draft,
              const KeycodeEventPriority priority = KeycodeEventPriority::normal)
    {
        push(KeycodeEvent{type, priority, keycode, switch_no});
    }

    auto& get_all_events() { return buffer; }

    auto get_filtered_events(const std::optional<KeycodeType> keycode_type = std::nullopt,
                             const std::optional<KeycodeEventType> keycode_event_type = std::nullopt,
                             const std::optional<KeycodeEventPriority> keycode_event_priority = std::nullopt)
    {
        return std::views::filter(buffer,
                                  [keycode_type, keycode_event_type, keycode_event_priority](const auto& keycode_event)
                                  {
                                      if (keycode_event.keycode.is_null() || keycode_event.keycode.is_sentinel() ||
                                          keycode_event.type
                                          == KeycodeEventType::canceled)
                                      {
                                          return false;
                                      }
                                      if (keycode_type && keycode_event.keycode.type != *keycode_type)
                                      {
                                          return false;
                                      }
                                      if (keycode_event_type && keycode_event.type != *keycode_event_type)
                                      {
                                          return false;
                                      }
                                      if (keycode_event_priority && keycode_event.priority != *keycode_event_priority)
                                      {
                                          return false;
                                      }
                                      return true;
                                  });
    }
};

}
