#ifndef __ds18b20__
#define __ds18b20__

#include "OneWire/tm_stm32f4_onewire.h"
#include "OneWire/tm_stm32f4_delay.h"
#include "OneWire/tm_stm32f4_delay.h"

uint8_t init_tempSensor();

float getTemperature();

#endif