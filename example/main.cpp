#include <status_led.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

extern "C" void app_main()
{
    status_led_handle_t handle;
    ESP_ERROR_CHECK(status_led_create(GPIO_NUM_22, 0, &handle));

    // Set manual state
    ESP_ERROR_CHECK(status_led_set_state(handle, true));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_state(handle, false));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Blinking
    ESP_ERROR_CHECK(status_led_set_interval(handle, 200, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_interval(handle, 40, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Done
    ESP_ERROR_CHECK(status_led_set_state(handle, false));
}
