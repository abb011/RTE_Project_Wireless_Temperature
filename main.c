#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "wireless.h"


#include "ds18b20.h"

#include <stdio.h>

#include "homework3.h"
#include "misc.h"	
#include "stm32f4xx_usart.h"
#include "ESP8266/esp8266.h"

#define MAX_STRLEN 32
volatile char received_string[MAX_STRLEN+1];
static void init_systick();
static void delay_ms(uint32_t n);
volatile uint32_t msTicks; // counts 1 ms timeTicks

// SysTick Handler (Interrupt Service Routine for the System Tick interrupt)
/*void SysTick_Handler(void)
{
  msTicks++;
}*/

// initialize the system tick
void init_systick(void)
{
	SystemCoreClockUpdate();                      /* Get Core Clock Frequency   */
  if (SysTick_Config(SystemCoreClock / 1000)) { /* SysTick 1 msec interrupts  */
    while (1);                                  /* Capture error              */
  }
}

// pause for a specified number (n) of milliseconds
void delay_ms(uint32_t n)
{
  uint32_t msTicks2 = msTicks + n;
  while(msTicks < msTicks2) ;
}



void init_LED_pins()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;  // enable clock to GPIOD

  for (int i=12; i<=15; i++)
  {
    GPIOD->MODER &= ~(0x3 << (2*i)); // clear the 2 bits corresponding to pin i
    GPIOD->MODER |= (1 << (2*i));    // set pin i to be general purpose output
  }
}

void LED_On(uint32_t i)
{
  GPIOD->BSRRL = 1 << (i+12);
}

void LED_Off(uint32_t i)
{
  GPIOD->BSRRH = 1 << (i+12);
}


void init_button()
{
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;  // enable clock to GPIOA

  GPIOA->MODER &= ~(0x3 << (2*0)); // clear the 2 bits corresponding to pin 0
  // if the 2 bits corresponding to pin 0 are 00, then it is in input mode
}


//TEst commit
void initialise_monitor_handles();

void printAVGTemps(){
	float temp_C = getAvgTemperature();
	printf("The Average Temperature is %f\n",temp_C);
	return;
}



float pitch, roll;
uint32_t gesture_state=0;
uint32_t announce_gesture = 0;
ESP8266_t wireless_S;
extern uint16_t numConnections;
extern uint16_t * reply;



void esp8266_update_func(){
	ESP8266_Update(&wireless_S);
	sendToConnection();
}

#define NUM_ELEMENTS 10
int main(void)
{


  float sp, local_temperature;
  float temperature_array[NUM_ELEMENTS];
  // initialize
  
  SystemInit();
  initialise_monitor_handles();
  init_systick();
  init_LED_pins();
  init_button();
  
  initWireless(&wireless_S, &sp, &local_temperature, &temperature_array, NUM_ELEMENTS);
  
  //Init the UARt
  //USART_Configuration();

  //uart_print(UART4, "Hello World\r\n");
  printf("%d\n", init_tempSensor());
  delay_ms(1000);
  float temp_C = getTemperature();
  printf("The current Temperature is %f\n",temp_C);

  add_timed_task(storeTemperature, DS18B20_PERIOD);
  //add_timed_task(printAVGTemps,4);
  add_timed_task(esp8266_update_func, .25);
  //add_timed_task(sendToConnection, .05);
  
  
  //add_timed_task(printstuff,0.5);
  
  
  while(1){
	  run_TimedTasks();
	  //ESP8266_Update(&wireless_S);
	  
  }

}




