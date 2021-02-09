#include <status_led.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

static const char TAG[] = "example";

extern "C" void app_main()
{
    status_led_handle_t handle;
    ESP_ERROR_CHECK(status_led_create(GPIO_NUM_22, 0, &handle));

    // Set manual state
    ESP_LOGI(TAG, "Manual");
    ESP_ERROR_CHECK(status_led_set_state(handle, true));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_state(handle, false));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Blinking
    ESP_LOGI(TAG, "Blinking");
    ESP_ERROR_CHECK(status_led_set_interval(handle, 200, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_interval(handle, 40, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Just delay
    ESP_ERROR_CHECK(status_led_set_state(handle, false));
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Timeout
    ESP_LOGI(TAG, "Timeout");
    ESP_ERROR_CHECK(status_led_set_interval_for(handle, 200, true, 4000, true));
    vTaskDelay(200 / portTICK_PERIOD_MS); // intentionally less then timeout

    ESP_ERROR_CHECK(status_led_set_interval_for(handle, 200, true, 1000, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_interval_for(handle, 40, true, 1000, false));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Timeout without blinking
    ESP_LOGI(TAG, "Timeout without blinking");
    ESP_ERROR_CHECK(status_led_set_interval_for(handle, 0, true, 1000, false));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Manual toggle
    ESP_LOGI(TAG, "Toggle");
    ESP_ERROR_CHECK(status_led_toggle_state(handle)); // off -> on
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(status_led_toggle_state(handle)); // on -> off

    // Cleanup
    ESP_LOGI(TAG, "Cleanup");
    ESP_ERROR_CHECK(status_led_delete(handle));
}
