Notes:

ESP8266
Using UART 4 
PWR = 3V
Grnd = GRND
RX = UART_TX = GPIOC10
TX = UART_RX = GPIOC11
Reset = GPIOC12
GPIOA1 = 1 (3.3V) for Access point| 0 (0V) for Secondary measurement points
RESET = GPIOC12


DSP18b20 Pins
Black = GRND
RED = 5V
Yellow = DATA = GPIOC15

Mode:
GPIOA1 = 1 for Primary host, 0  for secondary point

Communication flow ideas
Access point hosts HTTP server on:8000		--Done
All secondary devices host data server on port: #  --Open

Access point periodically sequentionally initiates a connection to all connected wifi modules (they must be in server mode) and requests data via port --Open

Access point then uses this to calculate the PID output --Open

Access point updates PWM signal. --Open

Other notes:
	There seems to be some bugs in the ESP8266.c file. I have fixed a few, but occasionally the device somehow manages to lose data that it receives. Possibly a UART issue.

Things we need to do:
Impliment the defined interrupt functions as required in the libraries
	- Secondary Server Functions
	- Primary server data request cycle
	- Add a timeout to IPD mode. 
		- Store MSticks when we enter IPD mode and reset to 0 when we leave IPD mode
		- Check if it has been exceeded by some threshold for the timeout we could do 5 seconds potentially
		  The system is very slow to update but taht could be from the USB debugging and printing
		- Need to empty buffer and close the connection
	- Add PID
	- Add PWM for PID output

