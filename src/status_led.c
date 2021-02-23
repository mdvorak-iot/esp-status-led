#include "status_led.h"
#include <esp_log.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <string.h>

static const char TAG[] = "status_led";

static portMUX_TYPE lock = portMUX_INITIALIZER_UNLOCKED;

struct status_led_handle
{
    gpio_num_t pin;
    uint32_t on_state;
    uint32_t off_state;
    esp_timer_handle_t timer;
    esp_timer_handle_t stop_timer;
    bool active;
    bool status;
    bool stop_status;
};

inline static esp_err_t status_led_set_level(status_led_handle_t handle)
{
    return gpio_set_level(handle->pin, handle->status ? handle->on_state : handle->off_state);
}

static void status_led_timer_callback(void *arg)
{
    status_led_handle_t handle = (status_led_handle_t)arg;
    assert(handle);

    // Toggle status
    handle->status = !handle->status;
    status_led_set_level(handle);
}

static void status_led_timer_stop(void *arg)
{
    status_led_handle_t handle = (status_led_handle_t)arg;
    assert(handle);

    ESP_LOGD(TAG, "timeout on pin %d", handle->pin);

    // Ignore error if timer is already stopped
    esp_err_t err = esp_timer_stop(handle->timer);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        ESP_LOGE(TAG, "failed to stop timeout timer: %d", err);
    }

    // Set final status
    handle->status = handle->stop_status;
    status_led_set_level(handle);
    handle->active = false;
}

static esp_err_t status_led_init_stop_timer(status_led_handle_t handle)
{
    // Already present
    if (handle->stop_timer)
    {
        return ESP_OK;
    }

    // Create new
    esp_timer_create_args_t args = {
        .callback = status_led_timer_stop,
        .arg = handle,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "status_led_stop",
    };

    return esp_timer_create(&args, &handle->stop_timer);
}

esp_err_t status_led_create(gpio_num_t pin, uint32_t on_state, status_led_handle_t *out_handle)
{
    if (out_handle == NULL || pin < 0 || !GPIO_IS_VALID_OUTPUT_GPIO(pin))
    {
        return ESP_ERR_INVALID_ARG;
    }

    status_led_handle_t result = (status_led_handle_t)malloc(sizeof(*result));
    if (result == NULL)
    {
        return ESP_ERR_NO_MEM;
    }
    memset(result, 0, sizeof(*result));

    // Configure
    result->pin = pin;
    result->on_state = on_state;
    result->off_state = on_state ? 0 : 1;

    // Timer config
    esp_timer_create_args_t args = {
        .callback = status_led_timer_callback,
        .arg = result,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "status_led",
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 4, 0) // NOT not released yet
        .skip_unhandled_events = true,
#endif
    };

    // GPIO config
    esp_err_t err = gpio_reset_pin(pin);
    if (err != ESP_OK)
    {
        goto error;
    }

    err = gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    if (err != ESP_OK)
    {
        goto error;
    }

    err = gpio_set_level(pin, result->off_state);
    if (err != ESP_OK)
    {
        goto error;
    }

    err = esp_timer_create(&args, &result->timer);
    if (err != ESP_OK)
    {
        goto error;
    }

    // Success
    *out_handle = result;
    ESP_LOGI(TAG, "initialized on pin %d", pin);
    return ESP_OK;

error:
    // Release memory and return error
    status_led_delete(result);
    return err;
}

esp_err_t status_led_set_interval_for(status_led_handle_t handle, uint32_t interval_ms, bool initial_state, uint32_t timeout_ms, bool final_state)
{
    if (handle == NULL)
    {
        return ESP_ERR_INVALID_ARG;
    }

    // Lock
    // NOTE set err and use goto exit; instead of return
    portENTER_CRITICAL_SAFE(&lock);
    esp_err_t err;
    bool active = false;

    // Stop current
    err = esp_timer_stop(handle->timer);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
    {
        goto exit;
    }
    // Cancel timeout as well
    if (handle->stop_timer)
    {
        err = esp_timer_stop(handle->stop_timer);
        if (err != ESP_OK && err != ESP_ERR_INVALID_STATE)
        {
            goto exit;
        }
    }

    // Set states
    handle->status = initial_state;
    handle->stop_status = final_state;
    status_led_set_level(handle);

    // Special timeout handling
    if (timeout_ms > 0)
    {
        // Make sure stop timer exists
        err = status_led_init_stop_timer(handle);
        if (err != ESP_OK)
        {
            goto exit;
        }

        // Schedule stop
        err = esp_timer_start_once(handle->stop_timer, timeout_ms * 1000);
        if (err != ESP_OK)
        {
            goto exit;
        }

        active = true;
        ESP_LOGD(TAG, "set timeout %d on pin %d ms into state %s", timeout_ms, handle->pin, final_state ? "on" : "off");
    }

    // If requested, restart timer
    if (interval_ms > 0)
    {
        // NOTE interval divide by two, because we need to turn it on and off during one cycle
        err = esp_timer_start_periodic(handle->timer, interval_ms * 1000 / 2); // period in us
        if (err != ESP_OK)
        {
            goto exit;
        }

        active = true;
    }

    // Success
    ESP_LOGD(TAG, "set interval %d ms on pin %d with state %s", interval_ms, handle->pin, initial_state ? "on" : "off");
    err = ESP_OK;

exit:
    handle->active = active;
    portEXIT_CRITICAL_SAFE(&lock);
    return err;
}

esp_err_t status_led_delete(status_led_handle_t handle)
{
    if (handle == NULL)
    {
        return ESP_OK;
    }

    gpio_num_t pin = handle->pin;
    esp_timer_handle_t timer = handle->timer;
    esp_timer_handle_t stop_timer = handle->stop_timer;

    // Try stopping timers
    if (timer)
    {
        esp_timer_stop(timer); // Ignore error
    }
    if (stop_timer)
    {
        esp_timer_stop(stop_timer); // Ignore error
    }

    // Free handle
    free(handle);

    // Delete timers
    if (timer)
    {
        esp_err_t err = esp_timer_delete(timer);
        if (err != ESP_OK)
        {
            return err;
        }
    }
    if (stop_timer)
    {
        esp_err_t err = esp_timer_delete(stop_timer);
        if (err != ESP_OK)
        {
            return err;
        }
    }

    // Reset pin
    gpio_reset_pin(pin); // Ignore error

    // Done
    ESP_LOGI(TAG, "deleted on pin %d", pin);
    return ESP_OK;
}

inline esp_err_t status_led_set_interval(status_led_handle_t handle, uint32_t interval_ms, bool initial_state)
{
    return status_led_set_interval_for(handle, interval_ms, initial_state, 0, initial_state);
}

inline esp_err_t status_led_set_state(status_led_handle_t handle, bool state)
{
    return status_led_set_interval_for(handle, 0, state, 0, state);
}

esp_err_t status_led_toggle_state(status_led_handle_t handle)
{
    return status_led_set_state(handle, !handle->status);
}

bool status_led_is_active(status_led_handle_t handle)
{
    return handle && handle->active;
}
