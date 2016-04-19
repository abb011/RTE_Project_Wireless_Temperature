Notes:

ESP8266
Using UART 4 
PWR = 3V
Grnd = GRND
RX = UART_TX = GPIOC10
TX = UART_RX = GPIOC11
Reset = GPIOC12
GPIOA1 = 1 (3.3V) for Access point| 0 (0V) for Secondary measurement points

DSP18b20 Pins
Black = GRND
RED = 5V
Yellow = DATA = GPIOC15


Communication flow ideas
Access point hosts HTTP server on:8000		--Done
All secondary devices host data server on port: #  --Open

Access point periodically sequentionally initiates a connection to all connected wifi modules (they must be in server mode) and requests data via port --Open

Access point then uses this to calculate the PID output --Open

Access point updates PWM signal. --Open

Other notes:
	There seems to be some bugs in the ESP8266.c file. I have fixed a few, but occasionally the device somehow manages to lose data that it receives. Possibly a UART issue.

