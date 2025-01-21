#pragma once

#include <map>
#include "base.hpp"
#include "actions/base_action.hpp"


/**
 * Actions are triggered for KeycodeType::action
 * This mechanizm is usefully to make ex rebot, rebot to bootloader, make a macro,
 * make some think with keyboard parameters
 */

namespace mkeybo {


template <size_t switches_count, size_t keycodes_buffer_size>
class ActionManager
{
    std::map<Keycode, actions::BaseAction<switches_count, keycodes_buffer_size>*> actions{};

public:
    explicit ActionManager(const std::map<Keycode, actions::BaseAction<switches_count, keycodes_buffer_size>*>& actions) :
        actions(actions)
    {
    }

    ~ActionManager()
    {
        for (const auto& action : actions | std::views::values)
        {
            delete action;
        }
    }

    void make_actions(Keyboard<switches_count, keycodes_buffer_size>* keyboard)
    {
        for (auto& keycode_event : keyboard->get_filtered_keycode_events(
                 KeycodeType::action, KeycodeEventType::finalized))
        {
            auto action_it = actions.find(keycode_event.keycode);
            if (action_it != actions.end())
            {
                action_it->second->execute(keyboard, keycode_event.keycode);
            }
        }
    }
};


}
