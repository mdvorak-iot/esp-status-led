#include "status_led.h"
#include <driver/gpio.h>
#include <esp_timer.h>
#include <esp_log.h>

static const char TAG[] = "status_led";

struct status_led_handle
{
    gpio_num_t pin;
    uint32_t on_state;
    uint32_t off_state;
    esp_timer_handle_t timer;
    volatile bool status;
};

static void status_led_timer_callback(void *arg)
{
    status_led_handle_t handle = (status_led_handle_t)arg;
    assert(handle);

    // Toggle status
    handle->status = !handle->status;

    // Set LED state
    gpio_set_level(handle->pin, handle->status ? handle->on_state : handle->off_state);
}

esp_err_t status_led_create(gpio_num_t pin, uint32_t on_state, status_led_handle_t *out_handle)
{
    if (out_handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    status_led_handle_t result = (status_led_handle_t)malloc(sizeof(*result));
    if (result == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    // Configure
    result->pin = pin;
    result->on_state = on_state;
    result->off_state = (~on_state) & 1;
    result->status = 0;
    result->timer = NULL;

    // Timer config
    esp_timer_create_args_t args = {
        .callback = status_led_timer_callback,
        .arg = result,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "status_led",
    };

    // GPIO config
    esp_err_t err = gpio_reset_pin(pin);
    if (err != ESP_OK)
    {
        status_led_delete(result);
        return err;
    }

    err = gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    if (err != ESP_OK)
    {
        status_led_delete(result);
        return err;
    }

    err = gpio_set_level(pin, on_state);
    if (err != ESP_OK)
    {
        status_led_delete(result);
        return err;
    }

    err = esp_timer_create(&args, &result->timer);
    if (err != ESP_OK)
    {
        status_led_delete(result);
        return err;
    }

    // Success
    *out_handle = result;
    ESP_LOGI(TAG, "initialized on pin %d", pin);
    return ESP_OK;
}

esp_err_t status_led_set_interval(status_led_handle_t handle, uint32_t interval_ms, bool initial_state)
{
    if (handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    // Stop current
    esp_err_t err = esp_timer_stop(handle->timer);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        return err;
    }

    // Set initial state
    handle->status = initial_state;
    gpio_set_level(handle->pin, initial_state ? handle->on_state : handle->off_state);

    // If requested, restart timer
    if (interval_ms > 0)
    {
        // NOTE interval divide by two, because we need to turn it on and off during one cycle
        err = esp_timer_start_periodic(handle->timer, interval_ms * 1000 / 2); // period in us
        if (err != ESP_OK)
        {
            return err;
        }
    }

    // Success
    ESP_LOGI(TAG, "set interval %d ms on pin %d with state %s", interval_ms, handle->pin, initial_state ? "on" : "off");
    return ESP_OK;
}

esp_err_t status_led_delete(status_led_handle_t handle)
{
    if (handle == NULL)
    {
        return ESP_OK;
    }

    gpio_num_t pin = handle->pin;
    esp_timer_handle_t timer = handle->timer;

    // Try stopping timer
    if (timer)
    {
        esp_timer_stop(timer); // Ignore error here
    }

    // Free handle
    free(handle);

    // Delete timer
    if (timer)
    {

        esp_err_t err = esp_timer_delete(timer);
        if (err != ESP_OK)
        {
            return err;
        }
    }

    // Done
    ESP_LOGI(TAG, "deleted on pin %d", pin);
    return ESP_OK;
}
