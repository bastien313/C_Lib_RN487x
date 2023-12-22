#include "RN487xH.h"
#include "RN487x.h"


	RN487x_hardwareInterface *hardwareInterface; // define this structure yourself, see RN487xH.c/h.
	char commandModeCaracter;
	uint8_t ServerOrClientMode; // 0 = server, 1 = client;
	BleutoothSerivce publicServices[4];
	BleutoothSerivce privateServices[8];
/*	
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
}	*/
	
	
void BleutoothCharacteristics_clear(BleutoothCharacteristics *dv){
	for(unsigned int i = 0; i < UUID_SIZE; i++){
		dv->uuid[i] = 0;
	}
	dv->flag = 0;
	dv->handle = 0;
	dv->size = 0;
	dv->data = null;
	dv->config = 0;
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
	dv->publicServicesCount = 0;
	dv->privateServicesCount = 0;
	
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


RN487x_Error RN487x_lineReplyChecker(RN487x *dv, char *validReply, char *errorReply, uint32_t timeOutMs){
	
	
	if(RN487xH_uartReadUntilStringDetected(dv->hardwareInterface,data, "\r", timeOutMs))
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
	RN487xH_uartClear(dv->hardwareInterface);
	if(strlen(name)=>15) 
		name[15] = 0;
	sprintf(dv->buffTmp,"S-,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* S$,<char>
	This command sets the Command mode character, where <car> is a single character
	in the three character pattern. This setting enables the user to change the default char-
	acter to enter Command mode ($$$) to another character string. Restoring the factory
	defaults returns the device to use $$$
 */
RN487x_Error RN487x_setCommandModeCaracter(RN487x *dv, char car){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"S$,%c\r",car);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}


/* S%,<pre>,<post>
	This command sets the pre and post delimiter of the status string from RN4870/71 to
	the host controller. The pre and post delimiter are up to four printable ASCII characters.
	If no parameter is given to the post delimiter, then the post delimiter is cleared; if no
	parameter is given to the pre-delimiter, then both pre and post delimiters are cleared
*/
RN487x_Error RN487x_setPrePostDelimCarStatuString(RN487x *dv, char *pre, char *post){
	RN487xH_uartClear(dv->hardwareInterface);
	if(strlen(pre)=>4) 
		pre[4] = 0;
	if(strlen(post)=>4) 
		post[4] = 0;
	sprintf(dv->buffTmp,"S%c,<%s,%s>\r",0x25, pre, post); // verify < and >
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SA,%u\r",(unint8_t)mode); 
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* SB,<H8>
This command sets the baud rate of the UART communication. The input parameter is
an 8-bit hex value in the range of 00 to 0B, representing baud rate from 2400 to 921K,
as shown in Table 2-3
*/
const uint8_t RN487xBaudArray[] = {921600, 460800, 230400, 115200, 57600, 38400, 28800, 19200, 14400, 9600, 4800, 2400};
RN487x_Error RN487x_setBaudRate(RN487x *dv, RN487x_BaudRate bd){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SB,%02x\r",(unint8_t)bd);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	RN487xH_uartSetBaudRate(dv->hardwareInterface,RN487xBaudArray[(unint8_t)bd]);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SDA,%04u\r",id); 
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	if(strlen(ver)=>20) 
		ver[20] = 0;
	sprintf(dv->buffTmp,"SDF,%s\r",ver);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* SDH,<text>
This command sets the value of the hardware revision characteristics in the Device
Information Service. This command is only effective if the Device Information Service
is enabled by command SS.
*/
RN487x_Error RN487x_setHardwareRevison (RN487x *dv, char *rev){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SDH,%s\r",rev);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* SDM,<text>
This command sets the model name characteristics in the Device Information Service.
This command is only effective if the Device Information Service is enabled by
command SS.
*/
RN487x_Error RN487x_setModelName (RN487x *dv, char *name){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SDM,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* SDN,<text>
This command sets the manufacturer name characteristics in the Device Information
Service. This command is only effective if the Device Information service is enabled by
command SS.
*/
RN487x_Error RN487x_setManufacturerName (RN487x *dv, char *name){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SDN,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* SDR,<text>
This command sets software revision in the Device Information Service. This command
is only effective if the Device Information Service is enabled by command SS.
*/
RN487x_Error RN487x_setSoftwareRevision (RN487x *dv, char *rev){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SDR,%s\r",rev);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* SDS,<text>
This command sets the value of serial number characteristics in the Device Information
Service. This command is only effective if the Device Information Service is enabled by
command SS.
*/
RN487x_Error RN487x_setSerialNumber (RN487x *dv, char *sn){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SDS,%s\r",sn);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}


/* SF,1
This command resets the configurations into factory default. The first parameter must
be set to 1. This command does not delete the private service and characteristics cre-
ated using PS and PC commands and the script created using the Embedded Scripting
Feature.
*/
RN487x_Error RN487x_factoryConfiguration(RN487x *dv, uint8_t clearServices){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	if(clearServices){
		RN487xH_uartWrite(dv->hardwareInterface, "SF,2\r");
	}else{
		RN487xH_uartWrite(dv->hardwareInterface, "SF,1\r");
	}
	dv->ServerOrClientMode = 0;
	dv->commandModeCaracter = '$';
	return RN487x_lineReplyChecker(dv, "Reboot after Factory Reset\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SGA,%u\r",(unit8_t)power);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SGC,%u\r",(unit8_t)power);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* SN,<text>
This command sets the device name, where <text> is up to 20 alphanumeric
characters.
*/
RN487x_Error RN487x_setDeviceName(RN487x *dv, char *name){
	RN487xH_uartClear(dv->hardwareInterface);
	if(strlen(name)=>20) 
		name[20] = 0;
	sprintf(dv->buffTmp,"SN,%s\r",name);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SO,%u\r",(unit8_t)powerMode);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	if(pinDigitCount == 6){
		sprintf(dv->buffTmp,"SP,%06u\r",securityPin);
	}else{
		sprintf(dv->buffTmp,"SP,%04u\r",securityPin);
	}
	
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);	
}

/* SR,<hex16>
This command sets the supported feature of the RN4870 device. The input parameter
is a 16-bit bitmap that indicates the features supported. After changing the features, a
reboot is necessary to make the changes effective. Table 2-6 shows the bitmap of
features.
*/
RN487x_Error RN487x_setDeviceConfiguration(RN487x *dv, uint16_t bitmap){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SR,%04x\r",(unit8_t)bitmap);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);	
}
/*
Set bit of SR register.
*/
RN487x_Error RN487x_setBitDeviceConfiguration(RN487x *dv, RN487x_ConfigMask bitMask){.
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"SS,%02x\r",(unit8_t)bitmap);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);	
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"ST,%04x,%04x,%04x,%04x\r",minInterval, maxInterval, latency, timeOut);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);	
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
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"STA,%04x,%04x,%04x\r",fastInterval, fastTimeOut, slowInterval);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);	
}

/*
This command sets the advertisement interval for beacons as defined by the 'IB' and
'NB' commands. The beacon advertisement interval parameter unit is 0.625 ms. The
corresponding Get command, GTB, returns the beacon advertisement interval.
*/
RN487x_Error RN487x_setAdvertiseIntervalB(RN487x *dv, uint16_t beaconInterval){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"STB,%04x\r",beaconInterval);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
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
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GK\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, status, "\r", dv->standardTimeOutMs)
}

