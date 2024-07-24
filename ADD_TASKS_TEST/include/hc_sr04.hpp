#ifndef HC_SR04_HPP
#define HC_SR04_HPP

#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/pwm.h>
#include <sensor/hc_sr04/hc_sr04.h>

class myHC_SR04
{
public:
    const struct device *dev;

    struct sensor_value distance;
    int false_value = 0;

    void init(const struct device *hc_sr04_dev);

    void update_values();
    double get_distance();
};

#endif