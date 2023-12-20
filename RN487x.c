#include "RN487xH.h"
#include "RN487x.h"


	RN487x_hardwareInterface *hardwareInterface; // define this structure yourself, see RN487xH.c/h.
	char commandModeCaracter;
	uint8_t ServerOrClientMode; // 0 = server, 1 = client;
	BleutoothSerivce publicServices[4];
	BleutoothSerivce privateServices[8];
	
void cpyAndRemovreCR(char *source, char *dest, uint32_t maxLen){
	uint32_t noSize = (maxLen)? 0:1;
	while(maxLen || noSize){
		if(*source){
			if(*source != '\r'){
				*dest = *source;
				dest++;
				source++;
			}
			maxLen--;
		}else{
			return;
		}
	}
}	
	
	
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
	
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, timeOutMs))
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
	sprintf(dv->buffTmp,"S-,%s\r",name);
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
	sprintf(dv->buffTmp,"S$,%c\r",car);
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
	sprintf(dv->buffTmp,"S%c,<%s,%s>\r",0x25, pre, post); // verify < and >
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
	sprintf(dv->buffTmp,"SA,%u\r",(unint8_t)mode); 
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
	sprintf(dv->buffTmp,"SB,%02x\r",(unint8_t)bd);
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
	sprintf(dv->buffTmp,"SDA,%04u\r",id); 
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
	sprintf(dv->buffTmp,"SDF,%s\r",ver);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SDH,<text>
