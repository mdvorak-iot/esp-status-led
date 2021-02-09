#include <status_led.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>

static const char TAG[] = "example";

extern "C" void app_main()
{
    ESP_ERROR_CHECK(status_led_create_default());

    // Set manual state
    ESP_LOGI(TAG, "Manual");
    ESP_ERROR_CHECK(status_led_set_state(DEFAULT_STATUS_LED, true));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_state(DEFAULT_STATUS_LED, false));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Blinking
    ESP_LOGI(TAG, "Blinking");
    ESP_ERROR_CHECK(status_led_set_interval(DEFAULT_STATUS_LED, 200, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_interval(DEFAULT_STATUS_LED, 40, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Just delay
    ESP_ERROR_CHECK(status_led_set_state(DEFAULT_STATUS_LED, false));
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Timeout
    ESP_LOGI(TAG, "Timeout");
    ESP_ERROR_CHECK(status_led_set_interval_for(DEFAULT_STATUS_LED, 200, true, 4000, true));
    vTaskDelay(200 / portTICK_PERIOD_MS); // intentionally less then timeout

    ESP_ERROR_CHECK(status_led_set_interval_for(DEFAULT_STATUS_LED, 200, true, 1000, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_interval_for(DEFAULT_STATUS_LED, 40, true, 1000, false));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Timeout without blinking
    ESP_LOGI(TAG, "Timeout without blinking");
    ESP_ERROR_CHECK(status_led_set_interval_for(DEFAULT_STATUS_LED, 0, true, 1000, false));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Manual toggle
    ESP_LOGI(TAG, "Toggle");
    ESP_ERROR_CHECK(status_led_toggle_state(DEFAULT_STATUS_LED)); // off -> on
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(status_led_toggle_state(DEFAULT_STATUS_LED)); // on -> off

    // Cleanup
    ESP_LOGI(TAG, "Cleanup");
    ESP_ERROR_CHECK(status_led_delete(DEFAULT_STATUS_LED));
}
