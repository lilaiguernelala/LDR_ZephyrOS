#include "hcsr04.hpp"
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

#define TRIGGER_NODE DT_ALIAS(ultrasonictrigger)
#define ECHO_NODE DT_ALIAS(ultrasonicecho)

static const struct gpio_dt_spec trigger = GPIO_DT_SPEC_GET(TRIGGER_NODE, gpios);
static const struct gpio_dt_spec echo = GPIO_DT_SPEC_GET_OR(ECHO_NODE, gpios, {0});
static struct gpio_callback echo_cb_data;

uint32_t start_time;
uint32_t end_time;
bool ready = false;

void echo_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("Echo changed at %d" PRIu32 "\n", k_cycle_get_32());
    if ((pins & (1 << echo.pin)) != 0)
    {
        if (gpio_pin_get(echo.port, echo.pin) == 1)
        {
            LOG_INF("Echo high at %d" PRIu32 "\n", k_cycle_get_32());
            start_time = k_cycle_get_32();

        }
        else
        {
            LOG_INF("Echo low at %d" PRIu32 "\n", k_cycle_get_32());
            end_time = k_cycle_get_32();
            (void) gpio_remove_callback(echo.port, &echo_cb_data);
            ready = true;
        }
    }
}

int myUltrasonicSensor::init()
{
    uint8_t returned = 0;
    // Configure Trigger as output
    if (!device_is_ready(trigger.port))
    {
        LOG_ERR("Error: Trigger devices are not ready (%s)", trigger.port->name);
        returned = -1;
    }

    if (!returned && gpio_pin_configure_dt(&trigger, GPIO_OUTPUT_ACTIVE) < 0)
    {
        LOG_ERR("Error: LEDs config failed (%s).", trigger.port->name);
        returned = -2;
    }

    // Configure Echo as input
    if (!returned && !device_is_ready(echo.port))
    {
        LOG_ERR("Error: Echo devices are not ready (%s)", echo.port->name);
        returned = -3;
    }

    if (!returned && gpio_pin_configure_dt(&echo, GPIO_INPUT) < 0)
    {
        LOG_ERR("Error: Echo config failed (%s).", echo.port->name);
        returned = -4;
    }

    if (!returned && gpio_pin_interrupt_configure_dt(&echo, GPIO_INT_EDGE_BOTH) != 0)
    {
        LOG_ERR("Error: failed to configure interrupt on %s pin %d.", echo.port->name, echo.pin);
        returned = -3;
    }

    if (!returned)
    {
        gpio_init_callback(&echo_cb_data, echo_callback, BIT(echo.pin));
        //gpio_add_callback(echo.port, &echo_cb_data);
    }

    LOG_INF("Set up echo at %s pin %d", echo.port->name, echo.pin);
    return returned;
}


int myUltrasonicSensor::update_distance()
{
    int err;
    uint32_t duration;
    uint32_t start_mesure = k_cycle_get_32();
    err = gpio_add_callback(echo.port, &echo_cb_data);
    if (0 != err) {
        LOG_DBG("Failed to add HC-SR04 echo callback");
        //(void) k_mutex_unlock(&m_shared_resources.mutex);
        return -1;
    }

    ready = false;
    gpio_pin_set_dt(&trigger, 1);
    k_busy_wait(11);
    gpio_pin_set_dt(&trigger, 0);

    while(!ready)
    {
        k_busy_wait(1);
        if (k_cyc_to_us_near32(k_cycle_get_32() - start_mesure) > 1000000)
        {
            LOG_ERR("Timeout");
            end_time = -1;
            ready = true;
        }
    }

    if (end_time == -1)
    {
        distance = -1;
    } else {
        duration = k_cyc_to_us_near32(end_time - start_time);
        distance = (duration * 340 / 2);
    }

    return distance;
}

double myUltrasonicSensor::get_distance()
{
    return distance;
}

      /* trig-gpios = <&gpio0 4 GPIO_ACTIVE_HIGH>;    
        echo-gpios = <&gpio0 5 GPIO_ACTIVE_HIGH>;*/