#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/shell/shell.h>

/* Same device the application uses: first "iomico,led-sensor" node. */
static const struct device *led_dev = DEVICE_DT_GET_ANY(iomico_led_sensor);

/* sensor fetch -> sensor_sample_fetch() -> LED on */
static int cmd_sensor_fetch(const struct shell *sh, size_t argc, char **argv)
{
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	ret = sensor_sample_fetch(led_dev);
	if (ret < 0) {
		shell_error(sh, "sensor_sample_fetch() failed: %d", ret);
		return ret;
	}

	shell_print(sh, "sample_fetch OK: LED is ON");
	return 0;
}

/* sensor read -> sensor_channel_get() -> LED off, print the value */
static int cmd_sensor_read(const struct shell *sh, size_t argc, char **argv)
{
	struct sensor_value val;
	int ret;

	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	ret = sensor_channel_get(led_dev, SENSOR_CHAN_ALL, &val);
	if (ret < 0) {
		shell_error(sh, "sensor_channel_get() failed: %d", ret);
		return ret;
	}

	shell_print(sh, "channel_get OK: val1=%d val2=%d (LED is %s)",
		    val.val1, val.val2, val.val1 ? "ON" : "OFF");
	return 0;
}

/* sensor info -> device name and ready state */
static int cmd_sensor_info(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	shell_print(sh, "Device name : %s", led_dev->name);
	shell_print(sh, "Ready       : %s",
		    device_is_ready(led_dev) ? "yes" : "no");
	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(sub_sensor,
	SHELL_CMD(fetch, NULL, "Call sensor_sample_fetch() (turns LED on)",
		  cmd_sensor_fetch),
	SHELL_CMD(read, NULL, "Call sensor_channel_get() and print result (turns LED off)",
		  cmd_sensor_read),
	SHELL_CMD(info, NULL, "Print device name and ready state",
		  cmd_sensor_info),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(sensor, &sub_sensor, "LED sensor driver commands", NULL);