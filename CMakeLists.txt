cmake_minimum_required(VERSION 3.11.0)

idf_component_register(
        SRCS
        src/status_led.c
        src/status_led_default.c
        INCLUDE_DIRS include
        REQUIRES freertos driver log
)