/* GNR
This command gets the peer device name when connected. If this command is issued
before a connection is established, an error message is the output
*/
RN487x_Error RN487x_getPeerDevName(RN487x *dv, char* name){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GNR\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, name, "\r", dv->standardTimeOutMs)
}


RN487x_Error RN487x_gsetSerializedDeviceName(RN487x *dv, char *name){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "G-\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, name, "\r", dv->standardTimeOutMs)	
}

RN487x_Error RN487x_getCommandModeCaracter(RN487x *dv, char *car){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "G$\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, car, "\r", dv->standardTimeOutMs)
}

RN487x_Error RN487x_getPrePostDelimCarStatuString(RN487x *dv, char *pre, char *post){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "G\x25\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	char *ptrTmp = strtok(dv->buffTmp, ',');
	strcpy(ptrTmp, pre);
	ptrTmp = strtok(null, ',');
	strcpy(ptrTmp, post);
	
	return RN487x_ok;		
}

RN487x_Error RN487x_getAuthenticationMode(RN487x *dv, RN487x_AuthenticationMode *mode){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GA\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",mode);
	return RN487x_ok;
}

RN487x_Error RN487x_getAppearanceId (RN487x *dv, uint16_t *id){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GDA\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",id);
	return RN487x_ok;	
}

RN487x_Error RN487x_getFirmwareVersion (RN487x *dv, char *ver){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GDF\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, ver, "\r", dv->standardTimeOutMs)
}

