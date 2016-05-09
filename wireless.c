#include "wireless.h"

uint16_t numConnections = 0;
uint16_t reply[ESP8266_MAX_CONNECTIONS];
uint16_t timed_out[ESP8266_MAX_CONNECTIONS];

static ESP8266_t * wireless_S;
static float * setpoint;
static uint16_t homebase = 0;
static void serverReply();
static uint16_t initHBServer();
static void initDataServer();
static void dataReply();

void initWireless(ESP8266_t *w, float * sp){
	for(uint8_t i = 0; i<ESP8266_MAX_CONNECTIONS; i++){
		timed_out[i] = 0;
	}
	wireless_S = w;
	setpoint = sp;
	printf("Starting SP %f\n", *sp);
	//Init a GPIO and read to see if it is homebase or not
	GPIO_InitTypeDef GPIO_InitDef;
	GPIO_InitDef.GPIO_Pin= GPIO_Pin_1;
	GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitDef.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitDef);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	homebase = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1);
	printf("Homebase %d\n", homebase);
	if(homebase){
		while(initHBServer());
	}else{
		initDataServer();
	}
}
void sendToConnection(){
	for(uint8_t i = 0; i < ESP8266_MAX_CONNECTIONS; i++){
		if(timed_out[i]){
			printf("Closing Connection: %d\n", ESP8266_CloseConnection(wireless_S, &(wireless_S->Connection[i])));
			timed_out[i] = 0;
		}
	}
	if (homebase){
		serverReply();
		return;
	}
	dataReply();
}

//Handles HTTP Processing
static
void serverReply(){
	ESP8266_Connection_t * con;
	for(uint i = 0; i<ESP8266_MAX_CONNECTIONS; i++){
		con = &(wireless_S->Connection[i]);
		if(reply[con->Number] && con->Active){
			static char temp[4000]; //= "Content-Type: text/html\r\nContent-Length: 111\r\n\r\n<html>\r\n<body>\r\n<h1>Happy New Millennium!</h1>\r\n(more file contents)\r\n  .\r\n  .\r\n  .\r\n</body>\r\n</html>";
			static char t1[100] = "HTTP/1.1 200 OK\r\n";
			static char t0[1000];
				if(reply[con->Number] == 1)
					sprintf(t0,"<html> <title> Temperature Stuff</title> <body> Wifi Temperature Homepage </body>  <form action=\"set_temp.asp\">Current Set Point:%f New:<input type=\"text\" name=\"setpoint\">%cC<br><input type=\"submit\" value=\"Submit\"></form> </html>\n", *setpoint, (char)(176) );
					//sprintf(t0,"<html> <title> Temperature Stuff</title> <body> Wifi Temperature Homepage </body>  <form action=\"set_temp.asp\">Current Set Point:%f New:<input type=\"text\" name=\"setpoint\">%cC<br><input type=\"submit\" value=\"Submit\"></form> </html>\n", (char)(176), *setpoint);
				if(reply[con->Number]==2)
					sprintf(t0,"<html> <title> Temperature Stuff</title> <body> You Submitted a New Setpoint: %f</body>  </html>\n", *setpoint);
				if(reply[con->Number] ==3){
					sprintf(t1, "HTTP/1.0 404 Not Found\r\n");
					sprintf(t0,"<html> <title> Temperature Stuff</title> <body> 404 page not found. try/index.html </body>  </html>\n");
				}
			static char t2[100] = "Content-Type: text/html; chaset=mbc\r\n";
			static char t3[100];
			sprintf(t3, "Content-Length: %d\r\n\r\n", strlen(t0));
			//sprintf(temp, "%s%s%s",t2,t3,t0);
			sprintf(temp, "%s%s%s%s",t1,t2,t3,t0);
			printf("REPLY: %s\n",temp);
			
			printf("%d\n",ESP8266_WaitReady(wireless_S));
			ESP8266_SendData(wireless_S, con, temp, strlen(temp));
			printf("Active Connection @ %d\n",i);
			printf("%d\n",ESP8266_WaitReady(wireless_S));
			//delay_ms(400);
			printf("Closing the connection %d\n", ESP8266_CloseConnection(wireless_S,con));
			printf("%d\n",ESP8266_WaitReady(wireless_S));
			reply[con->Number] = 0;
		}
	}
}



