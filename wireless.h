#include "ESP8266/esp8266.h"
#include "stm32f4xx_gpio.h"
#include <stdio.h>

#define GET_HOMEPAGE "GET /index.html "
#define SET_SP "GET /set_temp.asp?setpoint="
#define NETWORK_SSID "HomeAutomation"
#define NETWORK_PWD "password1234567890"
#define TEMPERATURE_REQ "GET /read_temp.asp?temperature="
#define TEMPERATURE_REPLY_FORMAT "GET /read_temp.asp?temperature=%f/source.asp?=%d"
#define HB_PORT 8000
#define SEC_PORT 4000
void sendToConnection();
void initWireless(ESP8266_t *w, float * sp);