RN487x_Error RN487x_getHardwareRevison (RN487x *dv, char *rev){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GDH\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, rev, "\r", dv->standardTimeOutMs)	
}

RN487x_Error RN487x_getModelName (RN487x *dv, char *name){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GDM\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, name, "\r", dv->standardTimeOutMs)	
}

RN487x_Error RN487x_getManufacturerName (RN487x *dv, char *name){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GDN\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, name, "\r", dv->standardTimeOutMs)	
}

RN487x_Error RN487x_getSoftwareRevision (RN487x *dv, char *rev){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GDR\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, rev, "\r", dv->standardTimeOutMs)	
}

RN487x_Error RN487x_getSerialNumber (RN487x *dv, char *sn){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GDS\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, sn, "\r", dv->standardTimeOutMs)	
}

RN487x_Error RN487x_getOuputPowerAdvertise(RN487x *dv, RN487x_OutputPower *power){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GGA\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",power);
	return RN487x_ok;	
}

RN487x_Error RN487x_getOuputPowerConnected(RN487x *dv, RN487x_OutputPower *power){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GGC\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",power);
	return RN487x_ok;		
}

//RN487x_Error RN487x_getBaudRate(RN487x *dv, uint32_t *bd);
RN487x_Error RN487x_getDeviceName(RN487x *dv, char *name){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GN\r");
return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, name, "\r", dv->standardTimeOutMs)	
}
/*
0 = normal mode, 1 = low power
*/ 
RN487x_Error RN487x_getLowPowerMode(RN487x *dv, uint8_t *powerMode){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GO\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",powerMode);
	return RN487x_ok;		
}

RN487x_Error RN487x_getSecurityPin(RN487x *dv, uint16_t *securityPin){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GP\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",securityPin);
	return RN487x_ok;			
}

RN487x_Error RN487x_getDeviceConfiguration(RN487x *dv, uint16_t *bitmap){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GR\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",bitmap);
	return RN487x_ok;		
}

RN487x_Error RN487x_getDefaultService(RN487x *dv, uint8_t *bitmap){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GS\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp,"%u",bitmap);
	return RN487x_ok;	
}

RN487x_Error RN487x_getCentralConectionParameters(RN487x *dv, uint16_t *minInterval, uint16_t *maxInterval, uint16_t *latency, uint16_t *timeOut){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GT\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
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
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GTA\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
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
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "GTA\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, dv->buffTmp, "\r", dv->standardTimeOutMs)
	if(err != RN487x_ok)
	{
		return err;
	}
	sscanf(dv->buffTmp, "%x", beaconInterval);

	return RN487x_ok;	
}