//Need to put initwifi into a while loop, potentially trying setting it to Accesspoint only mode.
uint16_t initHBServer(){
	
  ESP8266_APConfig_t ap_s;
  ap_s.SSID = NETWORK_SSID;
  ap_s.Pass = NETWORK_PWD;
  ap_s.Ecn = ESP8266_Ecn_OPEN;
  ap_s.Channel = 2;
  ap_s.MaxConnections = 4; //Cannot be greater than 4
  ap_s.Hidden = 0;
  ESP8266_Result_t res;
	do{
		res=ESP8266_Init(wireless_S, 115200);
		printf("Initializing Wifi: %d \n",res);
		ESP8266_WaitReady(wireless_S);
	}while(res);

	do{
		res = ESP8266_SetMode(wireless_S,ESP8266_Mode_AP );
		ESP8266_WaitReady(wireless_S);
	}while(res);
	
	ESP8266_DELAYMS(wireless_S, 1000);
	res = ESP8266_WaitReady(wireless_S);
	
	do{
	  res = ESP8266_SetAP(wireless_S, &ap_s);
	  printf("Hosting a Wifi AccessPoint: %d\n", res);
	  ESP8266_WaitReady(wireless_S);
	}while(res);
  
	ESP8266_DELAYMS(wireless_S, 1000);
	uint16_t port = HB_PORT;
	do{
		res = ESP8266_ServerEnable(wireless_S, port);
		printf("Hosting a server at port %d: Success = 0: %d\n", port, res);
		ESP8266_WaitReady(wireless_S);
	} while(res);
	
	return res;
}

void dataReply(){
	
}
void initDataServer(){
	printf("Being Called Right Now \n");
	ESP8266_Result_t temp = 1;
	do{
		temp=ESP8266_Init(wireless_S, 115200);
		printf("Initializing Wifi: %d \n",temp);
		ESP8266_WaitReady(wireless_S);
	}while(temp);
	
	do{
		temp = ESP8266_SetMode(wireless_S,ESP8266_Mode_STA );
		ESP8266_WaitReady(wireless_S);
	}while(temp);
	do {
		ESP8266_WaitReady(wireless_S);
		temp = ESP8266_WifiConnect(wireless_S, NETWORK_SSID, "");
		printf("The SSID is: - %s, %d \n", NETWORK_SSID, temp);
	}while (temp);



	do {
		ESP8266_WaitReady(wireless_S);
		ESP8266_DELAYMS(wireless_S, 1000);
		uint16_t port = SEC_PORT;
		temp = ESP8266_ServerEnable(wireless_S, port);
		printf("Hosting a server at port %d: Success = 0: %d\n", port, temp);
	} while(temp);

	do{
		ESP8266_WaitReady(wireless_S);  ESP8266_DELAYMS(wireless_S, 1000);
		temp =ESP8266_GetSTAIP(wireless_S);
		ESP8266_WaitReady(wireless_S);  ESP8266_DELAYMS(wireless_S, 1000);	
	}while(temp);
}

// Call back functions that we must impliment


