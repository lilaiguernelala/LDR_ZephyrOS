/**
  ******************************************************************************
  * @file    main.cpp
  * @author  P. COURBIN
  * @version V2.0
  * @date    08-12-2023
  * @brief   Sample version
  ******************************************************************************
*/

#include <zephyr/kernel.h>
#include <stdio.h>
#include <string.h>

#include "display.hpp"
#include "bme680.hpp"
#include "hcsr04.hpp"
#include "servo.hpp"

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

myDisplay display;
myBME680 bme680;
myUltrasonicSensor ultrasonic;
//myServo servo;

int main(void)
{
    char text[50] = {0};

    uint32_t pulse_width = min_pulse;
    enum direction dir = UP;
    int ret;

    if (!pwm_is_ready_dt(&servo)) {
		
		return 0;
	}

    while (1) {
		ret = pwm_set_pulse_dt(&servo, pulse_width);
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
		}

		k_sleep(K_SECONDS(1));
	}

    display.init(true);
    bme680.init();
    ultrasonic.init();
   // servo.init();
    //init_servo();  

    lv_task_handler();
    display_blanking_off(display.dev);

    while (1)
    {
        bme680.update_values();
        ultrasonic.update_distance();
      //  servo.update_angle(pulse_width);    //////

        display.task_handler();
        display.chart_add_temperature(bme680.get_temperature());
        display.chart_add_humidity(bme680.get_humidity());

        sprintf(text, "T:%d.%02d°C \t H:%d.%02d\%\t A:%dcm",
                bme680.temperature.val1, bme680.temperature.val2 / 10000,
                bme680.humidity.val1, bme680.humidity.val2 / 10000, ultrasonic.get_distance().val1);
        display.text_add(text);
        LOG_INF("%s\n", text);

        k_msleep(lv_task_handler());

        ret = pwm_set_pulse_dt(&servo, pulse_width);
        if (ret < 0) {
            LOG_ERR("Failed to set pulse width for servo");
            return -1;
        }
        if (pulse_width == min_pulse) pulse_width = max_pulse;
        else pulse_width = min_pulse;

        printf("%d\n",pulse_width);


        k_msleep(4000);
    

        // Control servo
        /*
      ret = pwm_set_pulse_dt(&servo, pulse_width);
        if (ret < 0) {
            LOG_ERR("Failed to set pulse width for servo");
            return -1;
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
    }
    return 0;
}
