#pragma once

#include <driver/gpio.h>
#include <esp_err.h>
#include <stdbool.h>
#include "sdkconfig.h"

#ifndef STATUS_LED_DEFAULT_GPIO
#define STATUS_LED_DEFAULT_GPIO CONFIG_STATUS_LED_DEFAULT_GPIO
#endif

#ifndef STATUS_LED_DEFAULT_ON
#define STATUS_LED_DEFAULT_ON CONFIG_STATUS_LED_DEFAULT_ON
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct status_led_handle *status_led_handle_ptr;

esp_err_t status_led_create(gpio_num_t pin, uint32_t on_state, status_led_handle_ptr *out_handle);

esp_err_t status_led_delete(status_led_handle_ptr handle);

esp_err_t status_led_set_interval_for(status_led_handle_ptr handle, uint32_t interval_ms, bool initial_state, uint32_t timeout_ms, bool final_state);

esp_err_t status_led_set_interval(status_led_handle_ptr handle, uint32_t interval_ms, bool initial_state);

esp_err_t status_led_set_state(status_led_handle_ptr handle, bool state);

esp_err_t status_led_toggle_state(status_led_handle_ptr handle);

bool status_led_is_active(status_led_handle_ptr handle);

// Default handle
#if CONFIG_STATUS_LED_DEFAULT_ENABLE

extern status_led_handle_ptr STATUS_LED_DEFAULT;

esp_err_t status_led_create_default();

#endif

#ifdef __cplusplus
}
#endif
