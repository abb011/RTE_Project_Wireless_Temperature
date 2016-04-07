#include "ds18b20.h"

#include <stdio.h>


#define MSB_MASK 0x07
#define LSB_MASK_FRAC 0x0f
#define LSB_MASK_WHOLE 0xf0
#define LSB_WHOLE_SHIFT 4
#define MSB_POW 4
#define LSB_FRAC_DIV 16.0
#define TEMP_CONV_COMMAND 0x44
#define ONLY_ONE_SLAVE 0xCC
#define READ_DATA  0xBE

static  TM_OneWire_t temperatureSensor;

static uint8_t init_ds18b20(TM_OneWire_t * tempSensor, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	TM_DELAY_Init();
	TM_OneWire_Init(tempSensor, GPIOx, GPIO_Pin);
	//TM_OneWire_Reset(tempSensor);
	return TM_OneWire_First(tempSensor);
}

static float read_temperature(TM_OneWire_t * tempSensor){
	uint8_t data[8];
	uint8_t lsb, msb;
	uint16_t wholeTemp;
	float temp;
	
	//Get the address for selecting
	TM_OneWire_GetFullROM(tempSensor,data);
	TM_OneWire_Reset(tempSensor);
	TM_OneWire_Select(tempSensor, data);
	TM_OneWire_WriteByte(tempSensor, READ_DATA);
	
	
	lsb = TM_OneWire_ReadByte(tempSensor);
	msb = TM_OneWire_ReadByte(tempSensor);
	printf("LSB: %x\n", lsb);
	printf("MSB: %x\n", msb);
	

	
	wholeTemp = (lsb & LSB_MASK_WHOLE)>> LSB_WHOLE_SHIFT;
	wholeTemp += (msb&MSB_MASK)<<MSB_POW;
	
	temp = (lsb & LSB_MASK_FRAC)/LSB_FRAC_DIV;
	temp += wholeTemp;
	return temp;
}

uint8_t init_tempSensor(){
	return init_ds18b20(&temperatureSensor, GPIOC, GPIO_Pin_15);
}

float getTemperature(){
	//Start getting a new Temp measurement
	TM_OneWire_Reset(&temperatureSensor);
	uint8_t byte = ONLY_ONE_SLAVE;
	TM_OneWire_WriteByte(&temperatureSensor, byte);
	byte = TEMP_CONV_COMMAND;
	TM_OneWire_WriteByte(&temperatureSensor, byte);
	
	Delay(1000);
	return read_temperature(&temperatureSensor);
}