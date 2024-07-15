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

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(bme680_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(servo_stack, STACKSIZE);

#define PRIO_SERVO_TASK 1
#define PRIO_BME680_TASK 2
#define PERIOD_SERVO_TASK 1000
#define PERIOD_BME680_TASK 500

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

static void bme680_task(void *p1, void *p2, void *p3)
{
    printf("BME680 task started\n");
    while (1) {
        bme680.update_values(); 
        printf("Temperature: %d\n", bme680.temperature.val1);
        k_sleep(K_MSEC(PERIOD_BME680_TASK));
    }
}

static void servo_task(void *p1, void *p2, void *p3)
{
    printf("Servo task started\n");
    while (1) {
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
}

int main(void)
{
    printf("Starting application\n");

    bme680.init(); 
    printf("BME680 initialized\n");

    servo.init(&servo_dev, min_pulse, max_pulse); 
    printf("Servo initialized\n");

    struct k_thread servo_t;
    struct k_thread bme680_t;

    printf("Creating threads\n");
    k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
                    bme680_task, NULL, NULL, NULL,
                    PRIO_BME680_TASK, 1, K_NO_WAIT);

    k_thread_create(&servo_t, servo_stack, K_THREAD_STACK_SIZEOF(servo_stack),
                    servo_task, NULL, NULL, NULL,
                    PRIO_SERVO_TASK, 2, K_NO_WAIT);

    k_thread_start(&bme680_t);
    k_thread_start(&servo_t);

    printf("Threads started\n");
    return 0;
}
