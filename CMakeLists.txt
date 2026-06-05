cmake_minimum_required(VERSION 3.13)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(as5600_usb C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(as5600_usb
    src/main.cpp
)

target_include_directories(as5600_usb PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(as5600_usb
    pico_stdlib
    hardware_i2c
    hardware_gpio
)

pico_enable_stdio_usb(as5600_usb 1)
pico_enable_stdio_uart(as5600_usb 0)

pico_add_extra_outputs(as5600_usb)
