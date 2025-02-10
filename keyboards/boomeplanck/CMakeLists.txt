file(GLOB_RECURSE SOURCES
        *.cpp
        *.hpp
        *.h
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/usb_callbacks.cpp
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/usb_descriptors.cpp
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/usb_descriptors.hpp
)
add_executable(boomeplanck ${SOURCES})
target_link_libraries(boomeplanck
        hardware_pio
        hardware_timer
        hardware_clocks
        hardware_pwm
        pico_stdlib
        pico_unique_id
        pico_multicore
        tinyusb_device
        tinyusb_board
)
target_include_directories(boomeplanck PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/
        .
)
pico_set_program_name(boomeplanck "boomeplanck")
pico_set_program_version(boomeplanck "1.0.1")
pico_enable_stdio_uart(boomeplanck 1)
pico_enable_stdio_usb(boomeplanck 0)
pico_add_extra_outputs(boomeplanck)
