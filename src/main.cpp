#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
//LOG_MODULE_REGISTER(app);

#include "display.hpp"
#include "bme680.hpp"
#include "hcsr04.hpp"

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(display_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(bme680_stack, STACKSIZE);
static K_THREAD_STACK_DEFINE(hcsr04_stack, STACKSIZE);

#define PRIO_DISPLAY_TASK 1
#define PRIO_BME680_TASK 2
#define PRIO_HCSRC4_TASK 3
#define PERIOD_DISPLAY_TASK 1000
#define PERIOD_BME680_TASK 500
#define PERIOD_HCSR04_TASK 500

myDisplay display;
myBME680 bme680;
myHCSRC4 hcsr04;

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

        sprintf(text, "%d.%02d°C - %d.%02d%% - %.2f cm", bme680.temperature.val1, bme680.temperature.val2 / 10000, bme680.humidity.val1, bme680.humidity.val2 / 10000, hcsr04.get_distance());
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

static void hcsr04_task(void *p1, void *p2, void *p3)
{
    char name[10] = "SRC4";
    k_tid_t tid = k_current_get();
    int period = PERIOD_HCSR04_TASK;

    uint32_t start;

    struct k_timer timer;
    k_timer_init(&timer, NULL, NULL);
    k_timer_start(&timer, K_MSEC(0), K_MSEC(period));
    LOG_INF("Run task HCSR04 - Priority %d - Period %d\n", k_thread_priority_get(tid), period);
    while (1)
    {
        k_timer_status_sync(&timer);
        LOG_INF("START task %s", name);
        start = k_uptime_get_32();
        double distance = src4.get_distance();
        LOG_INF("Distance: %.2f cm", distance);
        LOG_INF("END task %s - %dms", name, k_uptime_get_32() - start);
    }
}

int main(void)
{
    struct k_thread display_t;
    struct k_thread bme680_t;
    struct k_thread hcsr04_t;

    display.init(true);
    bme680.init();
    src4.init(TRIG_PIN, ECHO_PIN);

    k_thread_create(&display_t, display_stack, K_THREAD_STACK_SIZEOF(display_stack),
                    display_task, NULL, NULL, NULL,
                    PRIO_DISPLAY_TASK, 0, K_NO_WAIT);

    k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
                    bme680_task, NULL, NULL, NULL,
                    PRIO_BME680_TASK, 0, K_NO_WAIT);

    k_thread_create(&hcsr04_t, src4_stack, K_THREAD_STACK_SIZEOF(hcsr04_stack),
                    hcsr04_task, NULL, NULL, NULL,
                    PRIO_SRC4_TASK, 0, K_NO_WAIT);
}
