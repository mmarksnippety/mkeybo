cmake_minimum_required(VERSION 3.13)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(PICO_SDK_PATH "/home/mmark/Projects/pico/pico-sdk")
set(PICO_BOARD pico CACHE STRING "Board type")
# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.5.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.5.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif ()

project(mkeybo C CXX ASM)
pico_sdk_init()

# Main executable. Default name is the project name, version 0.1
#add_executable(mkeybo
#        src/mkeybo.cpp__
#        )
#pico_set_program_name(mkeybo "mkeybo")
#pico_set_program_version(mkeybo "0.1")
#pico_enable_stdio_uart(mkeybo 1)
#pico_enable_stdio_usb(mkeybo 0)
#target_link_libraries(mkeybo
#        pico_stdlib
#        pico_unique_id
#        hardware_pio
#        tinyusb_device
#        tinyusb_board
#        )
#
#target_include_directories(mkeybo PUBLIC
#        #  ${CMAKE_CURRENT_LIST_DIR}/
#        ${CMAKE_CURRENT_LIST_DIR}/src/
#        )
#target_sources(mkeybo PRIVATE
#        ${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.c
#        )
#pico_add_extra_outputs(mkeybo)


# switch matrix test
add_executable(boomeplanck
        src/keyboards/boomeplanck/keyboard.cpp
        )
target_compile_definitions(boomeplanck PRIVATE
    PICO_FLASH_SIZE_BYTES=4194304
#    PICO_DEBUG_MALLOC=1
#    PICO_DEBUG_MALLOC_LOW_WATER=1
#    PICO_HEAP_SIZE=4*4096
#    PICO_PHEAP_MAX_ENTRIES=512
# PICO_USE_STACK_GUARDS=1
#    PICO_STACK_SIZE=4096
)
pico_set_program_name(boomeplanck "boomeplanck")
pico_set_program_version(boomeplanck "0.1")
pico_enable_stdio_uart(boomeplanck 1)
pico_enable_stdio_usb(boomeplanck 0)
target_link_libraries(boomeplanck
        pico_stdlib
        pico_unique_id
        pico_bootrom
        hardware_pio
        hardware_pwm
        tinyusb_device
        tinyusb_board
        )
target_include_directories(boomeplanck PUBLIC
        #  ${CMAKE_CURRENT_LIST_DIR}/
        ${CMAKE_CURRENT_LIST_DIR}/src/
        )
target_sources(boomeplanck PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.c
#        ${CMAKE_CURRENT_LIST_DIR}/src/keyboards/boomeplanck/usb.cpp
        )
pico_add_extra_outputs(boomeplanck)


# Tests
add_executable(tests
        tests/tests.cpp
        #        tests/test_keyboard_matrix.cpp
        #        tests/test_keyboard_mapper.cpp
        )
pico_set_program_name(tests "mkeybo tests")
pico_set_program_version(tests "0.1")
pico_enable_stdio_uart(tests 1)
pico_enable_stdio_usb(tests 0)
target_link_libraries(tests PUBLIC
        pico_stdlib
        pico_unique_id
        hardware_pio
        tinyusb_device
        tinyusb_board
        )
target_include_directories(tests PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/
        ${CMAKE_CURRENT_LIST_DIR}/tests/
        )
target_sources(tests PRIVATE
        #        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        #        ${CMAKE_CURRENT_LIST_DIR}/KeyboardMapper.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/usb_descriptors.c
        #        ${CMAKE_CURRENT_LIST_DIR}/tests/test_keyboard
        )
target_include_directories(tests PRIVATE
        #        ${CMAKE_CURRENT_LIST_DIR}/
        #        ${CMAKE_CURRENT_LIST_DIR}/src/..
        ${CMAKE_CURRENT_LIST_DIR}/tests/...
        )
pico_add_extra_outputs(tests)
