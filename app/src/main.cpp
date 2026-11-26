#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <iomico_drivers/led_sensor.h>

#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS

/* After this many fetches, use the custom API to reset the counter. */
#define FETCHES_BEFORE_RESET 5

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* First devicetree node with compatible = "iomico,led-sensor" */
static const struct device *led_dev = DEVICE_DT_GET_ANY(iomico_led_sensor);

int main(void)
{
    struct sensor_value val;
    uint32_t count;

    if (!device_is_ready(led_dev)) {
        LOG_ERR("LED sensor device not ready");
        return 0;
    }

    LOG_INF("Using LED sensor driver: %s", led_dev->name);

    while (1) {
        /* Standard Sensor API: sample_fetch -> LED on */
        if (sensor_sample_fetch(led_dev) < 0) return 0;
        k_msleep(SLEEP_TIME_MS);

        /* Standard Sensor API: channel_get -> LED off */
        if (sensor_channel_get(led_dev, SENSOR_CHAN_ALL, &val) < 0) return 0;
        k_msleep(SLEEP_TIME_MS);

        /* Custom extension API: read the counter kept in the driver's data struct */
        if (led_sensor_get_fetch_count(led_dev, &count) < 0) return 0;
        LOG_INF("Driver has counted %u fetches", count);

        /* Custom extension API: change that counter from the application */
        if (count >= FETCHES_BEFORE_RESET) {
            LOG_INF("Resetting fetch counter via custom API");
            if (led_sensor_set_fetch_count(led_dev, 0) < 0) return 0;
        }
    }
    return 0;
}