/**
  ******************************************************************************
  * @file    main.c
  * @author  P. COURBIN
  * @version V2.0
  * @date    08-12-2023
  * @brief   WithGUI version
  ******************************************************************************
**/

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <stdio.h>
#include <string.h>
#include "display.hpp"
#include "bme680.hpp"
#include "servo.hpp"

#include <zephyr/drivers/pwm.h>
#include <sensor/hc_sr04/hc_sr04.h>

LOG_MODULE_REGISTER(app);

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(bme680_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(display_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(servo_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(hc_sr04_stack, STACKSIZE);

#define PRIO_SERVO_TASK 1
#define PRIO_BME680_TASK 2
#define PRIO_HC_SR04_TASK 3
#define PRIO_DISPLAY_TASK 4

#define PERIOD_SERVO_TASK 200
#define PERIOD_BME680_TASK 200
#define PERIOD_HC_SR04_TASK 200
#define PERIOD_DISPLAY_TASK 2000

static const struct pwm_dt_spec servo_dev = PWM_DT_SPEC_GET(DT_NODELABEL(servo));
static const uint32_t min_pulse = DT_PROP(DT_NODELABEL(servo), min_pulse);
static const uint32_t max_pulse = DT_PROP(DT_NODELABEL(servo), max_pulse);

const struct device *hc_sr04_dev;
myDisplay display;
myBME680 bme680;
myServo servo;

static volatile bool object_detected = false;    

enum direction {
    DOWN,
    UP,
};

static void display_task(void *p1, void *p2, void *p3)
{
	char name[10] = "DISPLAY";
	k_tid_t tid = k_current_get();
	int period = PERIOD_DISPLAY_TASK;

	char text[50] = {0};
	uint32_t start;

	struct k_timer timer;
	k_timer_init(&timer, NULL, NULL);
	k_timer_start(&timer, K_MSEC(0), K_MSEC(period));
	LOG_INF("Run task DISPLAY - Priority %d - Period %d\n", k_thread_priority_get(tid), period);
	while (1)
	{
		k_timer_status_sync(&timer);
		LOG_INF("START task %s", name);
		start = k_uptime_get_32();
		display.task_handler();
		display.chart_add_temperature(bme680.get_temperature());
		display.chart_add_humidity(bme680.get_humidity());

		sprintf(text, "%d.%02d - %d.%02d", bme680.temperature.val1, bme680.temperature.val2 / 10000, bme680.humidity.val1, bme680.humidity.val2 / 10000);
		display.text_add(text);
		LOG_INF("END task %s - %dms", name, k_uptime_get_32() - start);
	}
}

static void bme680_task(void *p1, void *p2, void *p3)
{
    char name[10] = "BME680";
    k_tid_t tid = k_current_get();
    int period = PERIOD_BME680_TASK;

    uint32_t start;

    struct k_timer timer;
    k_timer_init(&timer, NULL, NULL);
    k_timer_start(&timer, K_MSEC(0), K_MSEC(period));
    LOG_INF("Run task BME680 - Priority %d - Period %d\n", k_thread_priority_get(tid), period);
    while (1)
    {
        k_timer_status_sync(&timer);
        LOG_INF("START task %s", name);
        start = k_uptime_get_32();
        bme680.update_values();
        LOG_INF("END task %s - %dms", name, k_uptime_get_32() - start);       
    }
}

static void servo_task(void *p1, void *p2, void *p3)
{
    char name[20] = "SERVO";
    
    int period2 = PERIOD_SERVO_TASK;
    uint32_t start;
    struct k_timer timer;
    k_timer_init(&timer, NULL, NULL);
    k_timer_start(&timer, K_MSEC(0), K_MSEC(period2));

    while (1) {
        k_timer_status_sync(&timer);

        if (object_detected) {
            servo.setPosition(90);
            LOG_INF("Servo moving to 90 degrees in task %s", name);
        } else {
            servo.setPosition(0);
            LOG_INF("Servo moving to 0 degrees in task %s", name);
        }
    }
}

void hc_sr04_task(void *p1, void *p2, void *p3)
{
    static struct sensor_value distance;
    int ret;
    int period3 = PERIOD_HC_SR04_TASK;

    uint32_t start;

    struct k_timer timer;
    k_timer_init(&timer, NULL, NULL);
    k_timer_start(&timer, K_MSEC(0), K_MSEC(period3));

    //int nb = 10;

    while (1) {
        k_timer_status_sync(&timer);

        ret = sensor_sample_fetch(hc_sr04_dev);
        if (ret != 0) {
            LOG_ERR("Cannot take measurement: %d", ret);
        } else {
            sensor_channel_get(hc_sr04_dev, SENSOR_CHAN_DISTANCE, &distance);
        }

        LOG_INF("Distance: %d.%06d cm", distance.val1, distance.val2);
        if (distance.val1 < 20) {      ////// INFERIEUR A 20 CM
            object_detected = true;
        } else {
            object_detected = false;
        }
    }
}

int main(void)
{
    struct k_thread bme680_t;
    struct k_thread servo_t;
    struct k_thread hc_sr04_t;
	struct k_thread display_t;

	display.init(true);
    bme680.init();
    servo.init(&servo_dev, min_pulse, max_pulse); 
    hc_sr04_dev = DEVICE_DT_GET_ANY(zephyr_hc_sr04);
    __ASSERT(hc_sr04_dev != NULL, "Failed to get device binding");

    k_tid_t bme680 = k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
                    bme680_task, NULL, NULL, NULL,
                    PRIO_BME680_TASK, 0, K_NO_WAIT);
    k_thread_name_set(bme680, "bme680");

    k_tid_t servo = k_thread_create(&servo_t, servo_stack, K_THREAD_STACK_SIZEOF(servo_stack),
                    servo_task, NULL, NULL, NULL,
                    PRIO_SERVO_TASK, 0, K_NO_WAIT);
    k_thread_name_set(servo, "servo");

    k_tid_t hc_sr04 = k_thread_create(&hc_sr04_t, hc_sr04_stack, K_THREAD_STACK_SIZEOF(hc_sr04_stack),
                    hc_sr04_task, NULL, NULL, NULL,
                    PRIO_HC_SR04_TASK, 0, K_NO_WAIT);  
    k_thread_name_set(hc_sr04, "hc_sr04");

    k_tid_t display = k_thread_create(&display_t, display_stack, K_THREAD_STACK_SIZEOF(display_stack),
					display_task, NULL, NULL, NULL,
					PRIO_DISPLAY_TASK, 0, K_NO_WAIT);
    k_thread_name_set(display, "display");

    return 0;
}
