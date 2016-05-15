#ifndef PID_H
#define PID_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stdint.h"


void set_LED_dutycycle(uint32_t i , uint32_t duty_cycle);
void init_LED_PWM();
 void initPWM();
#endif