/// ------------------------------------------ Action command ------------------------------------------------ ///
/*$$$
This command causes the RN4870/71 to enter Command mode and display command
prompt. The device passes characters as data and enters Command mode if it sees
the $$$ sequence. If the Command Mode Guard bit is set using SR, the device sees
any bytes before or after the $$$ characters in a one-second window, the device does
not enter Command mode and these bytes are passed through.
You can change the character string used to enter Command mode with the S$
command
*/
RN487x_Error RN487x_commandMode(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"%c%c%c",dv->commandModeCaracter);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "CMD> ", "Err\r", dv->standardTimeOutMs);
}
/* ---
This command causes the device to exit Command mode, displaying END.
*/
RN487x_Error RN487x_quitCommandMode(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "---\r");
	return RN487x_lineReplyChecker(dv, "END\r", "Err\r", dv->standardTimeOutMs);
}
/* !,1
RN4870/71 has the capability of Remote Command mode over UART Transparent
connection. Remote Command mode uses UART Transparent service. Therefore, it is
necessary to enable UART Transparent service using command SS before accessing
Remote Command mode feature
*/
RN487x_Error RN487x_enterRemoteMode(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "!,1\r");
	return RN487x_lineReplyChecker(dv, "RMT> ", "Err\r", dv->standardTimeOutMs);
}

/* !,0
RN4870/71 has the capability of Remote Command mode over UART Transparent
connection. Remote Command mode uses UART Transparent service. Therefore, it is
necessary to enable UART Transparent service using command SS before accessing
Remote Command mode feature.
// Quit comand mode
*/
RN487x_Error RN487x_exitRemoteMode(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "!,0\r");
	return RN487x_lineReplyChecker(dv, "%RMT_CMD_OFF%", "Err", dv->standardTimeOutMs);
}

/* |O,<hex8>,<hex8>
Command |O sets the output value of the digital I/O ports. It expects two input param-
eters. The first parameter is the bitmap of digital I/O ports that are affected by this com-
mand; the second parameter is the output value in the bitmap. The bitmap format is the
same as in command |I, shown in Table 2-12. If the bit in the bitmap of I/O ports is set
for a corresponding pin, and the pin is assigned to a predefined function, such bit is
automatically cleared. For pin function assignment, please check command SW
(2.4.26).
*/
RN487x_Error RN487x_writeIO(RN487x *dv, uint8_t mask, uint8_t value){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"|O,%02x,%02x\r",mask, value);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* A[,<hex16>,<hex16>]
Command A is used to start advertisement. The advertisement is undirected
connectable.
By default, or when command A is issued without a parameter, the advertisement is set
as a fast advertisement at first (at a fast advertising interval of 20 ms), followed by a
low-power slow advertisement after 30 seconds (slow advertisement interval of
961 ms). Command A is followed by two optional 16-bit hex parameters which indicate
advertisement interval with unit of millisecond and total advertisement time with unit of
640 ms, respectively. After the total advertisement time has elapsed, the advertising
stops along with a status string indicating the same. The optional second parameter
must be larger than the first parameter in actual time. When a parameter is used in
command A, the fast advertisement time-out is no longer effective and the advertise-
ment with the interval parameter can last forever if there is no second input parameter,
or not up to the time indicated by the second input parameter
*/
RN487x_Error RN487x_startAdvertise(RN487x *dv, uint16_t fastInterval, uint8_t slowInterval){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"A,%04x,%04x\r",fastInterval, slowInterval);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}


/* B
Command B is used to secure the connection and bond two connected devices. Com-
mand B is only effective if two devices are already connected. The bonding process can
be initiated from either the central or the peripheral device.
Once bonded, security materials are saved in both ends of the connection. Therefore,
reconnection between bonded devices does not require authentication, and reconnec-
tion can be done in a very short time.
If the bonded connection is lost due to any reason, reconnection does not automatically
provide a secured link. To secure the connection, another B command must be issued.
However, this command is only for securing link rather than saving connection
information.
*/
RN487x_Error RN487x_bondDevice(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "B\r");
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}
/* C
This command makes RN4870/71 try to connect to the last bonded device. When this
command is used to reconnect to a bonded device, the RN4870/71 automatically
secures the link once the connection is established.
//Quit command mode
*/
RN487x_Error RN487x_connectLastBondedDevice(RN487x *dv){
	RN487x_Error err;
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "C\r");
	
	err = RN487xH_uartReadUntilTimeOut(dv->hardwareInterface, dv->buffTmp,1000)
	if(err != RN487x_ok){
		return err;
	}
	if(strstr(dv->buffTmp, "%ERR_SEC%")){
		return RN487x_errorResponse;
	}
	if(strstr(dv->buffTmp, "%DISCONNECT%")){
		return RN487x_errorResponse;
	}
	if(strstr(dv->buffTmp, "%CONNECT%")){
		return RN487x_ok;
	}
	return RN487x_badResponse;	
}

