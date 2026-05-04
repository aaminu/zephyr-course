#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS 1000

#define LED_NODE DT_ALIAS(led0)
#define LED_SENSOR_NODE DT_NODELABEL(led_sensor0)

// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
    printk("Hello World! %s\n", CONFIG_BOARD);

    const struct device *dev = DEVICE_DT_GET(LED_SENSOR_NODE);
    struct sensor_value val;

    if (!device_is_ready(dev)) {
        printk("LED sensor device not ready\n");
        return 0;
    }

    while (1) {
        sensor_sample_fetch(dev);                  // LED ON
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);

        sensor_channel_get(dev, SENSOR_CHAN_LIGHT, &val); // LED OFF
        k_msleep(CONFIG_APP_HEARTBEAT_PERIOD_MS);
    }
    return 0;
}