/**
 * \brief  Device is ready callback
 *         
 *         Function is called when device has ready string sent to stack
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_DeviceReady(ESP8266_t* ESP8266){
	printf("Ready\n");
}
 
/**
 * \brief  Watchdog reset detected on device
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WatchdogReset(ESP8266_t* ESP8266){
	printf("Watchdog reset \n");
}
 
/**
 * \brief  Device has disconnected from wifi network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiDisconnected(ESP8266_t* ESP8266){
	printf("WIFI Disconnect\n");
}

/**
 * \brief  Device has connected to wifi network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiConnected(ESP8266_t* ESP8266){
	printf("Wifi Connected %d.%d.%d.%d \n", ESP8266->STAIP[0],ESP8266->STAIP[1],ESP8266->STAIP[2],ESP8266->STAIP[3]);
	
}

/**
 * \brief  Device did not succeed with connection to wifi network
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiConnectFailed(ESP8266_t* ESP8266){
	printf("Wifi connection Failed\n");
	
}

/**
 * \brief  Device has received IP address as station (when connected to another access point) from connected access point (AP)
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiGotIP(ESP8266_t* ESP8266){
	printf("Received IP Address %hu.%hu.%hu.%hu\n",ESP8266->STAIP[0],ESP8266->STAIP[1],ESP8266->STAIP[2],ESP8266->STAIP[3]);
}
 
/**
 * \brief  Device has received station IP.
 * \note   Function is called in case you try to get IP with \ref ESP8266_GetSTAIP function
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiIPSet(ESP8266_t* ESP8266){
	 printf("Attempted to assign a static IP address\n");
}

/**
 * \brief  Device failed to retrieve IP address via DHCP
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_DHCPTimeout(ESP8266_t* ESP8266){
	printf("DHCP IP Address timed out\n");
}

/**
 * \brief  Device has detected wifi access point where we can connect to.
 * \note   Function is called when you use \ref ESP8266_ListWifiStations function
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *ESP8266_AP: Pointer to \ref ESP8266_APs_t structure with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_WifiDetected(ESP8266_t* ESP8266, ESP8266_APs_t* ESP8266_AP){
	printf("Detecting Wifi networks\n");
}

/**
 * \brief  ESP8266 has a new connection active, acting like server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionActive(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	numConnections++;
	printf("Received a new Server Connection @ %d\n", Connection->Number);
	timed_out[Connection->Number] = 0;
	//ESP8266_WaitReady(ESP8266);
	//printf("ESP is ready again\n");
}

/**
 * \brief  ESP8266 connection closed, acting like server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	numConnections--;
	printf("TCP Connection end of data stream\n");
	
}

/**
 * \brief  ESP8266 has a data received on active connection when acting like server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \param  *Buffer: Buffer with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionDataReceived(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer){
		timed_out[Connection->Number]=0;
		printf("Server received data%d\n %s", strlen(Buffer), Buffer);
		printf("Connection: %d.%d.%d.%d : %d\n",Connection->RemoteIP[0],Connection->RemoteIP[1],Connection->RemoteIP[2],Connection->RemoteIP[3], Connection->RemotePort );
		//printf("Connection @%d\n", Connection->Number);
		reply[Connection->Number] =3;
		char * stringStart =0;
		stringStart = strstr(Buffer, GET_HOMEPAGE);
		printf("STring STart %d\n", stringStart);
		if(stringStart){
			reply[Connection->Number] = 1;
		}
		stringStart = strstr(Buffer, SET_SP);
		printf("STring STart %d : \n ", stringStart);
		if(stringStart){
			reply[Connection->Number] = 2;
					float temp = 0.0;
					char * stringEnd;
					temp = strtof(stringStart +strlen(SET_SP), &stringEnd);
					if((temp>0) && (temp<100))
						*setpoint = temp;
					printf("NEW SETPOINT = %f, %f\n", temp, *setpoint);
		}
		//ESP8266_WaitReady(ESP8266);

		//ESP8266_RequestSendData(ESP8266, Connection);
}

/**
 * \brief  ESP8266 is ready to accept data to be sent when connection is active as server
 * \note   This function is called in case \ref ESP8266_RequestSendData is called by user
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \param  *Buffer: Pointer to buffer where data should be stored
 * \param  max_buffer_size: Buffer size in units of bytes
 * \retval Number of bytes written into buffer
 * \note   With weak parameter to prevent link errors if not defined by user
 */
uint16_t ESP8266_Callback_ServerConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size){
	printf("this function is called when a client requests data from a server. This is when we would send the temperature\n");
	uint16_t numBytes = 10;
	char temp[10] = "1234567890";
	for(uint16_t i = 0; i < numBytes; i++)
		Buffer[i] = temp[i];

	return numBytes;
}

void ESP8266_Callback_Connection_Timeout(ESP8266_t * ESP8266, ESP8266_Connection_t * Connection){
	printf("Connection Timedout!!!: %d\n", Connection->Number);
	ESP8266->ActiveCommand = 0;
	(ESP8266->IPD).InIPD = 0;
	timed_out[Connection->Number] = 1;
}

