#include "status_led.h"

#if CONFIG_STATUS_LED_DEFAULT_ENABLE

status_led_handle_t STATUS_LED_DEFAULT = NULL;

esp_err_t status_led_create_default()
{
    return status_led_create(STATUS_LED_DEFAULT_GPIO, STATUS_LED_DEFAULT_ON, &STATUS_LED_DEFAULT);
}

#endif
