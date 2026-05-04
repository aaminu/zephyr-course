#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>

#define DT_DRV_COMPAT our_led_sensor

LOG_MODULE_REGISTER(led_sensor, LOG_LEVEL_INF);


struct led_sensor_config 
{
    struct gpio_dt_spec led;
};

struct led_sensor_data 
{
    bool led_state;
};

static int led_sensor_sample_fetch(const struct device *dev,
                                   enum sensor_channel chan)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;

    if ((chan != SENSOR_CHAN_ALL) && (chan != SENSOR_CHAN_LIGHT)) 
    {
        return -1;
    }

    gpio_pin_set_dt(&config->led, 1);
    data->led_state = true;

    LOG_INF("sample_fetch: LED ON");

    return 0;
}

static int led_sensor_channel_get(const struct device *dev,
                                  enum sensor_channel chan,
                                  struct sensor_value *val)
{
    const struct led_sensor_config *config = dev->config;
    struct led_sensor_data *data = dev->data;

    if (chan != SENSOR_CHAN_LIGHT) {
        return -1;
    }

    gpio_pin_set_dt(&config->led, 0);
    data->led_state = false;

    val->val1 = 0;
    val->val2 = 0;

    LOG_INF("channel_get: LED OFF");

    return 0;
}


static int led_sensor_init(const struct device *dev)
{
    const struct led_sensor_config *config = dev->config;

    if (!gpio_is_ready_dt(&config->led)) {
        LOG_ERR("LED GPIO not ready");
        return -1;
    }

    gpio_pin_configure_dt(&config->led, GPIO_OUTPUT_INACTIVE);

    LOG_INF("LED sensor initialized");

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