cmake_minimum_required(VERSION 3.13)
include(pico_sdk_import.cmake)

project(WebPad360 C CXX ASM)
set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(WebPad360
    main.c
    index_html.c
    xinput/xinput_driver.c
    xinput/xinput_descriptors.c
)

target_include_directories(WebPad360 PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/config
    ${CMAKE_CURRENT_LIST_DIR}/xinput
)

target_link_libraries(WebPad360
    pico_stdlib
    pico_cyw43_arch_lwip_threadsafe_background
    pico_lwip_http
    tinyusb_device
    tinyusb_board
)

pico_add_extra_outputs(WebPad360)
pico_enable_stdio_usb(WebPad360 0)
pico_enable_stdio_uart(WebPad360 1)
