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

#include "bme680.hpp"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

myBME680 bme680;

int main(void)
{
	char text[50] = {0};

	bme680.init();

	while (1)
	{
		bme680.update_values();

		sprintf(text, "T:%d.%02d°C \t H:%d.%02d\%",
				bme680.temperature.val1, bme680.temperature.val2 / 10000,
				bme680.humidity.val1, bme680.humidity.val2 / 10000);
		printf("%s\n", text);
		k_msleep(200);
	}
}