/*
RN4870/71 can store the MAC addresses of up to eight bonded devices. The C com-
mand provides an easy way to reconnect to any stored device without typing the MAC
address of stored device, if such device uses public address. When this command is
used to reconnect to a bonded device, RN4870/71 automatically secures the link once
the connection is established. To display the list of stored devices, use command LB.
*/
RN487x_Error RN487x_connectBondedDevice(RN487x *dv, uint8_t bondId){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"C%x\r",bondId);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	err = RN487xH_uartReadUntilTimeOut(dv->hardwareInterface, dv->buffTmp,1000)
	if(err != RN487x_ok){
		return err;
	}
	if(strstr(dv->buffTmp, "%ERR_SEC%")){
		return RN487x_errorResponse;
	}
	if(strstr(dv->buffTmp, "%DISCONNECT%")){
		return RN487x_errorResponse;
	}
	if(strstr(dv->buffTmp, "%CONNECT%")){
		return RN487x_ok;
	}
	return RN487x_badResponse;	
}

/*
This command initiates connection to a remote BLE address where <address> is spec-
ified in hex format. The first parameter indicates the address type: 0 for public address
and 1 for private random address. When this command is used to connect to an already
bonded device, the link is not automatically secured. Instead, the user must use
command B to secure the link after the connection is established.
public = 0, private = 1
*/
RN487x_Error RN487x_connectByAdress(RN487x *dv, uint8_t publicOrPrivate, uint64_t adress){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"C,%x,%012x\r",publicOrPrivate, bondId);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	err = RN487xH_uartReadUntilTimeOut(dv->hardwareInterface, dv->buffTmp,1000)
	if(err != RN487x_ok){
		return err;
	}
	if(strstr(dv->buffTmp, "%ERR_SEC%")){
		return RN487x_errorResponse;
	}
	if(strstr(dv->buffTmp, "%DISCONNECT%")){
		return RN487x_errorResponse;
	}
	if(strstr(dv->buffTmp, "%CONNECT%")){
		return RN487x_ok;
	}
	return RN487x_badResponse;		
}

