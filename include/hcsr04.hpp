#ifndef HCSR04_HPP
#define HCSR04_HPP

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

//#define ULTRASONIC_DEV DT_CHOSEN(HCSR04) 
//  utiliser DT_ALIAS(ultrasonic) 

class myUltrasonicSensor
{
public:
    double distance; 

    int init();

    int update_distance();
    double get_distance();
};

#endif
