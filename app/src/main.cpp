#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/* First devicetree node with compatible = "iomico,led-sensor" */
static const struct device *led_dev = DEVICE_DT_GET_ANY(iomico_led_sensor);

int main(void)
{
    if (!device_is_ready(led_dev)) {
        LOG_ERR("LED sensor device not ready");
        return 0;
    }

    LOG_INF("LED sensor driver ready: %s", led_dev->name);
    LOG_INF("Type 'sensor -h' in the shell to control it");

    /* Nothing else to do here: the shell runs in its own thread and
     * the 'sensor' commands in sensor_shell.c drive the LED. */
    return 0;
}