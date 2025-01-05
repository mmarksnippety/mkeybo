#ifndef __mkeybo_keyboard_settings_hpp__
#define __mkeybo_keyboard_settings_hpp__

/**
 * Json example settings

    json_settings = {
      "keyboard_name": "My Keyboard",
      "manufactured_name": "DIY",
      "default_layout": "qwerty",
      "layouts": [
        {
          "name": "quwerty",
          "keycodes": [1,2,3,4,5,6,7,8,9,10,11,12]
        },
      ],
      "layers": [
        {
          "name": "fn",
          "keycodes": [10,20,30,40,50,60,70,80,90,100,110,120]
        },
        {
          "name": "media",
          "keycodes": [0,0,0,90,91,92,0,0,0,0,0,0]
        },
      ],
      "rules": [
        {
          "type_name": "change_layer",
          "json_config": "{"keys":[10,20], "layer":1}",
        },
        {
          "type_name": "default_mapping",
          "json_config": null,
        },
      ]
    }

  JSON data like this must be parsed and instance settings object.
 */


#include <string>
#include <cstdint>
#include <array>
#include <vector>

namespace mkeybo
{

    template<size_t key_count>
    struct Layer {
        std::string name;
        std::array<uint16_t, key_count> keycodes;
    };

    struct Rule {
        std::string type_name;
        std::string json_config;
        // pointer to rule instance?
    };

    template<size_t key_count>
    struct Settings {
        std::string keyboard_name;
        std::string manufactured_name;

        std::string default_layout;
        std::vector<Layer<key_count>> layouts;
        std::vector<Layer<key_count>> layers;
        std::vector<Rule> rules;
    };

};


#endif //__mkeybo_keyboard_settings_hpp__
