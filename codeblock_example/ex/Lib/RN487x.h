#ifndef RN487x_H
#define RN487x_H

#include "RN487xH.h"

#define UUID_SIZE (33) // Size = 32 + end string character => 33
#define SERVICE_CHARACTERISTICS_NUMBER (8)
#define PRIVATE_SERVICE_NUMBER (8)
#define PUBLIC_SERVICE_NUMBER (4)
#define BUFF_TMP_SIZE (1024)


typedef enum {
	btc_indicate = 0b00100000, btc_notify = 0b00010000, btc_write = 0b00001000,
	btc_write_without_response = 0b00000100, btc_read = 0b00000010
}RN487x_CaracteristicsFlag;

typedef enum {
	device_information = 0x80, uart_transparent = 0x40, beacon = 0x20, reserved = 0x10
}RN487x_ServicesMask;

typedef enum {
	enable_flow_control = 0x8000, no_prompt = 0x4000, fast_mode = 0x2000, no_beacon_scan = 0x1000,
	no_connect_scan = 0x0800, no_duplicate_scan_result_filter = 0x0400, passive_scan = 0x200,
	uart_transparent_without_ack = 0x0100, reboot_after_disconnection = 0x0080, running_script_after_power_on = 0x0040,
	support_rn4020_mldp_streaming_service = 0x0020, data_length_extension_dle = 0x0010, command_mode_guard = 0x0008
}RN487x_ConfigMask;

typedef enum {
	connection_established = 0x00, standby_state = 0x01, data_session_open = 0x02, power_on = 0x03
}RN487x_PinStatus;

typedef enum {
	no_input_no_output_with_bonding  = 0x00, display_yes_no = 0x01, no_input_no_output  = 0x02, keyboard_only  = 0x03,
	display_only = 0x04, keyboard_display = 0x05
}RN487x_AuthenticationMode;


typedef enum {
	dbm_0 = 0x00, dbm_m5 = 0x01, dbm_m10  = 0x02, dbm_m15  = 0x03, dbm_m20 = 0x04, dbm_m25 = 0x05
}RN487x_OutputPower;

typedef enum{
	b921600 = 0x00, b460800 = 0x01, b230400 = 0x02, b11520 = 0x03, b57600 = 0x04, b38400 = 0x05, b28800=0x06,
	b19200 = 0x07, b14400 = 0x08, b9600 = 0x09, b4800 = 0x0A, b2400 = 0x0B
}RN487x_BaudRate;


typedef enum{
	P2_2 = 0x01, P2_4 = 0x02, P3_5 = 0x04, P1_2 = 0x08, P1_3 = 0x10
}RN487x_IOmask;
/*
typedef enum{
	RN487x_ok = 0x00, RN487x_hardwareError = 0x01, RN487x_badResponse = 0x02, RN487x_errorResponse = 0x03, RN487x_timeOut = 0x04, RN487x_disable_in_client_mode = 0x05
}RN487x_Error;*/

typedef struct{
	uint8_t *address[16];
	uint8_t addrType;
	char name[256]
	uint8_t rssi;
	//uint8_t *data;
	uint8_t config;
}BleutoothScanDevice;
void BleutoothScanDevice_clear(BleutoothScanDevice *sd);

typedef struct{
	char uuid[UUID_SIZE]; //uuid string
	uint8_t flag;
	uint16_t handle;
	uint8_t size;
	//uint8_t *data;
	uint8_t config;
}BleutoothCharacteristics;
void BleutoothCharacteristics_clear(BleutoothCharacteristics *dv);

typedef struct{
	char uuid[UUID_SIZE]; //uuid string
	BleutoothCharacteristics characteristic[SERVICE_CHARACTERISTICS_NUMBER];
}BleutoothSerivce;
void BleutoothSerivce_clear(BleutoothSerivce *dv);

typedef struct {
	RN487x_hardwareInterface *hardwareInterface; // define this structure yourself, see RN487xH.c/h.
	char commandModeCaracter;
	uint8_t serverOrClientMode; // 0 = server, 1 = client;
	uint8_t localOrRemoteMode; // 0 = local, 1 = remote;
	char buffTmp[BUFF_TMP_SIZE];
	uint32_t standardTimeOutMs;
	uint32_t publicServicesCount;
	uint32_t privateServicesCount;
	BleutoothSerivce publicServices[PUBLIC_SERVICE_NUMBER];
	BleutoothSerivce privateServices[PRIVATE_SERVICE_NUMBER];
}RN487x;




RN487x_Error RN487x_init(RN487x *dv, RN487x_hardwareInterface *hardwareLink);
RN487x_PinStatus RN487x_getStatus(RN487x *dv);


RN487x_Error RN487x_setSerializedDeviceName(RN487x *dv, const char *name);
//RN487x_Error RN487x_setCommandModeCaracter(RN487x *dv, const char car);
//RN487x_Error RN487x_setPrePostDelimCarStatuString(RN487x *dv, const char *pre, const char *post);
RN487x_Error RN487x_setAuthenticationMode(RN487x *dv, RN487x_AuthenticationMode mode);

/// Device information setter ///
RN487x_Error RN487x_setAppearanceId (RN487x *dv, uint16_t id); // sensor = 0x0540, multisensor = 0x0556
RN487x_Error RN487x_setFirmwareVersion (RN487x *dv, const char *ver);
RN487x_Error RN487x_setHardwareRevison (RN487x *dv, const char *rev);
RN487x_Error RN487x_setModelName (RN487x *dv, const char *name);
RN487x_Error RN487x_setManufacturerName (RN487x *dv, const char *name);
RN487x_Error RN487x_setSoftwareRevision (RN487x *dv, const char *rev);
RN487x_Error RN487x_setSerialNumber (RN487x *dv, const char *sn);

