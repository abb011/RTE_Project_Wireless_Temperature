#include "wireless.h"

void TM_USART1_ReceiveHandler(uint8_t c) {
//Do your stuff here when byte is received
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
	printf("Wifi Connected\n");
	
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
	printf("Received a new Server Connection\n");
}

/**
 * \brief  ESP8266 connection closed, acting like server
 * \param  *ESP8266: Pointer to working \ref ESP8266_t structure
 * \param  *Connection: Pointer to \ref ESP8266_Connection_t connection 
 * \retval None
 * \note   With weak parameter to prevent link errors if not defined by user
 */
void ESP8266_Callback_ServerConnectionClosed(ESP8266_t* ESP8266, ESP8266_Connection_t* Connection){
	printf("Server closed not sure if we closed it or just client disconnected");
	
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
		printf("Server received data\n %s\n", Buffer);
		
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
	uint16_t numBytes = 0;
	return numBytes;
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
