Notes:

ESP8266
Using UART 4 
PWR = 3V
Grnd = GRND
RX = UART_TX = GPIOC10
TX = UART_RX = GPIOC11
RESET = GPIOC12

DSP18b20 Pins
Black = GRND
RED = 5V
Yellow = DATA = GPIOC15

Mode:
GPIOA1 = 1 for Primary host, 0  for secondary point

Communication flow ideas
Access point hosts HTTP server on:8080
All secondary devices host data server on port: #

Access point periodically sequentionally initiates a connection to all connected wifi modules (they must be in server mode) and requests data via port

Access point then uses this to calculate the PID output

Access point updates PWM signal.

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

