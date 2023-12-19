#include "RN487xH.h"
#include "RN487x.h"


	RN487x_hardwareInterface *hardwareInterface; // define this structure yourself, see RN487xH.c/h.
	char commandModeCaracter;
	uint8_t ServerOrClientMode; // 0 = server, 1 = client;
	BleutoothSerivce publicServices[4];
	BleutoothSerivce privateServices[8];
	
	
void BleutoothCharacteristics_clear(BleutoothCharacteristics *dv){
	for(unsigned int i = 0; i < UUID_SIZE; i++){
		dv->uuid[i] = 0;
	}
	dv->flag = 0;
	dv->handle = 0;
	dv->size = 0;
	dv->data = null;
}


void BleutoothSerivce_clear(BleutoothSerivce *dv){
	for(unsigned int i = 0; i < UUID_SIZE; i++){
		dv->uuid[i] = 0;
	}
	for(unsigned int i = 0; i<SERVICE_CHARACTERISTICS_NUMBER; i++){
		BleutoothCharacteristics_clear(&dv->characteristic[i]);
	}
}


RN487x_Error RN487x_init(RN487x *dv, RN487x_hardwareInterface *hardwareLink){
	RN487x_Error err;
	dv->hardwareInterface = hardwareLink;
	
	//Clear services
	for(unsigned int i = 0; i<PRIVATE_SERVICE_NUMBER; i++){
		BleutoothSerivce_clear(&dv->privateServices[i]);
	}
	for(unsigned int i = 0; i<PUBLIC_SERVICE_NUMBER; i++){
		BleutoothSerivce_clear(&dv->publicServices[i]);
	}
	
	RN487xH_uartClear(dv->hardwareInterface);
	
	dv->standardTimeOutMs = 200;
	dv->ServerOrClientMode = 0; // deflaut si server mode;
	//Enable command mode
	dv->commandModeCaracter = '$';
	
	err = RN487x_commandMode(dv); 
	if(err != RN487x_ok) 
		return err;
	
	err = RN487x_setAppearanceId(0x0556); // sensor = 0x0540, multisensor = 0x0556
	if(err != RN487x_ok) 
		return err;
}

RN487x_PinStatus RN487x_getStatus(RN487x *dv){
	return 	(RN487x_PinStatus)( RN487xH_readPinStatus1(dv->hardwareInterface) | 
							   (RN487xH_readPinStatus2(dv->hardwareInterface)<<1)
							  )

}


RN487x_Error RN487x_replyChecker(RN487x *dv, char *validReply, char *errorReply, uint32_t timeOutMs){
	
	if(RN487xH_uartRead(dv->hardwareInterface, dv->buffTmp, timeOutMs))#
	{
		//timeOut error
		return RN487x_timeOut;
	}
	//Valid reply
	if (!strcmp(validReply, dv->buffTmp)){
		return RN487x_ok;
	}
	//Error reply
	if (!strcmp(errorReply, dv->buffTmp)){
		return RN487x_errorResponse;
	}
	else{
		return RN487x_badResponse;
	}
	return RN487x_badResponse; 
}	


/* S-,<string>
	This command sets a serialized Bluetooth name for the device, where <name> is up to
	15 alphanumeric characters. This command automatically appends the last two bytes
	of the Bluetooth MAC address to the name which is useful for generating a custom
	name with unique numbering.This command does not have corresponding Get command
 */
