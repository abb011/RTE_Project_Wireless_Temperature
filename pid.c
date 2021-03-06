#include "pid.h"
#include "OneWire/tm_stm32f4_pwm.h"
 TM_PWM_TIM_t TIM2_Data;
 
 static uint32_t period;
 static volatile uint32_t T_On;
 static arm_pid_instance_f32 pid_loop;
 static uint8_t initted= 0;
 static float * temperatures;
 static float * sp;
 static float op = 0;
 static float lastTemp = 0;
 
 //Gets current Output and average temperature values. These values were internal but are presented on teh HTTP page
 void getDatas(float * op_, float * temp){
	 *op_ = op;
	 *temp = lastTemp;
 }
 
 //Used for PWM implimentation and display
 static void LED_On(uint32_t i)
{
  GPIOD->BSRRL = 1 << (i+12);
}

static void LED_Off(uint32_t i)
{
  GPIOD->BSRRH = 1 << (i+12);
}

//Init the PWM pins
 static void initPWM(uint32_t period_ms){
	period = period_ms;

	GPIO_InitTypeDef GPIO_InitDef;
	GPIO_InitDef.GPIO_Pin= GPIO_Pin_10;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitDef);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10 );
}

//Run PWM. This function needs to be called periodically(systick handler) to control the PWM, ms_incr is the time in miliseconds since the last call.
void runPWM(uint32_t ms_incr){
	if(initted ==0)
		return;
	static uint32_t time = 0;
	time = (time+ms_incr)%period;
	if(time<T_On){
		GPIO_SetBits(GPIOB, GPIO_Pin_10 );
		LED_On(2);
	}else{
		GPIO_ResetBits(GPIOB, GPIO_Pin_10 );
		LED_Off(2);
	}
	
}



//Init the PID controller
void initPID(float kp, float ki, float kd, float * temp_array, float *setpoint){
    initPWM(60*1000);
	pid_loop.Kp = kp;
	pid_loop.Ki = ki;
	pid_loop.Kd = kd;
	sp = setpoint;
	temperatures = temp_array;
	
	arm_pid_init_f32(&pid_loop, 1);
	initted = 1;
}

//Resets the temperature average array to handle lost devices from impacting the average temperature
static float reset_avg(float * temp){
	for(uint8_t i = 0; i < ESP8266_MAX_CONNECTEDSTATIONS; i++){
		temp[i] = -1;
	}
}

//Run the PID with the latest values. This function should be called to update the PWM output
void run_PID(){
	lastTemp = getAverageTemperature(temperatures);
	if(lastTemp<5.0)
		lastTemp = *sp;
	float error = (*sp)- lastTemp;
	op = arm_pid_f32(&pid_loop, error);
	D(printf("Current Error is %f\nCurrent OP is %f\n",error, op));
	if(op<L_THRESHOLD)
		op = 0.0;
	if(op>H_THRESHOLD)
		op = 100.0;
	
	T_On = (period/100.0)*op;
	reset_avg(temperatures);
}


//Calculates and returns the average temperature values. The only values considered are those above 0.
float getAverageTemperature(float* temp){
	float average = 0;
	uint8_t count = 0;
	for(uint8_t i = 0; i < ESP8266_MAX_CONNECTEDSTATIONS; i++){
		if(temp[i] >0){
			average+=temp[i];
			count++;
		}
	}
	if(count ==0)
		return -1;
	return average/count;
	
}