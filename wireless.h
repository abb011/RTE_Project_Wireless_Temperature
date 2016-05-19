#include "ESP8266/esp8266.h"
#include "ESP8266/esp8266_ll.h"
#include "stm32f4xx_gpio.h"
#include <stdio.h>
#include "defines.h"

#define GET_HOMEPAGE "GET /index.html "
#define SET_SP "GET /set_temp.asp?setpoint="
#define NETWORK_SSID "HomeAutomation"
#define NETWORK_PWD "password1234567890"
#define TEMPERATURE_REQ "GET /read_temp.asp?temperature=\n"
#define TEMPERATURE_R "GET /read_temp.asp?temperature="
#define TEMPERATURE_REPLY_FORMAT "GET /read_temp.asp?temperature=%f/source.asp?=%d\n\n\n"
#define HB_PORT 8000
#define SEC_PORT 4000
#define NUM_DEVICE 4


void sendToConnection();
void initWireless(ESP8266_t *w, float * sp, float * temperature_p, float * temperatures_p_p, uint16_t numElements);
void pullRemoteDevices();