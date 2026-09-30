cmake_minimum_required(VERSION 3.20.0)

list(APPEND BOARD_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(driver_sens2)

target_include_directories(app PRIVATE drivers/our_driver)

target_sources(app PRIVATE 
    src/main.c
    drivers/our_driver/our_driver.c
)

target_sources_ifdef(CONFIG_SHELL app PRIVATE drivers/our_driver/driver_shell.c)