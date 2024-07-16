#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <string.h>
#include "bme680.hpp"
#include "servo.hpp"
#include <zephyr/drivers/pwm.h>
#include <sensor/hc_sr04/hc_sr04.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(bme680_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(servo_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(hc_sr04_stack, STACKSIZE);

#define PRIO_SERVO_TASK 1
#define PRIO_BME680_TASK 2
#define PRIO_HC_SR04_TASK 3

#define PERIOD_SERVO_TASK 1000
#define PERIOD_BME680_TASK 500
#define PERIOD_HC_SR04_TASK 1500

static const struct pwm_dt_spec servo_dev = PWM_DT_SPEC_GET(DT_NODELABEL(servo));
static const uint32_t min_pulse = DT_PROP(DT_NODELABEL(servo), min_pulse);
static const uint32_t max_pulse = DT_PROP(DT_NODELABEL(servo), max_pulse);

const struct device *hc_sr04_dev;

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


void hc_sr04_task(void *p1, void *p2, void *p3)
{
    static struct sensor_value distance;
    int ret;

    while (1) {
        ret = sensor_sample_fetch(hc_sr04_dev);
        if (ret != 0) {
            LOG_ERR("Cannot take measurement: %d", ret);
        } else {
            sensor_channel_get(hc_sr04_dev, SENSOR_CHAN_DISTANCE, &distance);
            LOG_INF("Distance: %d.%06d cm", distance.val1, distance.val2);
        }
        k_sleep(K_MSEC(PERIOD_HC_SR04_TASK));
    }
}


int main(void)
{
    bme680.init(); 
    servo.init(&servo_dev, min_pulse, max_pulse); 

    hc_sr04_dev = DEVICE_DT_GET_ANY(zephyr_hc_sr04);
    __ASSERT(hc_sr04_dev == NULL, "Failed to get device binding");

    struct k_thread servo_t;
    struct k_thread bme680_t;
    struct k_thread hc_sr04_t;

    k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
                    bme680_task, NULL, NULL, NULL,
                    PRIO_BME680_TASK, 1, K_NO_WAIT);

    k_thread_create(&servo_t, servo_stack, K_THREAD_STACK_SIZEOF(servo_stack),
                    servo_task, NULL, NULL, NULL,
                    PRIO_SERVO_TASK, 2, K_NO_WAIT);

    k_thread_create(&hc_sr04_t, hc_sr04_stack, K_THREAD_STACK_SIZEOF(hc_sr04_stack),
                    hc_sr04_task, NULL, NULL, NULL,
                    PRIO_HC_SR04_TASK, 3, K_NO_WAIT);  

    k_thread_start(&bme680_t);
    k_thread_start(&servo_t);
    k_thread_start(&hc_sr04_t);

    return 0;
}
