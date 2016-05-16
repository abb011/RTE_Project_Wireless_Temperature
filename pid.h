#ifndef PID_H
#define PID_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "defines.h"
#include "arm_math.h"
#include "ESP8266/esp8266.h"

#define L_THRESHOLD 10.0
#define H_THRESHOLD 100.0
float getAverageTemperature(float* temp);
void run_PID();
void initPID(float kp, float ki, float kd, float * temp_array, float *setpoint);
void runPWM(uint32_t ms_incr);

#endif