RN487x_Error RN487x_setSerializedDeviceName(RN487x *dv, char *name){
	if(strlen(name)=>15) 
		name[15] = 0;
	sprinf(dv->buffTmp,"S-,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* S$,<char>
	This command sets the Command mode character, where <car> is a single character
	in the three character pattern. This setting enables the user to change the default char-
	acter to enter Command mode ($$$) to another character string. Restoring the factory
	defaults returns the device to use $$$
 */
RN487x_Error RN487x_setCommandModeCaracter(RN487x *dv, char car){
	sprinf(dv->buffTmp,"S$,%c\r",car);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}


/* S%,<pre>,<post>
	This command sets the pre and post delimiter of the status string from RN4870/71 to
	the host controller. The pre and post delimiter are up to four printable ASCII characters.
	If no parameter is given to the post delimiter, then the post delimiter is cleared; if no
	parameter is given to the pre-delimiter, then both pre and post delimiters are cleared
*/
RN487x_Error RN487x_setPrePostDelimCarStatuString(RN487x *dv, char *pre, char *post){
	if(strlen(pre)=>4) 
		pre[4] = 0;
	if(strlen(post)=>4) 
		post[4] = 0;
	sprinf(dv->buffTmp,"S%c,<%s,%s>\r",0x25, pre, post); // verify < and >
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* A,<0-5>
The Set Authentication command configures RN4870/71 Input/Output (I/O) capability
which sets the authentication method to use when securing the Bluetooth Low Energy
link. The options for the command parameter are described in Table 2-2.
Once a remote device exchanges PIN codes with the RN4870/71 device, a link key is
stored for future authentication. The device automatically stores authentication
information for up to eight peer devices in Non-Volatile Memory. If the bonded device
table is filled with eight entries and a ninth entry to be added, then the ninth entry
replaces the first entry on the table. If any particular entry in the bonded device table is
deleted, then a new entry to the table will take the place of the deleted entry.
*/
RN487x_Error RN487x_setAuthenticationMode(RN487x *dv, RN487x_AuthenticationMode mode){
	sprinf(dv->buffTmp,"SA,%u\r",(unint8_t)mode); // verify < and >
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SB,<H8>
This command sets the baud rate of the UART communication. The input parameter is
an 8-bit hex value in the range of 00 to 0B, representing baud rate from 2400 to 921K,
as shown in Table 2-3
*/
const uint8_t RN487xBaudArray[] = {921600, 460800, 230400, 115200, 57600, 38400, 28800, 19200, 14400, 9600, 4800, 2400};
RN487x_Error RN487x_setBaudRate(RN487x *dv, RN487x_BaudRate bd){
	sprinf(dv->buffTmp,"SB,%02u\r",(unint8_t)bd); // verify < and >
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	RN487xH_uartSetBaudRate(dv->hardwareInterface,RN487xBaudArray[(unint8_t)bd]);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SC,<0-2>
This command configures the connectable advertisement and non-connectable/bea-
con advertisement settings. It expects one single digit input parameter as described in
Table 2-4. The beacon feature enables non-connectable advertisement. The
RN4870/71 has the ability to advertise connectable advertisement and
non-connectable beacon advertisement in a tandem switching manner when the SC,2
is used.
*/

/* SDA,<H16>
This command sets the appearance of RN4870/71 in GAP service. It expects one
16-bit hex input parameter. Bluetooth SIG defines the appearance code for different
devices. Please refer to Bluetooth SIG website for details:
*/

RN487x_Error RN487x_setAppearanceId (RN487x *dv, uint16_t id){
	sprinf(dv->buffTmp,"SDA,%04u\r",id); 
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}; 

/* SDF,<text>
This command sets the value of the firmware revision characteristic in the Device Infor-
mation Service. This command is only effective if the Device Information service is
enabled by command SS.
The Device Information Service is used to identify the device. All its characteristics
rarely change. Therefore, values of characteristics in the Device Information Service
are set and saved into NVM. All values of characteristics in the Device Information
Service have a maximum size of 20 bytes
*/
RN487x_Error RN487x_setFirmwareVersion (RN487x *dv, char *ver){
	if(strlen(ver)=>20) 
		ver[20] = 0;
	sprinf(dv->buffTmp,"SDF,%s\r",ver);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SDH,<text>
This command sets the value of the hardware revision characteristics in the Device
Information Service. This command is only effective if the Device Information Service
is enabled by command SS.
*/
RN487x_Error RN487x_setHardwareRevison (RN487x *dv, char *rev){
	if(strlen(ver)=>20) 
		ver[20] = 0;
	sprinf(dv->buffTmp,"SDH,%s\r",rev);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}
RN487x_Error RN487x_setModelName (RN487x *dv, char *name);
RN487x_Error RN487x_setManufacturerName (RN487x *dv, char *name);
RN487x_Error RN487x_setSoftwareRevision (RN487x *dv, char *rev);
RN487x_Error RN487x_setSerialNumber (RN487x *dv, char *sn);

RN487x_Error RN487x_factoryConfiguration(RN487x *dv, uint8_t clearServices);

RN487x_Error RN487x_setOuputPowerAdvertise(RN487x *dv, RN487x_OutputPower power);
RN487x_Error RN487x_setOuputPowerConnected(RN487x *dv, RN487x_OutputPower power);


RN487x_Error RN487x_setDeviceName(RN487x *dv, char *name); //  20max
RN487x_Error RN487x_setLowPowerMode(RN487x *dv, uint8_t powerMode); //  0 = normal mode, 1 = low power

RN487x_Error RN487x_setSecurityPin(RN487x *dv, uint16_t securityPin); 

RN487x_Error RN487x_setDeviceConfiguration(RN487x *dv, uint16_t configuration);
RN487x_Error RN487x_setDefaultService(RN487x *dv, uint8_t services);
RN487x_Error RN487x_setCentralConectionParameters(RN487x *dv, uint16_t minInterval, uint16_t maxInterval, uint16_t latency, uint16_t timeOut);
RN487x_Error RN487x_setAdvertiseIntervalA(RN487x *dv, uint16_t fastInterval, uint16_t fastTimeOut, uint16_t slowInterval); 
RN487x_Error RN487x_setAdvertiseIntervalB(RN487x *dv, uint16_t beaconInterval); 


/// --------------------------------------------------- Getters --------------------------------------///

RN487x_Error RN487x_getConnectionStatus(RN487x *dv, char* status);
RN487x_Error RN487x_getPeerDevName(RN487x *dv, char* name);

RN487x_Error RN487x_gsetSerializedDeviceName(RN487x *dv, char *name);
RN487x_Error RN487x_getCommandModeCaracter(RN487x *dv, char *car);
RN487x_Error RN487x_getPrePostDelimCarStatuString(RN487x *dv, char *pre, char *post); // max 4 car 
RN487x_Error RN487x_getAuthenticationMode(RN487x *dv, RN487x_AuthenticationMode *mode);

/// Device information setter ///
RN487x_Error RN487x_getAppearanceId (RN487x *dv, uint16_t *id); // sensor = 0x0540, multisensor = 0x0556
RN487x_Error RN487x_getFirmwareVersion (RN487x *dv, char *ver);
RN487x_Error RN487x_getHardwareRevison (RN487x *dv, char *rev);
RN487x_Error RN487x_getModelName (RN487x *dv, char *name);
RN487x_Error RN487x_getManufacturerName (RN487x *dv, char *name);
RN487x_Error RN487x_getSoftwareRevision (RN487x *dv, char *rev);
RN487x_Error RN487x_getSerialNumber (RN487x *dv, char *sn);

RN487x_Error RN487x_getOuputPowerAdvertise(RN487x *dv, RN487x_OutputPower *power);
RN487x_Error RN487x_getOuputPowerConnected(RN487x *dv, RN487x_OutputPower *power);

RN487x_Error RN487x_getBaudRate(RN487x *dv, uint32_t *bd);
RN487x_Error RN487x_getDeviceName(RN487x *dv, char *name); //  20max
RN487x_Error RN487x_getLowPowerMode(RN487x *dv, uint8_t *powerMode); //  0 = normal mode, 1 = low power

RN487x_Error RN487x_getSecurityPin(RN487x *dv, uint16_t *securityPin); 

RN487x_Error RN487x_getDeviceConfiguration(RN487x *dv, uint16_t *configuration);
RN487x_Error RN487x_getDefaultService(RN487x *dv, uint8_t *services);
RN487x_Error RN487x_getCentralConectionParameters(RN487x *dv, uint16_t *minInterval, uint16_t *maxInterval, uint16_t *latency, uint16_t *timeOut);
RN487x_Error RN487x_getAdvertiseIntervalA(RN487x *dv, uint16_t *fastInterval, uint16_t *fastTimeOut, uint16_t *slowInterval); 
RN487x_Error RN487x_getAdvertiseIntervalB(RN487x *dv, uint16_t *beaconInterval); 



/// ------------------------------------------ Action command ------------------------------------------------ ///

RN487x_Error RN487x_commandMode(RN487x *dv);
RN487x_Error RN487x_quitCommandMode(RN487x *dv);
RN487x_Error RN487x_enterRemoteMode(RN487x *dv);
RN487x_Error RN487x_exitRemoteMode(RN487x *dv);
RN487x_Error RN487x_writeIO(RN487x *dv, RN487x_IOmask mask, uint8_t value);
RN487x_Error RN487x_startAdvertise(RN487x *dv, uint16_t fastInterval, uint8_t slowInterval);
RN487x_Error RN487x_bondDevice(RN487x *dv);
RN487x_Error RN487x_connectLastBondedDevice(RN487x *dv);
RN487x_Error RN487x_connectBondedDevice(RN487x *dv, uint8_t bondId);
RN487x_Error RN487x_connectByAdress(RN487x *dv, uint8_t publicOrPrivate, uint64_t adress); // public = 0, private = 1
RN487x_Error RN487x_startUartTransparentMode(RN487x *dv); // public = 0, private = 1
RN487x_Error RN487x_clearAdvertiseContent(RN487x *dv); // public = 0, private = 1
RN487x_Error RN487x_appendAdvertiseContent(RN487x *dv, uint8_t flag, uint8_t *data, uint32_t size); // public = 0, private = 1
RN487x_Error RN487x_whiteListMACadress(RN487x *dv, uint8_t publicOrPrivate, uint64_t adress);
RN487x_Error RN487x_whiteListBondedDevice(RN487x *dv);
RN487x_Error RN487x_clearWhiteListBondedDevice(RN487x *dv);
RN487x_Error RN487x_readWhiteList(RN487x *dv, char *data);
RN487x_Error RN487x_disconnect(RN487x *dv);
RN487x_Error RN487x_getSignalStreng(RN487x *dv, float *streng);
RN487x_Error RN487x_reboot(RN487x *dv);
RN487x_Error RN487x_clearBonding(RN487x *dv);

/// ------------------------------------------ List command ------------------------------------------------------ ///

RN487x_Error RN487x_listBondedDevice(RN487x *dv, char *device);
RN487x_Error RN487x_listServices(RN487x *dv);
RN487x_Error RN487x_startClientOperation(RN487x *dv);


/// ------------------------------------------ Services command ------------------------------------------------- ///

RN487x_Error RN487x_createServerServices(RN487x *dv, BleutoothSerivce *service);
RN487x_Error RN487x_writeCaracteristic(BleutoothCharacteristics *caracteristic);
RN487x_Error RN487x_readCaracteristic(BleutoothCharacteristics *caracteristic);