RN487x_Error RN487x_factoryConfiguration(RN487x *dv, uint8_t clearServices);

RN487x_Error RN487x_setOuputPowerAdvertise(RN487x *dv, RN487x_OutputPower power);
RN487x_Error RN487x_setOuputPowerConnected(RN487x *dv, RN487x_OutputPower power);

RN487x_Error RN487x_setBaudRate(RN487x *dv, uint32_t bd);
RN487x_Error RN487x_setDeviceName(RN487x *dv, const char *name); //  20max
//RN487x_Error RN487x_setLowPowerMode(RN487x *dv, uint8_t powerMode); //  0 = normal mode, 1 = low power

RN487x_Error RN487x_setSecurityPin(RN487x *dv, const char *securityPin);
RN487x_Error RN487x_setDeviceConfiguration(RN487x *dv, uint16_t configuration);
RN487x_Error RN487x_setBitDeviceConfiguration(RN487x *dv, RN487x_ConfigMask bitMask);
RN487x_Error RN487x_resetBitDeviceConfiguration(RN487x *dv, RN487x_ConfigMask bitMask);
RN487x_Error RN487x_setDefaultService(RN487x *dv, uint8_t services);
RN487x_Error RN487x_setBitDefaultService(RN487x *dv, RN487x_ServicesMask bitMask);
RN487x_Error RN487x_resetBitDefaultService(RN487x *dv, RN487x_ServicesMask bitMask);
RN487x_Error RN487x_setCentralConectionParameters(RN487x *dv, uint16_t minInterval, uint16_t maxInterval, uint16_t latency, uint16_t timeOut);
RN487x_Error RN487x_setAdvertiseIntervalA(RN487x *dv, uint16_t fastInterval, uint16_t fastTimeOut, uint16_t slowInterval);
RN487x_Error RN487x_setAdvertiseIntervalB(RN487x *dv, uint16_t beaconInterval);


/// --------------------------------------------------- Getters --------------------------------------///

RN487x_Error RN487x_getConnectionStatus(RN487x *dv, char* status);
RN487x_Error RN487x_getPeerDevName(RN487x *dv, char* name);

//RN487x_Error RN487x_getCommandModeCaracter(RN487x *dv, char *car);
RN487x_Error RN487x_getPrePostDelimCarStatuString(RN487x *dv, char *pre, char *post); // max 4 car
RN487x_Error RN487x_getAuthenticationMode(RN487x *dv, RN487x_AuthenticationMode *mode);

/// Device information setter ///
RN487x_Error RN487x_getAppearanceId (RN487x *dv, uint32_t *id); // sensor = 0x0540, multisensor = 0x0556
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
RN487x_Error RN487x_getLowPowerMode(RN487x *dv, uint32_t *powerMode); //  0 = normal mode, 1 = low power

RN487x_Error RN487x_getSecurityPin(RN487x *dv, char *securityPin);

RN487x_Error RN487x_getDeviceConfiguration(RN487x *dv, uint16_t *configuration);
RN487x_Error RN487x_getDefaultService(RN487x *dv, uint8_t *services);
RN487x_Error RN487x_getCentralConectionParameters(RN487x *dv, uint16_t *minInterval, uint16_t *maxInterval, uint16_t *latency, uint16_t *timeOut);
RN487x_Error RN487x_getAdvertiseIntervalA(RN487x *dv, uint16_t *fastInterval, uint16_t *fastTimeOut, uint16_t *slowInterval);
RN487x_Error RN487x_getAdvertiseIntervalB(RN487x *dv, uint16_t *beaconInterval);



/// ------------------------------------------ Action command ------------------------------------------------ ///
uint32_t RN487x_isInCommandMode(RN487x *dv);
RN487x_Error RN487x_commandMode(RN487x *dv);
RN487x_Error RN487x_quitCommandMode(RN487x *dv);
RN487x_Error RN487x_enterRemoteMode(RN487x *dv);
RN487x_Error RN487x_exitRemoteMode(RN487x *dv);
RN487x_Error RN487x_writeIO(RN487x *dv, char mask, uint8_t value);
RN487x_Error RN487x_startAdvertise(RN487x *dv, uint16_t fastInterval, uint8_t slowInterval);
RN487x_Error RN487x_bondDevice(RN487x *dv);
RN487x_Error RN487x_connectLastBondedDevice(RN487x *dv);
RN487x_Error RN487x_connectBondedDevice(RN487x *dv, uint8_t bondId);
RN487x_Error RN487x_connectByAdress(RN487x *dv, uint8_t publicOrPrivate, uint64_t adress); // public = 0, private = 1
RN487x_Error RN487x_startUartTransparentMode(RN487x *dv); // public = 0, private = 1
RN487x_Error RN487x_clearAdvertiseContent(RN487x *dv, uint8_t permanent, char charSource);
RN487x_Error RN487x_appendAdvertiseContent(RN487x *dv, uint8_t adType, uint8_t *data, uint32_t size, uint8_t permanent, char charSource);
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
RN487x_Error RN487x_writeCaracteristic(RN487x *dv, BleutoothCharacteristics *caracteristic, uint8_t *data, uint8_t dataLen);
RN487x_Error RN487x_readCaracteristic(RN487x *dv, BleutoothCharacteristics *caracteristic, uint8_t *data, uint8_t readLen);

BleutoothSerivce *RN487x_getServicesStructureByUuid(RN487x *dv, const char *uuidSearch);
BleutoothCharacteristics *RN487x_getCharacteristcsStructureByUuid(BleutoothSerivce *sv, const char *uuidSearch, uint8_t flag);


/// Scannig
RN487x_Error RN487x_startScan(RN487x *dv);
RN487x_Error RN487x_scanGetNextEntry(RN487x *dv);



#endif