/* I
Command I is used to initiate UART Transparent operation with RN4870/71. This com-
mand expects no input parameter. Once this command is issued, RN4870/71 automat-
ically enters Data mode
*/
RN487x_Error RN487x_startUartTransparentMode(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "I");
	return RN487x_ok;
}
/*
Commands IA, IB, IS and NA, NB, NS set the advertisement, beacon and scan
response payload format, respectively.
All advertisement, beacon and scan response are composed of one or more Advertise-
ment Structure (AD Structure). Each AD structure has one byte of length, one byte of
Advertisement Type (AD Type, listed in Table 2-14) and Advertisement Data (AD Data).
The set of commands either append an AD structure or remove all AD structures,
depending on the first parameter. The total bytes in the advertisement payload contrib-
uted by one or more AD structures which includes the one byte of length, one byte of
AD type and AD data must be less than or equal to 31 bytes.
charSource = A for advertise, B for beacon, S for scan.
*/
RN487x_Error RN487x_appendAdvertiseContent(RN487x *dv, uint8_t flag, uint8_t *data, uint32_t size, uint8_t permanent, char charSource){
	char locBuffTmp[10];
	
	sprintf(dv->buffTmp,"%c%c,%02x", (permanent)? 'N': 'I', charSource)
	for(unsigned int i = 0; i< size; i++)
	{
		sprintf(locBuffTmp,"02x", *data);
		strcat(dv->buffTmp, locBuffTmp);
		data++;
	}
	strcat(dv->buffTmp, "\r");
	
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

RN487x_Error RN487x_clearAdvertiseContent(RN487x *dv, uint8_t permanent, char charSource){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"%c%c,Z\r", (permanent)? 'N': 'I', charSource)
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* JA,<0,1>,<MAC>
Command JA is used to add a MAC address to the white list. Once one device is added
to the white list, the white list feature is enabled. With the white list feature enabled,
when performing a scan, any device not included in the white list does not appear in
the scan results. As a peripheral, any device not listed in the white list cannot be
connected with a local device. The RN4870/71 supports up to 16 addresses in the
white list. If the white list is full, any attempt to add more addresses returns an error.
Command JA expects two input parameters. The first parameter is 0 or 1, indicating
that the following address is public or private. The second parameter is a 6-byte
address in hex format.
A random address stored in the white list cannot be resolved. If the peer device does
not change the random address, it is valid in the white list. If the random address is
changed, this device is no longer considered to be on the white list.
*/
RN487x_Error RN487x_whiteListMACadress(RN487x *dv, uint8_t publicOrPrivate, uint64_t adress){
	RN487xH_uartClear(dv->hardwareInterface);
	sprintf(dv->buffTmp,"JA,%x,%012x\r",publicOrPrivate, adress);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* JB
Command JB is used to add all currently bonded devices to the white list. Command
JB does not expect any parameter.
The random address in the white list can be resolved with command JB for connection
purposes. If the peer device changes its resolvable random address, the RN4870/71 is
still able to detect that the different random addresses are from the same physical
device; therefore, allows connection from such peer device. This feature is particularly
useful if the peer device is a iOS or Android device which uses resolvable random
address.
*/
RN487x_Error RN487x_whiteListBondedDevice(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "JB\r");
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* JC
Command JC is used to clear the white list. Once the white list is cleared, white list fea-
ture is disabled. Command JC does not expect any parameter.
The only way to disable white list is to clear it.
*/
RN487x_Error RN487x_clearWhiteList(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "JC\r");
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* JD
Command JD is used to display all MAC addresses that are currently in the white list.
Each MAC address displays in the white list, followed by 0 or 1 to indicate address
type, separated by a coma.
*/
RN487x_Error RN487x_readWhiteList(RN487x *dv, char *data){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "JD\r");
	return RN487xH_uartReadUntilStringDetected(dv->hardwareInterface, data, "END\r" dv->standardTimeOutMs);
}

/* K,1
Command K is used to disconnect the active BTLE link. It can be used in central or
peripheral role.
*/
RN487x_Error RN487x_disconnect(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "K,1\r");
	return RN487x_lineReplyChecker(dv, "AOK\r", "Err\r", dv->standardTimeOutMs);
}

/* M
Command M is used to get the signal strength of the last communication with the peer
device. The signal strength is used to estimate the distance between the device and its
remote peer. Command M does not expect any parameter.
The return value of command M is the signal strength in dBm
*/
RN487x_Error RN487x_getSignalStreng(RN487x *dv, float *strength){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487x_Error err;
	RN487xH_uartWrite(dv->hardwareInterface, "M\r");
	 
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface,dv->buffTmp, "\r", dv->standardTimeOutMs);
	if(err != RN487x_ok){
		return err;
	}
	sscanf(dv->buffTmp,"%f",strength);
	return RN487x_ok;
}

/* R,1
This command forces a complete device reboot (similar to a reboot or power cycle). It
has one mandatory parameter of 1. After rebooting RN4870/71, all prior made setting
changes takes effect.
*/
RN487x_Error RN487x_reboot(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "R,1\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface,dv->buffTmp, "%REBOOT%", dv->standardTimeOutMs);
	if(err != RN487x_ok){
		return err;
	}else{
		return RN487x_ok;
	}
}

