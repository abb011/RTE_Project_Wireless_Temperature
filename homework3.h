#ifndef HW3_H
#define HW3_H

#include <stdio.h>
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

#include "mystruct.h"


void toggle_LED(uint32_t i);
void toggle_LED0();
void toggle_LED1();
void toggle_LED2();
void toggle_LED3();
void print_times();


void add_timed_task(void (* function), float period_s);


//homework 2 things
void __attribute__((weak)) LED_on(uint32_t i);
void __attribute__((weak)) LED_off(uint32_t i);
uint32_t __attribute__((weak)) read_button(void);
int max(int * arr, uint32_t n, uint32_t * max_Index);
void run_TimedTasks();


//homework 3 things
void button_detector(uint32_t detection_length);
void button_announcer();
#endif