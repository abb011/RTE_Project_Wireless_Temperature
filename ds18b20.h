#ifndef __ds18b20__
#define __ds18b20__

#include "tm_stm32f4_onewire.h"
#include "tm_stm32f4_delay.h"

uint8_t init_tempSensor();

float getTemperature();

#endif