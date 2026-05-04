#ifndef OUR_LED_SENSOR_H
#define OUR_LED_SENSOR_H

#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

int led_sensor_set_blink_count(const struct device *dev, uint8_t count);

#ifdef __cplusplus
}
#endif

#endif /* OUR_LED_SENSOR_H */