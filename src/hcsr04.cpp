#include "ultrasonic_sensor.hpp"
#include <zephyr/logging/log.h>

void myUltrasonicSensor::init()
{
    dev = device_get_binding(DT_LABEL(ULTRASONIC_DEV));
    if (!dev)
    {
        LOG_ERR("Could not get %s device. Using false data.", DT_LABEL(ULTRASONIC_DEV));
    }
}

void myUltrasonicSensor::update_distance()
{
    if (dev)
    {
        int ret = sensor_sample_fetch(dev);
        if (ret == 0)
        {
            ret = sensor_channel_get(dev, SENSOR_CHAN_DISTANCE, &distance);
            if (ret != 0)
            {
                LOG_ERR("Failed to fetch distance data: %d", ret);
            }
        }
        else
        {
            LOG_ERR("Failed to fetch samples: %d", ret);
        }
    }
    else
    {
        false_value++;
        distance = 10.0 + false_value % 25; // Valeur fictive si le capteur n'est pas prêt
    }
}

double myUltrasonicSensor::get_distance()
{
    return distance;
}

      /* trig-gpios = <&gpio0 4 GPIO_ACTIVE_HIGH>;    
        echo-gpios = <&gpio0 5 GPIO_ACTIVE_HIGH>;*/