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
ESP8266_APConfig_t ap_s;
extern uint16_t numConnections;
extern uint16_t reply;

void esp8266_update_func(){
	ESP8266_Update(&wireless_S);
}
void sendToConnection(){
	if(reply && (numConnections>0)){
		ESP8266_WaitReady(&wireless_S);
		ESP8266_RequestSendData(&wireless_S, &(wireless_S.Connection[0]));
		reply = 0;
	}else if (numConnections>1){
		printf("Number Connections %d\n", numConnections);
	}
}

int main(void)
{
  // initialize
  ap_s.SSID = "TEST";
  ap_s.Pass = "";
  ap_s.Ecn = ESP8266_Ecn_OPEN;//ESP8266_Ecn_t. ;
  ap_s.Channel = 2;
  ap_s.MaxConnections = 4; //Cannot be greater than 4
  ap_s.Hidden = 0;
  
  SystemInit();
  initialise_monitor_handles();
  init_systick();
  init_LED_pins();
  init_button();
  
  //Init the UARt
  //USART_Configuration();

  //uart_print(UART4, "Hello World\r\n");
  printf("%d\n", init_tempSensor());
  delay_ms(1000);
  float temp_C = getTemperature();
  
  //struct ESP8266
  printf("The current Temperature is %f\n",temp_C);

  add_timed_task(storeTemperature, DS18B20_PERIOD);
  //add_timed_task(printAVGTemps,4);
  add_timed_task(esp8266_update_func, .05);
  add_timed_task(sendToConnection, 2);
  
  ESP8266_Init(&wireless_S, 115200);
  printf("Initializing Wifi \n");
  ESP8266_WaitReady(&wireless_S);
  ESP8266_ListWifiStations(&wireless_S);
  ESP8266_WaitReady(&wireless_S);
  delay_ms(1000);
  ESP8266_SetMode(&wireless_S, ESP8266_Mode_AP);

  ESP8266_WaitReady(&wireless_S);
  ESP8266_Result_t res = ESP8266_SetAP(&wireless_S, &ap_s);
  printf("Hosting a Wifi AccessPoint: %d\n", res);
  
  ESP8266_WaitReady(&wireless_S);
  uint16_t port = 3333;
  res = ESP8266_ServerEnable(&wireless_S, port);
  printf("Hosting a server at port %d: Success = 0: %d\n", port, res);
  
  //add_timed_task(printstuff,0.5);
  
  
  while(1){
	  run_TimedTasks();

	  
  }

}




