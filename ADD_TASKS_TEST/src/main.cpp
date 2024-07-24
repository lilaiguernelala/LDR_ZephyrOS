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

#include "servo.hpp"
#include "bme680.hpp"
#include "hc_sr04.hpp"
#include "display.hpp"

LOG_MODULE_REGISTER(app);

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(servo_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(bme680_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(hc_sr04_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(display_stack, STACKSIZE);

#define PRIO_SERVO_TASK 4
#define PRIO_BME680_TASK 2
#define PRIO_HC_SR04_TASK 3
#define PRIO_DISPLAY_TASK 1

#define PERIOD_SERVO_TASK 200
#define PERIOD_BME680_TASK 200
#define PERIOD_HC_SR04_TASK 200
#define PERIOD_DISPLAY_TASK 2000

static const struct pwm_dt_spec servo_dev = PWM_DT_SPEC_GET(DT_NODELABEL(servo));
static const uint32_t min_pulse = DT_PROP(DT_NODELABEL(servo), min_pulse);
static const uint32_t max_pulse = DT_PROP(DT_NODELABEL(servo), max_pulse);

const struct device *bme680_dev = DEVICE_DT_GET(DT_CHOSEN(perso_bme680)); // OR DT_ALIAS(bme680)
const struct device *hc_sr04_dev = DEVICE_DT_GET_ANY(zephyr_hc_sr04);
const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

myServo servo;
myBME680 bme680;
myHC_SR04 hc_sr04;
myDisplay display;

#define TASK_PREPARE(given_period)                    \
    k_tid_t tid = k_current_get();                    \
    const char *name = k_thread_name_get(tid);        \
    int period = given_period;                        \
    uint32_t start;                                   \
    struct k_timer timer;                             \
    k_timer_init(&timer, NULL, NULL);                 \
    k_timer_start(&timer, K_MSEC(0), K_MSEC(period)); \
    LOG_DBG("Run task %s - Priority %d - Period %d\n", name, k_thread_priority_get(tid), period);

#define TASK_START()                \
    k_timer_status_sync(&timer);    \
    LOG_DBG("START task %s", name); \
    start = k_uptime_get_32();

#define TASK_END() \
    LOG_DBG("END task %s - %dms", name, k_uptime_get_32() - start);

static void display_task(void *p1, void *p2, void *p3)
{
    TASK_PREPARE(PERIOD_DISPLAY_TASK);
    char text[50] = {0};

    while (1)
    {
        TASK_START()

        display.task_handler();
        display.chart_add_temperature(bme680.get_temperature());
        display.chart_add_humidity(bme680.get_humidity());

        sprintf(text, "%d.%02d - %d.%02d - %d.%2d - %d", bme680.temperature.val1, bme680.temperature.val2 / 10000, bme680.humidity.val1, bme680.humidity.val2 / 10000, hc_sr04.distance.val1, hc_sr04.distance.val2, servo.getPosition());
        display.text_add(text);

        TASK_END()
    }
}

static void bme680_task(void *p1, void *p2, void *p3)
{
    TASK_PREPARE(PERIOD_BME680_TASK);

    while (1)
    {
        TASK_START()

        bme680.update_values();

        TASK_END()
    }
}

static void servo_task(void *p1, void *p2, void *p3)
{
    TASK_PREPARE(PERIOD_SERVO_TASK);

    while (1)
    {
        TASK_START()

        if (hc_sr04.get_distance() < 20)
        { ////// INFERIEUR A 20 CM
            servo.setPosition(90);
            LOG_INF("Servo moving to 90 degrees in task %s", name);
        }
        else
        {
            servo.setPosition(0);
            LOG_INF("Servo moving to 0 degrees in task %s", name);
        }

        TASK_END()
    }
}

static void hc_sr04_task(void *p1, void *p2, void *p3)
{
    TASK_PREPARE(PERIOD_HC_SR04_TASK);

    while (1)
    {
        TASK_START()

        hc_sr04.update_values();
        LOG_INF("Distance: %d.%06d cm", hc_sr04.distance.val1, hc_sr04.distance.val2);

        TASK_END()
    }
}

int main(void)
{
    struct k_thread servo_t;
    k_tid_t servo_tid;
    struct k_thread bme680_t;
    k_tid_t bme680_tid;
    struct k_thread hc_sr04_t;
    k_tid_t hc_sr04_tid;
    struct k_thread display_t;
    k_tid_t display_tid;

    servo.init(&servo_dev, min_pulse, max_pulse);
    bme680.init(bme680_dev);
    hc_sr04.init(hc_sr04_dev);
    display.init(display_dev, true);

    servo_tid = k_thread_create(&servo_t, servo_stack, K_THREAD_STACK_SIZEOF(servo_stack),
                                servo_task, NULL, NULL, NULL,
                                PRIO_SERVO_TASK, 0, K_NO_WAIT);
    k_thread_name_set(servo_tid, "servo");

    bme680_tid = k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
                                 bme680_task, NULL, NULL, NULL,
                                 PRIO_BME680_TASK, 0, K_NO_WAIT);
    k_thread_name_set(bme680_tid, "bme680");

    hc_sr04_tid = k_thread_create(&hc_sr04_t, hc_sr04_stack, K_THREAD_STACK_SIZEOF(hc_sr04_stack),
                                  hc_sr04_task, NULL, NULL, NULL,
                                  PRIO_HC_SR04_TASK, 0, K_NO_WAIT);
    k_thread_name_set(hc_sr04_tid, "hc_sr04");

    display_tid = k_thread_create(&display_t, display_stack, K_THREAD_STACK_SIZEOF(display_stack),
                                  display_task, NULL, NULL, NULL,
                                  PRIO_DISPLAY_TASK, 0, K_NO_WAIT);
    k_thread_name_set(display_tid, "display");

    return 0;
}
