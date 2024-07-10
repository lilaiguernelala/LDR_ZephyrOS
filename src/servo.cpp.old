#include "servo.hpp"
#include <zephyr/logging/log.h>


void init_servo() {
   printk("Servomotor control\n");

	if (!pwm_is_ready_dt(&servo)) {
		printk("Error: PWM device %s is not ready\n", servo.dev->name);
		return 0;
	}
}

void control_servo() {
    uint32_t pulse_width = min_pulse;
    enum direction dir = UP;
    int ret;

    while (1) {
        ret = pwm_set_pulse_dt(&servo, pulse_width);
        if (ret < 0) {
            printk("Error %d: failed to set pulse width\n", ret);
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
}

/*
  int main(void) {
    init_servo();
    control_servo();
    return 0;
} 
*/
