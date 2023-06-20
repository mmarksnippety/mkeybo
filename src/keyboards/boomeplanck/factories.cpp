#include "Keyboard.h"
#include "components/Actions.h"
#include "components/SwitchMatrix.h"
#include "config.h"
#include "mapping.h"


KeyboardSettings *keyboard_settings_factory() { return new KeyboardSettings(50, 10, 50, 100, 300); }


template <uint8_t switches_count>
Mapper<switches_count> *mapper_factory(
    MapperConfig *mapper_config,
    KeyboardSettings const *keyboard_settings,
    KeyboardState<switches_count> *keyboard_state) {
    // clang-format off
    return new Mapper<switches_count>(
        mapper_config,
        keyboard_settings,
        keyboard_state,
        {
            new MapperRuleKey<switches_count>(keyboard_settings, keyboard_state),
            new MapperRuleMultiSwitchKey<switches_count>(keyboard_settings, keyboard_state),
//            new MapperRuleTapDanceKey<switches_count>(keyboard_settings, keyboard_state),
        }
    );
    // clang-format on
}


template <uint8_t switches_count>
BoomeplanckKeyboard<switches_count> *keyboard_factory() {
    auto const keyboard_settings = keyboard_settings_factory();
    auto keyboard_state = new KeyboardState<switches_count>();
    auto switch_reader = new MatrixSwitchReader<switches_count>(
        SWITCH_MATRIX_COL_START_PIN,
        SWITCH_MATRIX_COLS,
        SWITCH_MATRIX_ROW_START_PIN,
        SWITCH_MATRIX_ROWS,
        keyboard_state);
    auto switch_state_updater = new SwitchStateUpdater<switches_count>(keyboard_settings, keyboard_state);
    auto mapper_config = mapper_config_factory();
    auto mapper = mapper_factory<switches_count>(mapper_config, keyboard_settings, keyboard_state);
    // clang-format off
    return new BoomeplanckKeyboard<switches_count>(
        keyboard_settings,
        keyboard_state,
        switch_reader,
        switch_state_updater,
        mapper,
        {
            new HIDKeyReporter<switches_count>(keyboard_state),
            new HIDCCReporter<switches_count>(keyboard_state),
        },
        {
            new InternalAction<switches_count>(keyboard_state),
            new MacroAction<switches_count>(keyboard_state)
        },
        new PWMLed(4, 0),
        new PWMLed(5, 0)
    );
    // clang-format on
}
