#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <status_led.h>

static const char TAG[] = "example";

extern "C" void app_main()
{
    ESP_ERROR_CHECK(status_led_create_default());

    assert(status_led_is_active(STATUS_LED_DEFAULT) == false);

    // Set manual state
    ESP_LOGI(TAG, "Manual");
    ESP_ERROR_CHECK(status_led_set_state(STATUS_LED_DEFAULT, true));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_state(STATUS_LED_DEFAULT, false));
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Blinking
    ESP_LOGI(TAG, "Blinking");
    ESP_ERROR_CHECK(status_led_set_interval(STATUS_LED_DEFAULT, 200, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK(status_led_set_interval(STATUS_LED_DEFAULT, 40, true));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Just delay
    ESP_ERROR_CHECK(status_led_set_state(STATUS_LED_DEFAULT, false));
    vTaskDelay(500 / portTICK_PERIOD_MS);

    // Timeout
    ESP_LOGI(TAG, "Timeout");
    ESP_ERROR_CHECK(status_led_set_interval_for(STATUS_LED_DEFAULT, 200, true, 4000, true));
    vTaskDelay(200 / portTICK_PERIOD_MS); // intentionally less then timeout

    ESP_ERROR_CHECK(status_led_set_interval_for(STATUS_LED_DEFAULT, 200, true, 1000, true));
    assert(status_led_is_active(STATUS_LED_DEFAULT) == true);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    assert(status_led_is_active(STATUS_LED_DEFAULT) == false);

    ESP_ERROR_CHECK(status_led_set_interval_for(STATUS_LED_DEFAULT, 40, true, 1000, false));
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    // Timeout without blinking
    ESP_LOGI(TAG, "Timeout without blinking");
    ESP_ERROR_CHECK(status_led_set_interval_for(STATUS_LED_DEFAULT, 0, true, 1000, false));
    assert(status_led_is_active(STATUS_LED_DEFAULT) == true);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    assert(status_led_is_active(STATUS_LED_DEFAULT) == false);

    // Manual toggle
    ESP_LOGI(TAG, "Toggle");
    ESP_ERROR_CHECK(status_led_toggle_state(STATUS_LED_DEFAULT)); // off -> on
    assert(status_led_is_active(STATUS_LED_DEFAULT) == false);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(status_led_toggle_state(STATUS_LED_DEFAULT)); // on -> off
    assert(status_led_is_active(STATUS_LED_DEFAULT) == false);

    // Cleanup
    ESP_LOGI(TAG, "Cleanup");
    ESP_ERROR_CHECK(status_led_delete(STATUS_LED_DEFAULT));
}