/* U,<1-8,Z>
Command U removes existing bonding. This command works in both central or periph-
eral GAP roles.
Command U expects one input parameter, a single digit indicating the index of the
bonding to be removed. The index of the bonding is known by using command LB. If
the input parameter is letter “Z”, then all bonding information is cleared. If an empty
index is available, the new pairing and bonding information will be added at the first
available empty index.
*/
RN487x_Error RN487x_clearBonding(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "U,1\r");
	return RN487x_lineReplyChecker(dv,"AOK\r", "Err\r", dv->standardTimeOutMs);
}

/// ------------------------------------------ List command ------------------------------------------------------ ///
/* LB
Command LB lists all bonded devices in the following format:
<index>,<address>,<address type>
where <index> is a single-digit index in the range of 1 to 8, representing the index of
the bonded device in the bonding table. This index is used in command C<1-8> to
reconnect, and in command U,<1-8> to remove bonding.
The <address> is a 6-byte number representing the address of the bonded device;
<address type> is a single-digit number, taking either 0 or 1. Value 0 for <address type>
means that the address in the bonding information is a public address. In such case,
command C or C<1-8> is used to reconnect to the bonded device. Value 1 for
<address type> means random address, therefore, reconnection is not possible using
the bonded information, since the peer device may use a different random address
when RN4870/71 tries to reconnect
*/
RN487x_Error RN487x_listBondedDevice(RN487x *dv, char *device){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "LB\r");
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface,device, "END\r", dv->standardTimeOutMs);
	if(err != RN487x_ok){
		return err;
	}
	return RN487x_ok;
}


/* LS LC
Command LS, LC lists the server services and their characteristics.
Update in structure memory.
If device is in client mode service present in strucutre memory will be device service.
*/
RN487x_Error RN487x_listServices(RN487x *dv){
	//Clear services
	RN487xH_uartClear(dv->hardwareInterface);
	for(unsigned int i = 0; i<PRIVATE_SERVICE_NUMBER; i++){
		BleutoothSerivce_clear(&dv->privateServices[i]);
	}
	for(unsigned int i = 0; i<PUBLIC_SERVICE_NUMBER; i++){
		BleutoothSerivce_clear(&dv->publicServices[i]);
	}
	if(dv->ServerOrClientMode){
		//cliend mode
		RN487xH_uartWrite(dv->hardwareInterface, "LC\r");
	}else{
		//server mode
		RN487xH_uartWrite(dv->hardwareInterface, "LS\r");
	}
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface,device, "END\r", dv->standardTimeOutMs);
	if(err != RN487x_ok){
		return err;
	}
	char *line = strtok(dv->buffTmp, "\r"));
	dv->publicServicesCount = 0;
	dv->privateServicesCount = 0;
	uint32_t caracteristcId = 0;
	BleutoothSerivce *servPtr = null;
	while(line)
	{
		if(line[0] == ' ' && line[1] == ' '){
			// caracteristic
			uint16_t intval;
			
			char *valueStr = strtok(&line[2], ","));
			if (valueStr){
				// uuid
				strcpy(servPtr->characteristic[caracteristcId]->uuid, line);
			}
			
			valueStr = strtok(null, ","));
			if (valueStr){
				//handle
				sscanf(valueStr,"%u",&intval);
				servPtr->characteristic[caracteristcId]->uuid = intval;
			}
			
			valueStr = strtok(null, ","));
			if (valueStr){
				//flag
				sscanf(valueStr,"%u",&intval);
				servPtr->characteristic[caracteristcId]->flag = (uint8_t)intval;
			}
			
			valueStr = strtok(null, ","));
			if (valueStr){
				//handle
				sscanf(valueStr,"%u",&intval);
				servPtr->characteristic[caracteristcId]->config = (uint8_t)intval;
			}
			caracteristcId++;
		}else{
			caracteristcId = 0;
			if(strlen(line) > 6){
				//private
				servPtr = dv->privateServices[dv->privateServicesCount];
				dv->privateServicesCount++;
				strcpy(servPtr->uuid, line);
			}else{
				//public
				servPtr = dv->publicServices[dv->publicServicesCount];
				dv->publicServicesCount ++;
				strcpy(servPtr->uuid, line);
			}
		}
	}
	
	return RN487x_ok;
	
}

