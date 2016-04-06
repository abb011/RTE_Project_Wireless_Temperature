#include "homework3.h"
#define B_RELEASED 0
#define B_PRESSED 1
#define B_SILENT 0
#define B_ANNOUNCE 1
#define B_MAX_COUNT 250


extern struct TimedTask timed_tasks[10];
uint32_t TT_count = 0;
extern volatile uint32_t msTicks;
uint32_t LED_Status = 0;


void toggle_LED(uint32_t i){
	uint32_t LED_Mask = ( 1<<i);
	LED_Status = LED_Status ^LED_Mask;
	
	if(LED_Status & LED_Mask){
		LED_on(i);
	}else{
		LED_off(i);
	}
}


void toggle_LED0(){
	toggle_LED(12);
}
void toggle_LED1(){
	toggle_LED(13);
}
void toggle_LED2(){
	toggle_LED(14);
}
void toggle_LED3(){
	toggle_LED(15);
}
void print_times(){
	uint32_t i;
	//Loop through all scheduled tasks
	for(i = 0; i<TT_count; i++){
		printf("%lu: %lu @t = %lu \n",i, timed_tasks[i].times_called,msTicks);
	}
	printf("\n");
}

void add_timed_task(void (* function), float period_s){
	timed_tasks[TT_count].fp_void = function;
	timed_tasks[TT_count].last_time = 0;
	timed_tasks[TT_count].times_called = 0;
	timed_tasks[TT_count++].period_ms = (uint32_t)(period_s*1000);
}
void run_TimedTasks(){
	uint32_t i;
	//Loop through all scheduled tasks
	for(i = 0; i<TT_count; i++){
		//if it hasn't been run before or the period has elapsed
		if((timed_tasks[i].last_time == 0) || (timed_tasks[i].last_time+timed_tasks[i].period_ms<=msTicks)){
			timed_tasks[i].last_time = msTicks;
			timed_tasks[i].fp_void();
			timed_tasks[i].times_called++;
		}
	}
}

uint16_t button_state = B_PRESSED;
uint16_t b_announced = B_SILENT;
uint16_t b_count = 0;
void button_detector(uint32_t detection_length){
	switch (button_state){
		//detects if the button is released
		case B_RELEASED:
			if(read_button())
				b_count = 0;
			else
				b_count++;
			if(b_count >= detection_length){
				b_announced = B_ANNOUNCE;
				button_state = B_PRESSED;
				b_count = 0;
			}
			return;
		//Detects if the button is pressed
		case B_PRESSED:
			if(read_button())
				b_count++;
			else
				b_count = 0;
			if(b_count >= detection_length){
				b_announced = B_ANNOUNCE;
				button_state = B_RELEASED;
				b_count = 0;
			}
			return;
		default:
			return;
	}
	return;
}

void button_announcer(){
	if(b_announced == B_ANNOUNCE){
		b_announced = B_SILENT;
		if(button_state == B_PRESSED){
			printf("Buton Released\n");
		}
		if(button_state == B_RELEASED){
			printf("Button Pressed\n");
		}
	}else{

	}
	return;
}


void __attribute__((weak)) LED_on(uint32_t i){
	uint16_t tempVal = (1<<i);
	GPIOD->BSRRL = tempVal;
}
void __attribute__((weak)) LED_off(uint32_t i){
	uint16_t tempVal = (1<<i);
	GPIOD->BSRRH = tempVal;
}

uint32_t __attribute__((weak)) read_button(void){
	uint32_t ret = GPIOA->IDR& 0x1;
	return ret;
}

int max(int * arr, uint32_t n, uint32_t * max_Index);
