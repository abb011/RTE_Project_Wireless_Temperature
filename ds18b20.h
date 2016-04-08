#ifndef __ds18b20__
#define __ds18b20__

#include "OneWire/tm_stm32f4_onewire.h"
#include "OneWire/tm_stm32f4_delay.h"
#include "OneWire/tm_stm32f4_delay.h"

#define MSB_MASK 0x07
#define LSB_MASK_FRAC 0x0f
#define LSB_MASK_WHOLE 0xf0
#define LSB_WHOLE_SHIFT 4
#define MSB_POW 4
#define LSB_FRAC_DIV 16.0
#define TEMP_CONV_COMMAND 0x44
#define ONLY_ONE_SLAVE 0xCC
#define READ_DATA  0xBE
#define SIGN_BIT 0x10
#define MAX_I_W 0xffffffff
#define DS18B20_PERIOD 0.8

#define NUM_READINGS 4


uint8_t init_tempSensor();

float getTemperature();

float getAvgTemperature();

void storeTemperature();

#endif