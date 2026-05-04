#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>
#include "our_led_sensor.h"

#define DT_DRV_COMPAT our_led_sensor

LOG_MODULE_REGISTER(led_sensor, LOG_LEVEL_INF);

struct led_sensor_config 
{
    struct gpio_dt_spec led;
};

struct led_sensor_data 
{
    bool led_state;
    uint8_t blink_count;
};


int led_sensor_set_blink_count(const struct device *dev, uint8_t count)
{
    if (count == 0) 
    {
        return -1;
    }

    struct led_sensor_data *data = dev->data;
    data->blink_count = count;
    LOG_INF("blink_count set to %u", count);
    return 0;
}


static int led_sensor_sample_fetch(const struct device *dev,
                                   enum sensor_channel chan)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;

    if ((chan != SENSOR_CHAN_ALL) && (chan != SENSOR_CHAN_LIGHT)) 
    {
        return -1;
    }

    uint8_t cycles = (data->blink_count > 0) ? data->blink_count : 1;

    for (uint8_t i = 0; i < cycles; i++) {
        gpio_pin_set_dt(&config->led, 1);
        data->led_state = true;
        LOG_INF("sample_fetch: LED ON  (blink %u/%u)", i + 1, cycles);
        k_msleep(200);

        gpio_pin_set_dt(&config->led, 0);
        data->led_state = false;
        LOG_INF("sample_fetch: LED OFF (blink %u/%u)", i + 1, cycles);
        k_msleep(200);
    }

    return 0;
}

static int led_sensor_channel_get(const struct device *dev,
                                  enum sensor_channel chan,
                                  struct sensor_value *val)
{
    struct led_sensor_data *data = dev->data;

    if (chan != SENSOR_CHAN_LIGHT) 
    {
        return -1;
    }

    val->val1 = data->led_state ? 1 : 0;
    val->val2 = data->blink_count;
    LOG_INF("channel_get: led_state=%d blink_count=%u",
            data->led_state, data->blink_count);
    return 0;
}

static int led_sensor_init(const struct device *dev)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;

    if (!gpio_is_ready_dt(&config->led)) {
        LOG_ERR("LED GPIO not ready");
        return -1;
    }

    gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);
    data->blink_count = 1; 
    LOG_INF("LED sensor initialized (default blink_count=1)");
    return 0;
}

static DEVICE_API(sensor, led_sensor_api) = {
    .sample_fetch = led_sensor_sample_fetch,
    .channel_get = led_sensor_channel_get,
};

#define LED_SENSOR_DEFINE(inst)                                      \
    static struct led_sensor_data led_sensor_data_##inst;            \
                                                                     \
    static const struct led_sensor_config led_sensor_config_##inst = {\
        .led = GPIO_DT_SPEC_INST_GET(inst, led_gpios),               \
    };                                                               \
                                                                     \
    DEVICE_DT_INST_DEFINE(inst,                                      \
                          led_sensor_init,                           \
                          NULL,                                      \
                          &led_sensor_data_##inst,                   \
                          &led_sensor_config_##inst,                 \
                          POST_KERNEL,                               \
                          80,                                        \
                          &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)