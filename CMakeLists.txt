cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(PICO_BOARD pico CACHE STRING "Board type")
include(external/pico-sdk/pico_sdk_init.cmake)
project(mkeybo2 C CXX ASM)
pico_sdk_init()

#
# tests
#
add_executable(tests
        tests/main_tests.cpp
)
target_include_directories(tests PRIVATE
        mkeybo
)
file(GLOB_RECURSE MKEYBO_HEADER_FILES "mkeybo/*.hpp")
target_sources(tests PRIVATE ${MKEYBO_HEADER_FILES})
pico_set_program_name(tests "tests")
pico_set_program_version(tests "0.1")
pico_enable_stdio_uart(tests 1)
pico_enable_stdio_usb(tests 0)
target_link_libraries(tests
        pico_stdlib
)

#
# lwings - put it to lwings directory
add_executable(lwings
        keyboards/lwings/keyboard.cpp
)
pico_set_program_name(lwings "lwings")
pico_set_program_version(lwings "0.1")
pico_enable_stdio_uart(lwings 1)
pico_enable_stdio_usb(lwings 0)
target_link_libraries(lwings
        pico_stdlib
)
target_include_directories(lwings PRIVATE
        mkeybo
        . # for our common lwipopts or any other standard includes, if required
)
target_link_libraries(lwings
        hardware_pio
        hardware_timer
        hardware_clocks
)
pico_add_extra_outputs(lwings)