/* CI
Command CI is used to start client operation on the RN4870/71.
RN4870/71 starts as a GATT server by default. If the user also prefers RN4870/71 to
act as a GATT client, the command CI must be issued first.
Command CI performs essential service discovery process with the remote GATT
server and acquires supported public and private services and characteristics on the
remote GATT server. RN4870/71 supports up to six client public services and four client
private services. Each client service is able to define up to eight characteristics. Since
RN4870/71 needs to acquire the client service information over Bluetooth link, a con-
nection with the remote GATT server must be established before command CI is used.
Since command CI retrieves critical client information from the remote GATT server, it
is a perquisite over any Client Service related commands, such as LC, CHR and CHW.
*/
RN487x_Error RN487x_startClientOperation(RN487x *dv){
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, "CI\r");
	return RN487x_lineReplyChecker(dv,"AOK\r", "Err\r", dv->standardTimeOutMs);
}


/// ------------------------------------------ Services command ------------------------------------------------- ///

/*
Create a new service with specified caracteristics.
If service already exist a new copy will be crated, multiple service can have same uuid.
*/
RN487x_Error RN487x_createServerServices(RN487x *dv, BleutoothSerivce *service){
	RN487x_Error err;
	if(dv->ServerOrClientMode){
		// client mode 
		return RN487x_disable_in_client_mode;
	}
	
	sprintf(dv->buffTmp,"PS,%s",service->uuid);
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	err = RN487x_lineReplyChecker(dv,"AOK\r", "Err\r", dv->standardTimeOutMs);
	if(err != RN487x_ok){
		return err;
	}
	for(unsigned int i=0; i<SERVICE_CHARACTERISTICS_NUMBER; i++){
		if(service->characteristic[i]->uuid){
			// uuid is not 0 => exist, we add it.
			sprintf(dv->buffTmp,"PC,%s,%02x,%02x",service->characteristic[i]->uuid,
												  service->characteristic[i]->flag,
												  service->characteristic[i]->size,);
			err = RN487x_lineReplyChecker(dv,"AOK\r", "Err\r", dv->standardTimeOutMs);
			if(err != RN487x_ok){
				return err;
			}
		}
	}
	return RN487x_ok;
}


RN487x_Error RN487x_writeCaracteristic(BleutoothCharacteristics *caracteristic, uint8_t *data, uint8_t dataLen){
	char valTmp[10];
	char carClienServer = (dv->ServerOrClientMode)?'C':'S';
	sprintf(dv->buffTmp,"%cHW,%s",carClienServer, service->uuid);
	for(unsigned int i=0; i<dataLen; i++){
		sprintf(valTmp,"%02x",data[i]);
		strcat(dv->buffTmp,valTmp);
	}
	strcat(dv->buffTmp,"\r");
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	return RN487x_lineReplyChecker(dv,"AOK\r", "Err\r", dv->standardTimeOutMs);
}

RN487x_Error RN487x_readCaracteristic(BleutoothCharacteristics *caracteristic, uint8_t *data, uint8_t readLen){
	char carClienServer = (dv->ServerOrClientMode)?'C':'S';
	sprintf(dv->buffTmp,"%cHR,%s\r",carClienServer, service->uuid);	
	RN487xH_uartClear(dv->hardwareInterface);
	RN487xH_uartWrite(dv->hardwareInterface, dv->buffTmp);
	err = RN487xH_uartReadUntilStringDetected(dv->hardwareInterface,data, "\r", dv->standardTimeOutMs);
	if(err != RN487x_ok){
		return err;
	}
	return RN487x_ok;
}