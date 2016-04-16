#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "wireless.h"


#include "ds18b20.h"

#include <stdio.h>

#include "homework3.h"
#include "misc.h"	
#include "stm32f4xx_usart.h"

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



void initialise_monitor_handles();

void printAVGTemps(){
	float temp_C = getAvgTemperature();
	printf("The Average Temperature is %f\n",temp_C);
	return;
}

//UART Structs
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

// UART Pin C10 (TXD) C11 (RXD)
void USART_Configuration(void){
	//Clocks
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
		// Map UART4 to C.10 and C.11
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	
	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	
	// Initialize Transmit
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	/* Configure USART */
	USART_Init(UART4, &USART_InitStructure);
	
	//Enable receive Interrupt
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	
	// Init the receive side and enable interrupts
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	
	/* Enable the USART */
	USART_Cmd(UART4, ENABLE);
	
}

void uart_print(USART_TypeDef * USARTx, volatile char * s ){
	//Until the data is 0
	while(*s){
		while(!(USARTx->SR&0x00000040));
		USART_SendData(USARTx, *s);
		*s++;
	}
	
}

void UART4_IRQHandler(void){
	//printf("In Interrupt Handler\n");
	// check if the UART4 receive interrupt flag was set
	if( USART_GetITStatus(UART4, USART_IT_RXNE) ){
		
		static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = UART4->DR; // the character from the USART4 data register is saved in t
		
		/* check if the received character is not the LF character (used to determine end of string) 
		 * or the if the maximum string length has been been reached 
		 */
		if( (((char)t) != '\n') && (cnt < MAX_STRLEN) ){ 
			received_string[cnt] = t;
			cnt++;
			//printf("Received %c\n", (char)t);
		}
		else{ // otherwise reset the character counter and print the received string
			received_string[cnt] = '\0';
			cnt = 0;
			printf("%s\n", received_string);
		}
	}
}

float pitch, roll;
uint32_t gesture_state=0;
uint32_t announce_gesture = 0;
int main(void)
{
  // initialize
  SystemInit();
  initialise_monitor_handles();
  init_systick();
  init_LED_pins();
  init_button();
  
  //Init the UARt
  USART_Configuration();

  uart_print(UART4, "Hello World\r\n");
  printf("%d\n", init_tempSensor());
  delay_ms(1000);
  float temp_C = getTemperature();
  
  printf("The current Temperature is %f\n",temp_C);

  add_timed_task(storeTemperature, DS18B20_PERIOD);
  add_timed_task(printAVGTemps,4);
  //add_timed_task(printstuff,0.5);
  
  
  while(1){
	  run_TimedTasks();
	  
  }

}