This command sets the value of the hardware revision characteristics in the Device
Information Service. This command is only effective if the Device Information Service
is enabled by command SS.
*/
RN487x_Error RN487x_setHardwareRevison (RN487x *dv, char *rev){
	sprintf(dv->buffTmp,"SDH,%s\r",rev);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SDM,<text>
This command sets the model name characteristics in the Device Information Service.
This command is only effective if the Device Information Service is enabled by
command SS.
*/
RN487x_Error RN487x_setModelName (RN487x *dv, char *name){
	sprintf(dv->buffTmp,"SDM,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SDN,<text>
This command sets the manufacturer name characteristics in the Device Information
Service. This command is only effective if the Device Information service is enabled by
command SS.
*/
RN487x_Error RN487x_setManufacturerName (RN487x *dv, char *name){
	sprintf(dv->buffTmp,"SDN,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SDR,<text>
This command sets software revision in the Device Information Service. This command
is only effective if the Device Information Service is enabled by command SS.
*/
RN487x_Error RN487x_setSoftwareRevision (RN487x *dv, char *rev){
	sprintf(dv->buffTmp,"SDR,%s\r",rev);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SDS,<text>
This command sets the value of serial number characteristics in the Device Information
Service. This command is only effective if the Device Information Service is enabled by
command SS.
*/
RN487x_Error RN487x_setSerialNumber (RN487x *dv, char *sn){
	sprintf(dv->buffTmp,"SDS,%s\r",sn);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}


/* SF,1
This command resets the configurations into factory default. The first parameter must
be set to 1. This command does not delete the private service and characteristics cre-
ated using PS and PC commands and the script created using the Embedded Scripting
Feature.
*/
RN487x_Error RN487x_factoryConfiguration(RN487x *dv, uint8_t clearServices){
	if(clearServices){
		RN487xH_uartWrite(dv->hardwareInterface, "SF,2\r");
	}else{
		RN487xH_uartWrite(dv->hardwareInterface, "SF,1\r");
	}
	return RN487x_replyChecker(dv, "Reboot after Factory Reset\r", "ERR\r", dv->standardTimeOutMs);
}


/* SGA,<0-5>
Command SGA  adjust the output power of RN4870/71 under advertisement. 
These commands expect a single digit as input
parameter which can range from 0 to 5, where 0 represents highest power output and
5 lowest power output. The approximate output power (in dBm) for each parameter
value is provided in Table 2-5. There can be a variation in output power based on the
individual calibration of the module and the enclosure in which the module is placed
*/
RN487x_Error RN487x_setOuputPowerAdvertise(RN487x *dv, RN487x_OutputPower power){
	sprintf(dv->buffTmp,"SGA,%u\r",(unit8_t)power);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SGC,<0-5>
Command SGC  adjust the output power of RN4870/71 under connecting. 
These commands expect a single digit as input
parameter which can range from 0 to 5, where 0 represents highest power output and
5 lowest power output. The approximate output power (in dBm) for each parameter
value is provided in Table 2-5. There can be a variation in output power based on the
individual calibration of the module and the enclosure in which the module is placed
*/
RN487x_Error RN487x_setOuputPowerConnected(RN487x *dv, RN487x_OutputPower power){
	sprintf(dv->buffTmp,"SGC,%u\r",(unit8_t)power);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SN,<text>
This command sets the device name, where <text> is up to 20 alphanumeric
characters.
*/
RN487x_Error RN487x_setDeviceName(RN487x *dv, char *name){
	if(strlen(name)=>20) 
		name[20] = 0;
	sprintf(dv->buffTmp,"SN,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}

/* SO,<0,1>
Command SO enables or disables low-power operation of RN4870/71. It expects one
single digit as input parameter.
If the input parameter is 0, then RN4870/71 runs 16 MHz clock all the time, therefore,
can operate UART all the time. On the other hand, if the input parameter is 1, then
RN4870 enables Low-Power mode by running 32 kHz clock with much lower power
consumption. When RN4870 runs on 32 kHz clock, UART is not operational.
RN4870/71 restarts 16 MHz clock by pulling UART_RX_IND pin low. When UART_RX-
_IND pin is high, RN4870/71 runs 32 kHz clock. When RN4870/71 runs on 32 kHz
clock, a BLE connection can still be maintained, but UART cannot receive data. If the
user sends input data to the UART, UART_RX_IND pin must be pulled low to start 16
MHz clock, then wait for 5 ms to operate UART again.
*/
RN487x_Error RN487x_setLowPowerMode(RN487x *dv, uint8_t powerMode){
	sprintf(dv->buffTmp,"SO,%u\r",(unit8_t)powerMode);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}


/* SP,<4/6 digit pin>
This command sets the fixed security PIN code. The fixed PIN code has two
functionalities:
• If the fixed PIN is a six-digit code, it is used to display when I/O capability is set to
Display Only by command. The six-digit PIN is used for Simple Secure Pairing
(SSP) authentication method in BLE if a fixed passkey is desirable. In this way,
RN4870 is not required to display the passkey if the remote peer already knows
the passkey. The user must understand the security implication by using the fixed
passkey.
• The four digit PIN code option is used to authenticate remote command connec-
tion. For more details on remote command feature, refer to command ! (2.4.4).
*/
RN487x_Error RN487x_setSecurityPin(RN487x *dv, uint16_t securityPin, uint8_t pinDigitCount){
	if(pinDigitCount == 6){
		sprintf(dv->buffTmp,"SP,%06u\r",securityPin);
	}else{
		sprintf(dv->buffTmp,"SP,%04u\r",securityPin);
	}
	
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);	
}

/* SR,<hex16>
This command sets the supported feature of the RN4870 device. The input parameter
is a 16-bit bitmap that indicates the features supported. After changing the features, a
reboot is necessary to make the changes effective. Table 2-6 shows the bitmap of
features.
*/
RN487x_Error RN487x_setDeviceConfiguration(RN487x *dv, uint16_t bitmap){
	sprintf(dv->buffTmp,"SR,%04x\r",(unit8_t)bitmap);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);	
}
/*
Set bit of SR register.
*/
RN487x_Error RN487x_setBitDeviceConfiguration(RN487x *dv, RN487x_ConfigMask bitMask){
	uint16_t oldBitmap;
	RN487x_Error err;
	
	 err = RN487x_getDeviceConfiguration(RN487x *dv, &oldBitmap);
	 if(err != RN487x_ok){
		 return err;
	 }
	 return RN487x_setDeviceConfiguration(dv, oldBitmap |= bit);
	 
}

/*
Reset bit of SR register.
*/
RN487x_Error RN487x_resetBitDeviceConfiguration(RN487x *dv, RN487x_ConfigMask bitMask){
	uint16_t oldBitmap;
	RN487x_Error err;
	
	 err = RN487x_getDeviceConfiguration(RN487x *dv, &oldBitmap);
	 if(err != RN487x_ok){
		 return err;
	 }
	 return RN487x_setDeviceConfiguration(dv, oldBitmap &= ~bit);
}



/* SS,<hex8>
This command sets the default services to be supported by the RN4870 in the GAP
server role. The input parameter is an 8-bit bitmap that indicates the services to be
supported as a server. Supporting service in server role means that the host MCU must
supply the values of all characteristics in supported services and to provide client
access to those values upon request. Once the service bitmap is modified, the device
must reboot to make the new services effective. The 8-bit bitmap is listed in Table 2-7.
For information on Bluetooth Services visit https://developer.bluetooth.org/gatt/ser-
vices/Pages/ServicesHome.aspx.
*/
RN487x_Error RN487x_setDefaultService(RN487x *dv, uint8_t bitmap){
	sprintf(dv->buffTmp,"SS,%02x\r",(unit8_t)bitmap);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);	
}

/*
Set bit of Ss register.
*/
RN487x_Error RN487x_setBitDefaultServiceRN487x *dv, RN487x_ServicesMask bitMask){
	uint16_t oldBitmap;
	RN487x_Error err;
	
	 err = RN487x_getDefaultService(RN487x *dv, &oldBitmap);
	 if(err != RN487x_ok){
		 return err;
	 }
	 return RN487x_setDefaultService(dv, oldBitmap |= bit);
	 
}

/*
Reset bit of Ss register.
*/
RN487x_Error RN487x_getDefaultService(RN487x *dv, RN487x_ServicesMask bitMask){
	uint16_t oldBitmap;
	RN487x_Error err;
	
	 err = RN487x_getDeviceConfiguration(RN487x *dv, &oldBitmap);
	 if(err != RN487x_ok){
		 return err;
	 }
	 return RN487x_setDefaultService(dv, oldBitmap &= ~bit);
}

/* ST,<hex16>,<hex16>,<hex16>,<hex16>
This command sets the initial connection parameters of the central device for future
connections. The four input parameters are all 16-bit values in hex format. To modify
current connection parameters, refer to Action command T (2.6.29).
The corresponding Get command, GT, returns the desirable connection parameters set
by command ST when connection is not established. Once the connection is estab-
lished, the actual connection parameters displays in response to command GT.
Connection interval, latency and time-out are often associated with how frequently a
peripheral device must communicate with the central device, therefore, closely related
to power consumption. The parameters, range and description are listed in Table 2-8
*/
RN487x_Error RN487x_setCentralConectionParameters(RN487x *dv, uint16_t minInterval, uint16_t maxInterval, uint16_t latency, uint16_t timeOut){
	sprintf(dv->buffTmp,"ST,%04x,%04x,%04x,%04x\r",minInterval, maxInterval, latency, timeOut);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);	
}

/* STA,<hex16>,<hex16>,<hex16>
This command sets the advertisement interval and time-out parameters to connect
advertisements defined by the 'A', 'IA' and 'NA' commands. The three inputs are fast
advertisement interval, fast advertisement time-out, and slow advertisement interval,
respectively
The corresponding Get command, GTA, returns in the same order as follows, fast
advertisement interval, fast advertisement time-out, and slow advertisement interval.
The unit for fast and slow advertisement intervals unit is 0.625 ms. The fast advertise-
ment time-out unit is 10.24 seconds. All input parameters are in Hex format
*/
RN487x_Error RN487x_setAdvertiseIntervalA(RN487x *dv, uint16_t fastInterval, uint16_t fastTimeOut, uint16_t slowInterval){
	sprintf(dv->buffTmp,"STA,%04x,%04x,%04x\r",fastInterval, fastTimeOut, slowInterval);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);	
}

/*
This command sets the advertisement interval for beacons as defined by the 'IB' and
'NB' commands. The beacon advertisement interval parameter unit is 0.625 ms. The
corresponding Get command, GTB, returns the beacon advertisement interval.
*/
RN487x_Error RN487x_setAdvertiseIntervalB(RN487x *dv, uint16_t beaconInterval){
	sprintf(dv->buffTmp,"STB,%04x\r",beaconInterval);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_replyChecker(dv, "AOK\r", "ERR\r", dv->standardTimeOutMs);
}


/// --------------------------------------------------- Getters --------------------------------------///
/* GK
Command GK gets the current connection status. It expects no input parameter.
If the RN4870/71 is not connected, the output is none.
If the RN4870/71 is connected, command GK returns the following connection
information:
<Peer BT Address>,<Address Type>,<Connection Type>
where <Peer BT Address> is the 6-byte hex address of the peer device; <Address
Type> is either 0 for public address or 1 for random address. <Connection Type> spec-
ifies if the connection enables UART Transparent feature, where 1 indicates UART
Transparent is enabled and 0 indicates UART Transparent is disabled.
*/
RN487x_Error RN487x_getConnectionStatus(RN487x *dv, char* status){
	RN487xH_uartWrite(dv->hardwareInterface, "GK\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, status, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;
}

/* GNR
This command gets the peer device name when connected. If this command is issued
before a connection is established, an error message is the output
*/
RN487x_Error RN487x_getPeerDevName(RN487x *dv, char* name){
	RN487xH_uartWrite(dv->hardwareInterface, "GNR\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, name, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;	
}


RN487x_Error RN487x_gsetSerializedDeviceName(RN487x *dv, char *name){
	RN487xH_uartWrite(dv->hardwareInterface, "G-\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, name, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;		
}

RN487x_Error RN487x_getCommandModeCaracter(RN487x *dv, char *car){
	RN487xH_uartWrite(dv->hardwareInterface, "G$\r");
	if(RN487xH_uartReadChar(dv->hardwareInterface, car, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;		
}

RN487x_Error RN487x_getPrePostDelimCarStatuString(RN487x *dv, char *pre, char *post){
	RN487xH_uartWrite(dv->hardwareInterface, "G\x25\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	char *ptrTmp = strtok(dv->buffTmp, ',');
	strcpy(ptrTmp, pre);
	ptrTmp = strtok(null, ',');
	strcpy(ptrTmp, post);
	
	return RN487x_ok;		
}

RN487x_Error RN487x_getAuthenticationMode(RN487x *dv, RN487x_AuthenticationMode *mode){
	RN487xH_uartWrite(dv->hardwareInterface, "GA\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",mode);
	return RN487x_ok;
}

RN487x_Error RN487x_getAppearanceId (RN487x *dv, uint16_t *id){
	RN487xH_uartWrite(dv->hardwareInterface, "GDA\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",id);
	return RN487x_ok;	
}

RN487x_Error RN487x_getFirmwareVersion (RN487x *dv, char *ver){
	RN487xH_uartWrite(dv->hardwareInterface, "GDF\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, ver, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;	
}

RN487x_Error RN487x_getHardwareRevison (RN487x *dv, char *rev){
	RN487xH_uartWrite(dv->hardwareInterface, "GDH\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, rev, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;	
}

RN487x_Error RN487x_getModelName (RN487x *dv, char *name){
	RN487xH_uartWrite(dv->hardwareInterface, "GDM\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, name, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;
}

RN487x_Error RN487x_getManufacturerName (RN487x *dv, char *name){
	RN487xH_uartWrite(dv->hardwareInterface, "GDN\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, name, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;
}

RN487x_Error RN487x_getSoftwareRevision (RN487x *dv, char *rev){
	RN487xH_uartWrite(dv->hardwareInterface, "GDR\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, rev, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;
}

RN487x_Error RN487x_getSerialNumber (RN487x *dv, char *sn){
	RN487xH_uartWrite(dv->hardwareInterface, "GDS\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, sn, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;	
}

RN487x_Error RN487x_getOuputPowerAdvertise(RN487x *dv, RN487x_OutputPower *power){
	RN487xH_uartWrite(dv->hardwareInterface, "GGA\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",power);
	return RN487x_ok;	
}

RN487x_Error RN487x_getOuputPowerConnected(RN487x *dv, RN487x_OutputPower *power){
	RN487xH_uartWrite(dv->hardwareInterface, "GGC\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",power);
	return RN487x_ok;		
}

//RN487x_Error RN487x_getBaudRate(RN487x *dv, uint32_t *bd);
RN487x_Error RN487x_getDeviceName(RN487x *dv, char *name){
	RN487xH_uartWrite(dv->hardwareInterface, "GN\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, name, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	return RN487x_ok;	
}
/*
0 = normal mode, 1 = low power
*/ 
RN487x_Error RN487x_getLowPowerMode(RN487x *dv, uint8_t *powerMode){
	RN487xH_uartWrite(dv->hardwareInterface, "GO\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",powerMode);
	return RN487x_ok;		
}

RN487x_Error RN487x_getSecurityPin(RN487x *dv, uint16_t *securityPin){
	RN487xH_uartWrite(dv->hardwareInterface, "GP\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",securityPin);
	return RN487x_ok;			
}

RN487x_Error RN487x_getDeviceConfiguration(RN487x *dv, uint16_t *bitmap){
	RN487xH_uartWrite(dv->hardwareInterface, "GR\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",bitmap);
	return RN487x_ok;		
}

RN487x_Error RN487x_getDefaultService(RN487x *dv, uint8_t *bitmap){
	RN487xH_uartWrite(dv->hardwareInterface, "GS\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp,"%u",bitmap);
	return RN487x_ok;	
}

RN487x_Error RN487x_getCentralConectionParameters(RN487x *dv, uint16_t *minInterval, uint16_t *maxInterval, uint16_t *latency, uint16_t *timeOut){
	RN487xH_uartWrite(dv->hardwareInterface, "GT\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	char *ptrTmp = strtok(dv->buffTmp, ',');
	sscanf(ptrTmp, "%u", minInterval)
	ptrTmp = strtok(null, ',');
	sscanf(ptrTmp, "%u", maxInterval)
	ptrTmp = strtok(null, ',');
	sscanf(ptrTmp, "%u", latency)
	ptrTmp = strtok(null, ',');
	sscanf(ptrTmp, "%u", timeOut)
	
	return RN487x_ok;			
}

RN487x_Error RN487x_getAdvertiseIntervalA(RN487x *dv, uint16_t *fastInterval, uint16_t *fastTimeOut, uint16_t *slowInterval){
	RN487xH_uartWrite(dv->hardwareInterface, "GTA\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	char *ptrTmp = strtok(dv->buffTmp, ',');
	sscanf(ptrTmp, "%u", fastInterval)
	ptrTmp = strtok(null, ',');
	sscanf(ptrTmp, "%u", fastTimeOut)
	ptrTmp = strtok(null, ',');
	sscanf(ptrTmp, "%u", slowInterval)
	
	return RN487x_ok;		
}
RN487x_Error RN487x_getAdvertiseIntervalB(RN487x *dv, uint16_t *beaconInterval){
	RN487xH_uartWrite(dv->hardwareInterface, "GTA\r");
	if(RN487xH_uartReadLine(dv->hardwareInterface, dv->buffTmp, dv->standardTimeOutMs))
	{
		//timeOut error
		return RN487x_timeOut;
	}
	sscanf(dv->buffTmp)

	return RN487x_ok;	
}



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

