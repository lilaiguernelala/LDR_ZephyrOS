#ifndef SERVO_HPP
#define SERVO_HPP

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>

static const struct pwm_dt_spec servo = PWM_DT_SPEC_GET(DT_NODELABEL(servo));
static const uint32_t min_pulse = DT_PROP(DT_NODELABEL(servo), min_pulse);
static const uint32_t max_pulse = DT_PROP(DT_NODELABEL(servo), max_pulse);  

#define STEP PWM_USEC(100)

enum direction {
    DOWN,
    UP,
};

void init_servo();
//void control_servo();

#endif 

