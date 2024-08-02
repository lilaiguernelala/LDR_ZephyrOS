#include "hcsr04.hpp"
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(app);

#define TRIGGER_NODE DT_ALIAS(ultrasonictrigger)
#define ECHO_NODE DT_ALIAS(ultrasonicecho)

static const struct gpio_dt_spec trigger = GPIO_DT_SPEC_GET(TRIGGER_NODE, gpios);
static const struct gpio_dt_spec echo = GPIO_DT_SPEC_GET_OR(ECHO_NODE, gpios, {0});
static struct gpio_callback echo_cb_data;

#define T_MAX_WAIT_MS 130

uint32_t start_time;
uint32_t end_time;
struct k_sem read_sem;
enum mesure_states { WAIT_RISING, WAIT_FALLING, END_MESURE };
enum mesure_states state;
void echo_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    LOG_INF("Echo changed at %d" PRIu32 "", k_cycle_get_32());
    if ((pins & (1 << echo.pin)) != 0)
    {
        switch(state) {
            case WAIT_RISING:
                state = WAIT_FALLING;
                start_time = k_cycle_get_32();
                break;
            case WAIT_FALLING:
                state = END_MESURE;
                end_time = k_cycle_get_32();
                (void) gpio_remove_callback(echo.port, &echo_cb_data);
                k_sem_give(&read_sem);
                break;
            default:
                break;
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
        returned = -5;
    }

    if (!returned)
    {
        gpio_init_callback(&echo_cb_data, echo_callback, BIT(echo.pin));
        //gpio_add_callback(echo.port, &echo_cb_data);
    }

    LOG_INF("Set up echo at %s pin %d", echo.port->name, echo.pin);

    int err = k_sem_init(&read_sem, 0, 1);
    if (0 != err) {
        returned = -6;
    }

    return returned;
}


void myUltrasonicSensor::update_distance()
{
    int err;
    uint32_t duration;
    state = WAIT_RISING;
    gpio_add_callback(echo.port, &echo_cb_data);

    gpio_pin_set_dt(&trigger, 1);
    k_busy_wait(11);
    gpio_pin_set_dt(&trigger, 0);

    if (0 != k_sem_take(&read_sem, K_MSEC(T_MAX_WAIT_MS))) {
        LOG_ERR("UltraSonic Timeout");
        end_time = -1;
        (void) gpio_remove_callback(echo.port, &echo_cb_data);
    }

    if (end_time == -1)
    {
        distance_d = -1;
    } else {
        duration = k_cyc_to_us_near32(end_time - start_time);
        //distance_d = (duration * 34000 / 2) / 100000;
        distance_d = duration / 58;
    }

    sensor_value_from_double(&distance, distance_d);
}

struct sensor_value myUltrasonicSensor::get_distance()
{
    return distance;
}

      /* trig-gpios = <&gpio0 4 GPIO_ACTIVE_HIGH>;    
        echo-gpios = <&gpio0 5 GPIO_ACTIVE_HIGH>;*/
