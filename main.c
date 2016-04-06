#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#include "ds18b20.h"

#include <stdio.h>

#include "homework3.h"

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

void printTemps(){
	float temp_C = getTemperature();
	printf("The current Temperature is %f\n",temp_C);
	return;
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
  
 
  delay_ms(20);
  
  add_timed_task(print_times, 5);
  //add_timed_task(gesture_detection, .02);

  
  
  delay_ms(100);
  //add_timed_task(read_acc,0.004);
  //add_timed_task(gesture_detection_avg, 0.02);
  //add_timed_task(printPitchAndRoll,1.0);
  printf("%d\n", init_tempSensor());
  float temp_C = getTemperature();
  printf("The current Temperature is %f\n",temp_C);
  delay_ms(1000);
  add_timed_task(printTemps, 2);
  while(1){
	  run_TimedTasks();
	  
  }

/*  uint32_t t_prev = 0;
  while (1)
	{
    if ( (msTicks - t_prev) >= 1000) // 1 second has elapsed
    {
      t_prev = msTicks;
      float a[3]; // array of 3 floats into which accelerometer data will be read
      read_accelerometers(a); // read data from accelerometers (X, Y, and Z axes)
      printf("%f %f %f\n", a[0], a[1], a[2]);

      calc_pitch_roll(a[0], a[1], a[2], &pitch, &roll);
      printf("Pitch = %f, Roll = %f\n", pitch, roll);
    }
  }
  */
}




