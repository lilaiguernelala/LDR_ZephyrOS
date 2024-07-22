/**
  ******************************************************************************
  * @file    main.c
  * @author  P. COURBIN
  * @version V2.0
  * @date    08-12-2023
  * @brief   WithGUI version
  ******************************************************************************
*/

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

#include "bme680.hpp"

#define STACKSIZE (4096)
static K_THREAD_STACK_DEFINE(bme680_stack, STACKSIZE);

#define PRIO_BME680_TASK 2
#define PERIOD_BME680_TASK 500

myBME680 bme680;

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

int main(void)
{
	struct k_thread bme680_t;

	bme680.init();

	k_thread_create(&bme680_t, bme680_stack, K_THREAD_STACK_SIZEOF(bme680_stack),
					bme680_task, NULL, NULL, NULL,
					PRIO_BME680_TASK, 0, K_NO_WAIT);
}