/**	
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
 */
#include "esp8266_ll.h"

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/*                                                                        */
/*    Edit file name to esp8266_ll.c and edit values for your platform    */
/*                                                                        */
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
//UART Structs
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;

uint8_t ESP8266_LL_USARTInit(uint32_t baudrate) {
	/* Init USART */
	//Clocks
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	
		// Map UART4 to C.10 and C.11
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
	
	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	
	// Initialize Transmit
	USART_InitStructure.USART_BaudRate = baudrate;
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
	
	/* Return 0 = Successful */
	return 0;
}

uint8_t ESP8266_LL_USARTSend(uint8_t* data, uint16_t count) {
	//Could incorporate a timeout and return 1 if need be.
	for(uint16_t i = 0; i< count; i++){
		while(!(UART4->SR&0x00000040)); //Block until the UART is ready to send
			USART_SendData(UART4, data[i]); //Send the data
	}
	
	/* Return 0 = Successful */
	return 0;
}

void UART4_IRQHandler(void){
	//printf("In Interrupt Handler\n");
	// check if the UART4 receive interrupt flag was set
	if( USART_GetITStatus(UART4, USART_IT_RXNE) ){
		uint8_t ch;
	/* Get character from USART */
		ch = UART4->DR;
	/* Send received character to ESP stack */
		ESP8266_DataReceived(&ch, 1);
	}
	if(USART_GetITStatus(UART4, USART_IT_ORE_RX) || USART_GetITStatus(UART4, USART_IT_ORE_ER)|| USART_GetITStatus(UART4, USART_IT_NE)|| USART_GetITStatus(UART4, USART_IT_FE)|| USART_GetITStatus(UART4, USART_IT_PE))
		printf("INTERRUPT ERROR\n");
}

void init_ESP8266_reset(){
	GPIO_InitTypeDef GPIO_InitDef;
	GPIO_InitDef.GPIO_Pin= GPIO_Pin_12;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitDef);
}
void reset_low_ESP8266(){
	GPIO_ResetBits(GPIOC, GPIO_Pin_12 );
}
void reset_high_ESP8266(){
	GPIO_SetBits(GPIOC, GPIO_Pin_12 );
}

/*
void UART4_IRQHandler(void){
	//printf("In Interrupt Handler\n");
	// check if the UART4 receive interrupt flag was set
	if( USART_GetITStatus(UART4, USART_IT_RXNE) ){
		
		static uint8_t cnt = 0; // this counter is used to determine the string length
		char t = UART4->DR; // the character from the USART4 data register is saved in t
		
		 check if the received character is not the LF character (used to determine end of string) 
		 or the if the maximum string length has been been reached 
		/
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
}*/

/* USART receive interrupt handler */
//void USART_RX_INTERRUPT_HANDLER_FUNCTION_NAME(void) {
//	uint8_t ch;
	/* Get character from USART */
	
	
	/* Send received character to ESP stack */
//	ESP8266_DataReceived(&ch, 1);
//}
