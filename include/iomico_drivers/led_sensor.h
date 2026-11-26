#ifndef IOMICO_DRIVERS_LED_SENSOR_H_
#define IOMICO_DRIVERS_LED_SENSOR_H_

#include <stdint.h>
#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Custom extension API, beyond the standard Sensor API.
 *
 * Overwrite the driver's internal fetch counter (a field in the
 * runtime data struct) with a new value.
 *
 * @param dev   LED sensor device.
 * @param value New value for the fetch counter.
 * @return 0 on success, -EINVAL if dev is not an LED sensor device.
 */
int led_sensor_set_fetch_count(const struct device *dev, uint32_t value);

/**
 * Read back the current fetch counter.
 *
 * @param dev   LED sensor device.
 * @param value Where to store the counter.
 * @return 0 on success, -EINVAL if dev is not an LED sensor device.
 */
int led_sensor_get_fetch_count(const struct device *dev, uint32_t *value);

#ifdef __cplusplus
}
#endif

#endif /* IOMICO_DRIVERS_LED_SENSOR_H_ */