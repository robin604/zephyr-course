#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#define SLEEP_TIME_MS CONFIG_APP_HEARTBEAT_PERIOD_MS

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* First devicetree node with compatible = "iomico,led-sensor" */
static const struct device *led_dev = DEVICE_DT_GET_ANY(iomico_led_sensor);

int main(void)
{
    struct sensor_value val;

    if (!device_is_ready(led_dev)) {
        LOG_ERR("LED sensor device not ready");
        return 0;
    }

    LOG_INF("Using LED sensor driver: %s", led_dev->name);

    while (1) {
        /* sample_fetch -> LED on */
        if (sensor_sample_fetch(led_dev) < 0) return 0;
        k_msleep(SLEEP_TIME_MS);

        /* channel_get -> LED off */
        if (sensor_channel_get(led_dev, SENSOR_CHAN_ALL, &val) < 0) return 0;
        LOG_INF("LED state reported by driver: %d", val.val1);
        k_msleep(SLEEP_TIME_MS);
    }
    return 0;
}