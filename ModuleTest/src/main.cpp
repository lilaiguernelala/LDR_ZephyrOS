/**
  ******************************************************************************
  * @file    main.c
  * @author  P. COURBIN
  * @version V2.0
  * @date    08-12-2023
  * @brief   Sample version
  ******************************************************************************
*/

#include <zephyr/kernel.h>
#include <stdio.h>
#include <string.h>

#include <sensor/hc_sr04/hc_sr04.h>

#include "bme680.hpp"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

const struct device *hc_sr04_dev;
myBME680 bme680;

void measure(void)
{
	static struct sensor_value distance;
	int ret;

	ret = sensor_sample_fetch(hc_sr04_dev);
	if (ret != 0) {
		LOG_ERR("Cannot take measurement: %d", ret);
	} else {
		sensor_channel_get(hc_sr04_dev, SENSOR_CHAN_DISTANCE, &distance);
		LOG_INF("Distance: %d.%06d cm", distance.val1, distance.val2);
	}
}

int main(void)
{
	char text[50] = {0};

	bme680.init();

	hc_sr04_dev = DEVICE_DT_GET_ANY(zephyr_hc_sr04);
	__ASSERT(hc_sr04_dev == NULL, "Failed to get device binding");

	while (1)
	{
		bme680.update_values();

		sprintf(text, "T:%d.%02d°C \t H:%d.%02d\%",
				bme680.temperature.val1, bme680.temperature.val2 / 10000,
				bme680.humidity.val1, bme680.humidity.val2 / 10000);
		printf("%s\n", text);

		measure();
		k_msleep(200);
	}
}