#include "ds18b20.h"


static float temp_measured[NUM_READINGS];
static uint32_t index_w=0;
static  TM_OneWire_t temperatureSensor;

//Initialize the Temperature Sensor
static uint8_t init_ds18b20(TM_OneWire_t * tempSensor, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	TM_DELAY_Init();
	TM_OneWire_Init(tempSensor, GPIOx, GPIO_Pin);
	//TM_OneWire_Reset(tempSensor);
	
	//Start the first temperature conversion
	uint8_t numDevices = TM_OneWire_First(tempSensor);
	TM_OneWire_Reset(tempSensor);
	uint8_t byte = ONLY_ONE_SLAVE;
	TM_OneWire_WriteByte(tempSensor, byte);
	byte = TEMP_CONV_COMMAND;
	TM_OneWire_WriteByte(tempSensor, byte);
	
	return numDevices;
}

//Read the Current measured temperature
static float read_temperature(TM_OneWire_t * tempSensor){
	uint8_t data[8];
	uint8_t lsb, msb;
	uint16_t wholeTemp;
	float temp;
	
	//Get the address for selecting
	TM_OneWire_GetFullROM(tempSensor,data);
	TM_OneWire_Reset(tempSensor);
	TM_OneWire_Select(tempSensor, data);
	//Do I NEED THE ABOVE SELECTION THINGS? Will need to experiment
	
	TM_OneWire_WriteByte(tempSensor, READ_DATA);
	
	
	lsb = TM_OneWire_ReadByte(tempSensor);
	msb = TM_OneWire_ReadByte(tempSensor);
	float sign = 1.0;
	if(msb&SIGN_BIT){
		sign = -1.0;
	}
	//D(printf("LSB: %x\n", lsb));
	//D(printf("MSB: %x\n", msb));
	

	
	wholeTemp = (lsb & LSB_MASK_WHOLE)>> LSB_WHOLE_SHIFT;
	wholeTemp += (msb&MSB_MASK)<<MSB_POW;
	
	temp = (lsb & LSB_MASK_FRAC)/LSB_FRAC_DIV;
	temp += wholeTemp;
	temp *= sign;
	return temp;
}

//Externally called init function.
uint8_t init_tempSensor(){
	return init_ds18b20(&temperatureSensor, GPIOC, GPIO_Pin_15);
}

//Gets the previously measured temperature as well as issues a new temperature conversion command. Cannot be called more frequently than every 750 ms. 
float getTemperature(){
	//Start getting a new Temp measurement
	//Delay(1000);
	float temp = read_temperature(&temperatureSensor);
	temp_measured[(index_w++)%NUM_READINGS] = temp;

	
	//Handle overflow case so we have the whole average not just firt point
	if(index_w == MAX_I_W)
		index_w += NUM_READINGS;
	
	
	TM_OneWire_Reset(&temperatureSensor);
	uint8_t byte = ONLY_ONE_SLAVE;
	TM_OneWire_WriteByte(&temperatureSensor, byte);
	byte = TEMP_CONV_COMMAND;
	TM_OneWire_WriteByte(&temperatureSensor, byte);
	
	return temp;
}
void storeTemperature(){
	getTemperature();
}
//Get the average of the last few measurements. This function ended up not being used as the temperature sensors are not noisy. 
float getAvgTemperature(){
	uint32_t limit = index_w;
	uint32_t i;
	float average=0;
	if(index_w>NUM_READINGS-1)
		limit = NUM_READINGS;
	for(i = 0; i<limit; i++){
		average+=temp_measured[i];
	}
	return average/limit;
}