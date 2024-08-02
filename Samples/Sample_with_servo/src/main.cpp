/**
 ******************************************************************************
 * @file    main.c
 * @author  P. COURBIN
 * @version V2.0
 * @date    08-12-2023
 * @brief   NoGUI version
 ******************************************************************************
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>

#include <stdio.h>
#include <string.h>
#include "bme680.hpp"
#include "servo.hpp"

#include <zephyr/drivers/pwm.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

static const struct pwm_dt_spec servo_dev = PWM_DT_SPEC_GET(DT_NODELABEL(servo));
static const uint32_t min_pulse = DT_PROP(DT_NODELABEL(servo), min_pulse);
static const uint32_t max_pulse = DT_PROP(DT_NODELABEL(servo), max_pulse);

myBME680 bme680;
myServo servo;

#define STEP PWM_USEC(100)

enum direction {
	DOWN,
	UP,
};

int main(void)
{
	char text[50] = {0};
	uint32_t pulse_width = min_pulse;
	enum direction dir = UP;
	int ret;

	bme680.init();
	servo.init(&servo_dev,min_pulse,max_pulse);


	while (1) {
		bme680.update_values();
		printf("%d\n",bme680.temperature.val1);

		/*ret = servo.setPulse(pulse_width);
		if (ret < 0) {
		
			return 0;
		}

		if (dir == DOWN) {
			if (pulse_width <= min_pulse) {
				dir = UP;
				pulse_width = min_pulse;
			} else {
				pulse_width -= STEP;
			}
		} else {
			pulse_width += STEP;

			if (pulse_width >= max_pulse) {
				dir = DOWN;
				pulse_width = max_pulse;
			}
		}*/
	servo.setPosition(-90);
		k_sleep(K_SECONDS(1));
	servo.setPosition(-45);
		k_sleep(K_SECONDS(1));
	servo.setPosition(0);
		k_sleep(K_SECONDS(1));
	servo.setPosition(45);
		k_sleep(K_SECONDS(1));
	servo.setPosition(90);
		k_sleep(K_SECONDS(1));
	}
	
	return 0;
}