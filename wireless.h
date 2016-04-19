#include "ESP8266/esp8266.h"
#include "stm32f4xx_gpio.h"
#include <stdio.h>

#define GET_HOMEPAGE "GET /index.html "
#define SET_SP "GET /set_temp.asp?setpoint="
#define NETWORK_SSID "HomeAutomation"
#define NETWORK_PWD "password1234567890"
void sendToConnection();
void initWireless(ESP8266_t *w, float * sp);