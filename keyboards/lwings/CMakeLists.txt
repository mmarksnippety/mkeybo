file(GLOB_RECURSE SOURCES
        *.cpp
        *.hpp
        *.h
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/usb_callbacks.cpp
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/usb_descriptors.cpp
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/usb_descriptors.hpp
)
add_executable(lwings ${SOURCES})
target_link_libraries(lwings
        hardware_pio
        hardware_timer
        hardware_clocks
        hardware_i2c
        pico_stdlib
        pico_unique_id
        tinyusb_device
        tinyusb_board
        pico_ssd1306
)
target_include_directories(lwings PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/mkeybo/defaults/
        .
)
pico_set_program_name(lwings "lwings")
pico_set_program_version(lwings "0.1")
pico_enable_stdio_uart(lwings 1)
pico_enable_stdio_usb(lwings 0)
pico_add_extra_outputs(lwings)
#target_compile_definitions(lwings PUBLIC
##        CFG_TUSB_DEBUG=3
##        PICO_DEBUG_MALLOC=1
##        PICO_DEBUG_MALLOC_LOW_WATER=1
#)