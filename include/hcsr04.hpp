#ifndef HCSR04_HPP
#define HCSR04_HPP

#include <zephyr/drivers/sensor.h>

#define ULTRASONIC_DEV DT_CHOSEN(HCSR04) 
//  utiliser DT_ALIAS(ultrasonic) 

class myUltrasonicSensor
{
public:
    const struct device *dev;

    double distance; 

    void init();

    void update_distance();
    double get_distance();
};
#endif