/**
 * \brief  ESP8266 has successfully sent data for active connection
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("The server just sent Data\n");
}

/**
 * \brief  ESP8266 has not sent data for active connection
 * \note   When this happen, you can use \ref ESP8266_RequestSendData again to request new data sent
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Server Failed to send Data\n");
}

/**
 * \brief  Connection is active when ESP8266 starts new connection using \ref ESP8266_StartClientConnection
 * \note   When this function is called, use \ref ESP8266_RequestSendData if you want to send any data to connection
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionConnected(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Client successfully connected to a server");
}

/**
 * \brief  ESP826 returns error when trying to connect to external server as client
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Client connection error\n");
}

/**
 * \brief  ESP826 has not return any response in defined amount of time when connection to external server as client
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionTimeout(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Client connection timed out - probably should retry them stop\n ");
}

/**
 * \brief  Connection as client has been successfully closed
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Client connection Closed successfully\n");
}

/**
 * \brief  ESP8266 is ready to accept data to be sent when connection is active as client
 * \note   This function is called in case \ref ESP8266_RequestSendData is called by user
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \param  *Buffer: Pointer to buffer where data should be stored
 * \param  max_buffer_size: Buffer size in units of bytes
 * \retval Number of bytes written into buffer
 * \note   With weak parameter to prevent link errors if not defined by user
 */
uint16_t ESP8266_Callback_ClientConnectionSendData(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer, uint16_t max_buffer_size){
	printf("The client has been requested to send data. We need to reply with stuff in the buffer?\n");
	uint16_t numBytes = 0;
	return numBytes;
}

/**
 * \brief  ESP8266 has successfully sent data for active connection as client
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionDataSent(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Client sent Data!\n");
}
/**
 * \brief  ESP8266 has not sent data for active connection as client
 * \note   When this happen, you can use \ref ESP8266_RequestSendData again to request new data sent
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionDataSentError(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Client failed to send data\n");
}

/**
 * \brief  ESP8266 received network data and sends it to microcontroller. Function is called when when entire package of data is parsed
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection
 * \param  *Buffer: Buffer with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ClientConnectionDataReceived(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection, char* Buffer){
	printf("Client received data\n %s\n", Buffer);
}

//Ignoring ping parameters. This is used to send ping requests
/**
 * \brief  Pinging to external server has started
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *address: Pointer to address string where ping started
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
//void ESP8266_Callback_PingStarted(ESP8266_t* ESP8266, const char* address);

/**
 * \brief  Pinging to external server has started
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Pinging: Pointer to \ref ESP8266_Ping_t structure with information
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
//void ESP8266_Callback_PingFinished(ESP8266_t* ESP8266, ESP8266_Ping_t* Pinging);


//Ignoring firmware update as that will not be used
/**
 * \brief  Firmware update status checking
 * \note   You must use \ref ESP8266_FirmwareUpdate function to start updating
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  status: Called with current status about updating. This parameter can be a value of \ref ESP8266_FirmwareUpdate_t enumeration
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
//void ESP8266_Callback_FirmwareUpdateStatus(ESP8266_t* ESP8266, ESP8266_FirmwareUpdate_t status);

/**
 * \brief  Firmware update has been successful
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
//void ESP8266_Callback_FirmwareUpdateSuccess(ESP8266_t* ESP8266);

/**
 * \brief  Firmware update has failed
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
//void ESP8266_Callback_FirmwareUpdateError(ESP8266_t* ESP8266);

/**
 * \brief  ESP8266 returns new data about connected stations to our softAP
 * \note   This function is called in case \ref ESP8266_GetConnectedStations is used for detection connected stations
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Stations: Pointer to \ref ESP8266_ConnectedStations_t structure with data
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ConnectedStationsDetected(ESP8266_t* ESP8266, ESP8266_ConnectedStations_t* Stations){
	printf("Get Connected STations called. Not sure when this is used\n");
}
/**
 * \brief  ESP8266 returns new data about connected stations to our softAP
 * \note   This function is called in case \ref ESP8266_SNTPGetDateTime is used for current date time detection and error is returned
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *SNTP: Pointer to \ref ESP8266_SNTP_t structure with data from response
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_SNTPOk(ESP8266_t* ESP8266, ESP8266_SNTP_t* SNTP){
	printf("SNTP Success\n");
}

/**
 * \brief  ESP8266 returns new data about connected stations to our softAP
 * \note   This function is called in case \ref ESP8266_SNTPGetDateTime is used for current date time detection and error is returned
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_SNTPError(ESP8266_t* ESP8266){
	printf("SNTP Failure\n");
}
