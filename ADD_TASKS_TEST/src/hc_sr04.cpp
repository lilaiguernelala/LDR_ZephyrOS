#include "hc_sr04.hpp"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_hc_sr04);

void myHC_SR04::init(const struct device *hc_sr04_dev)
{
    dev = hc_sr04_dev;
    if (!device_is_ready(dev))
    {
        LOG_ERR("Could not get %s device. Using false data.", dev->name);
    }
}

void myHC_SR04::update_values()
{
    if (device_is_ready(dev))
    {
        sensor_sample_fetch(dev);
        sensor_channel_get(dev, SENSOR_CHAN_DISTANCE, &distance);
    }
    else
    {
        false_value++;
        sensor_value_from_double(&distance, 10 + false_value % 30);
    }
}

double myHC_SR04::get_distance()
{
    return sensor_value_to_double(&distance);
}