#ifndef HCSR04_HPP
#define HCSR04_HPP

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/sensor.h>

//#define ULTRASONIC_DEV DT_CHOSEN(HCSR04) 
//  utiliser DT_ALIAS(ultrasonic) 

class myUltrasonicSensor
{
public:
    double distance_d; 
    struct sensor_value distance;

    int init();

    void update_distance();
    struct sensor_value get_distance();
};

#endif
