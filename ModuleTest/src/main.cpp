/*
 * Copyright (c) 2024 Pierre COURBIN
 * 
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#include <string.h>

#include <sensor/hc_sr04/hc_sr04.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

struct sensor_value measure(const struct device *dev)
{
	struct sensor_value distance = {-1, 0};
	int ret;

	ret = sensor_sample_fetch(dev);
	if (ret != 0)
	{
		LOG_ERR("%s - Cannot take measurement: %d", dev->name, ret);
	}
	else
	{
		sensor_channel_get(dev, SENSOR_CHAN_DISTANCE, &distance);
		LOG_INF("%s - Distance: %d.%06d cm", dev->name, distance.val1, distance.val2);
	}
	return distance;
}

int main(void)
{
	char text[50] = {0};

	const struct device *hc_sr04_dev0 = DEVICE_DT_GET(DT_NODELABEL(us0));
	__ASSERT(hc_sr04_dev0 == NULL, "Failed to get device binding");
	struct sensor_value distance0 = {0, 0};

	const struct device *hc_sr04_dev1 = DEVICE_DT_GET(DT_NODELABEL(us1));
	__ASSERT(hc_sr04_dev1 == NULL, "Failed to get device binding");
	struct sensor_value distance1 = {0, 0};

	while (1)
	{
		distance0 = measure(hc_sr04_dev0);
		distance1 = measure(hc_sr04_dev1);

		sprintf(text, "US0:%d.%02dcm \t US1:%d.%02dcm",
				distance0.val1, distance0.val2,
				distance1.val1, distance1.val2);
		printf("%s\n", text);
		k_msleep(1000);
	}
}