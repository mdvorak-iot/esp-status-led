#ifndef STATUS_LED_H
#define STATUS_LED_H

#include <esp_err.h>
#include <driver/gpio.h>

#ifndef STATUS_LED_DEFAULT_GPIO
#define STATUS_LED_DEFAULT_GPIO CONFIG_STATUS_LED_DEFAULT_GPIO
#endif

#ifndef STATUS_LED_DEFAULT_ON
#define STATUS_LED_DEFAULT_ON CONFIG_STATUS_LED_DEFAULT_ON
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct status_led_handle *status_led_handle_t;

    esp_err_t status_led_create(gpio_num_t pin, uint32_t on_state, status_led_handle_t *out_handle);

    esp_err_t status_led_delete(status_led_handle_t handle);

    esp_err_t status_led_set_interval_for(status_led_handle_t handle, uint32_t interval_ms, bool initial_state, uint32_t timeout_ms, bool final_state);

    inline esp_err_t status_led_set_interval(status_led_handle_t handle, uint32_t interval_ms, bool initial_state)
    {
        return status_led_set_interval_for(handle, interval_ms, initial_state, 0, initial_state);
    }

    inline esp_err_t status_led_set_state(status_led_handle_t handle, bool state)
    {
        return status_led_set_interval_for(handle, 0, state, 0, state);
    }

    esp_err_t status_led_toggle_state(status_led_handle_t handle);

#if CONFIG_STATUS_LED_DEFAULT_ENABLE

    extern status_led_handle_t STATUS_LED_DEFAULT;

    esp_err_t status_led_create_default();
    
#endif

#ifdef __cplusplus
}
#endif

#endif
