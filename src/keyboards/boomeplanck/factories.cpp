#include "Keyboard.h"
#include "components/SwitchMatrix.h"
#include "config.h"
#include "mapping.h"


KeyboardSettings *keyboard_settings_factory() { return new KeyboardSettings(50, 10, 50, 100, 300); }


Mapper<SWITCH_MATRIX_SWITCHES_COUNT> *mapper_factory(
    MapperConfig *mapper_config,
    KeyboardSettings const *keyboard_settings,
    KeyboardState<SWITCH_MATRIX_SWITCHES_COUNT> *keyboard_state) {
    // clang-format off
    return new Mapper<SWITCH_MATRIX_SWITCHES_COUNT>(
        mapper_config,
        keyboard_settings,
        keyboard_state,
        {
            new MapperRuleKey<SWITCH_MATRIX_SWITCHES_COUNT>(keyboard_settings, keyboard_state),
            new MapperRuleMultiSwitchKey<SWITCH_MATRIX_SWITCHES_COUNT>(keyboard_settings, keyboard_state),
//            new MapperRuleTapDanceKey<SWITCH_MATRIX_SWITCHES_COUNT>(keyboard_settings, keyboard_state),
        }
    );
    // clang-format on
}


BoomeplanckKeyboard<SWITCH_MATRIX_SWITCHES_COUNT> *keyboard_factory() {
    auto const keyboard_settings = keyboard_settings_factory();
    auto keyboard_state = new KeyboardState<SWITCH_MATRIX_SWITCHES_COUNT>();
    auto switch_reader = new MatrixSwitchReader<SWITCH_MATRIX_SWITCHES_COUNT>(
        SWITCH_MATRIX_COL_START_PIN,
        SWITCH_MATRIX_COLS,
        SWITCH_MATRIX_ROW_START_PIN,
        SWITCH_MATRIX_ROWS,
        keyboard_state);
    auto switch_state_updater =
        new SwitchStateUpdater<SWITCH_MATRIX_SWITCHES_COUNT>(keyboard_settings, keyboard_state);
    auto mapper_config = mapper_config_factory();
    auto mapper = mapper_factory(mapper_config, keyboard_settings, keyboard_state);
    auto left_led = new PWMLed(5, 0);
    auto right_led = new PWMLed(4, 0);
    return new BoomeplanckKeyboard<SWITCH_MATRIX_SWITCHES_COUNT>(
        keyboard_settings,
        keyboard_state,
        switch_reader,
        switch_state_updater,
        mapper,
        {new HIDKeyReporter<SWITCH_MATRIX_SWITCHES_COUNT>(keyboard_state),
         new HIDCCReporter<SWITCH_MATRIX_SWITCHES_COUNT>(keyboard_state),},
        right_led,
        left_led);
}
