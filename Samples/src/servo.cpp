#include "servo.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(servo, LOG_LEVEL_DBG);

void init_servo() {
    if (!pwm_is_ready_dt(&servo)) {
        LOG_ERR("PWM device is not ready");
    }
}

void update_servo(uint32_t &pulse_width, enum direction &dir) {
    int ret = pwm_set_pulse_dt(&servo, pulse_width);
    if (ret < 0) {
        LOG_ERR("Failed to set pulse width for servo");
        return;
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
}
