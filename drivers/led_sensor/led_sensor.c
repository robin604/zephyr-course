#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <iomico_drivers/led_sensor.h>

#define DT_DRV_COMPAT iomico_led_sensor

LOG_MODULE_REGISTER(led_sensor, CONFIG_SENSOR_LOG_LEVEL);

/* Read-only settings, filled from the devicetree node at build time. */
struct led_sensor_config {
	struct gpio_dt_spec led;
};

/* Runtime state, lives in RAM and changes while the program runs. */
struct led_sensor_data {
	bool led_on;
	uint32_t fetch_count;
};

/* Sensor API: sample_fetch turns the LED on. */
static int led_sensor_sample_fetch(const struct device *dev,
				   enum sensor_channel chan)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;
	int ret;

	ARG_UNUSED(chan);

	ret = gpio_pin_set_dt(&cfg->led, 1);
	if (ret < 0) {
		return ret;
	}

	data->led_on = true;
	data->fetch_count++;
	LOG_INF("LED on (fetch #%u)", data->fetch_count);

	return 0;
}

/* Sensor API: channel_get turns the LED off and reports its state. */
static int led_sensor_channel_get(const struct device *dev,
				  enum sensor_channel chan,
				  struct sensor_value *val)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;
	int ret;

	ARG_UNUSED(chan);

	ret = gpio_pin_set_dt(&cfg->led, 0);
	if (ret < 0) {
		return ret;
	}

	data->led_on = false;
	val->val1 = data->led_on ? 1 : 0;
	val->val2 = 0;
	LOG_INF("LED off");

	return 0;
}

static int led_sensor_init(const struct device *dev)
{
	const struct led_sensor_config *cfg = dev->config;
	struct led_sensor_data *data = dev->data;
	int ret;

	if (!gpio_is_ready_dt(&cfg->led)) {
		LOG_ERR("LED GPIO not ready");
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&cfg->led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return ret;
	}

	data->led_on = false;
	data->fetch_count = 0;

	return 0;
}

static DEVICE_API(sensor, led_sensor_api) = {
	.sample_fetch = led_sensor_sample_fetch,
	.channel_get = led_sensor_channel_get,
};

/*
 * Custom extension API (not part of the Sensor API).
 *
 * Any struct device can be passed in, so first make sure it really is
 * one of ours by comparing its api pointer with our own API table.
 */
static bool led_sensor_is_ours(const struct device *dev)
{
	return dev != NULL && dev->api == &led_sensor_api;
}

int led_sensor_set_fetch_count(const struct device *dev, uint32_t value)
{
	struct led_sensor_data *data;

	if (!led_sensor_is_ours(dev)) {
		return -EINVAL;
	}

	data = dev->data;
	LOG_INF("fetch_count changed from %u to %u", data->fetch_count, value);
	data->fetch_count = value;

	return 0;
}

int led_sensor_get_fetch_count(const struct device *dev, uint32_t *value)
{
	const struct led_sensor_data *data;

	if (!led_sensor_is_ours(dev) || value == NULL) {
		return -EINVAL;
	}

	data = dev->data;
	*value = data->fetch_count;

	return 0;
}

#define LED_SENSOR_DEFINE(inst)						\
	static struct led_sensor_data led_sensor_data_##inst;		\
	static const struct led_sensor_config led_sensor_config_##inst = { \
		.led = GPIO_DT_SPEC_INST_GET(inst, led_gpios),		\
	};								\
	DEVICE_DT_INST_DEFINE(inst,					\
			      led_sensor_init, NULL,			\
			      &led_sensor_data_##inst,			\
			      &led_sensor_config_##inst,		\
			      POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,	\
			      &led_sensor_api);

DT_INST_FOREACH_STATUS_OKAY(LED_SENSOR_DEFINE)