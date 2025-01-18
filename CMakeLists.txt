cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(PICO_BOARD pico CACHE STRING "Board type")
include(external/pico-sdk/pico_sdk_init.cmake)
project(mkeybo C CXX ASM)
pico_sdk_init()

#
# tests
#
add_executable(tests
        tests/main_tests.cpp
)
target_include_directories(tests PRIVATE
        tests
        mkeybo
        .
        mkeybo/defaults/
)
file(GLOB_RECURSE MKEYBO_SOURCES
        mkeybo/components/*.hpp
        debug.hpp
        factories.hpp
        consts.hpp
        tests/*
)
target_sources(tests PRIVATE ${MKEYBO_SOURCES})
target_compile_options(tests PRIVATE -Wall)
target_link_libraries(tests
        pico_stdlib
        tinyusb_device
        tinyusb_board
)
pico_set_program_name(tests "tests")
pico_set_program_version(tests "0.1")
pico_enable_stdio_uart(tests 1)
pico_enable_stdio_usb(tests 0)


#
# Keyboards
#
add_subdirectory(keyboards/lwings)