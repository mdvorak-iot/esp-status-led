#include "status_led.h"

#if CONFIG_DEFAULT_STATUS_LED_ENABLE

status_led_handle_t DEFAULT_STATUS_LED = NULL;

esp_err_t status_led_create_default()
{
    return status_led_create(DEFAULT_STATUS_LED_GPIO, DEFAULT_STATUS_LED_ON, &DEFAULT_STATUS_LED);
}

#endif
