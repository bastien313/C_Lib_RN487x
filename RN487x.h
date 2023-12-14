#ifndef RN487x_H
#define RN487x_H

typedef struct {
 char commandModeCaracter;
}RN487x;




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



/// Hardawre specific method, rewrite this functions to match with your specific hardware ///
int RN487X_uartWrite(RN487x *dv, char *dataToSend);
int RN487X_uartRead(RN487x *dv, char *dataToRead);
int RN487X_uartClear(RN487x *dv);
uint8_t RN487X_readPinStatus1(void);
uint8_t RN487X_readPinStatus2(void);
///  ----------------------------------------------------------------------------------  ///



RN487x_PinStatus RN487X_getPinStatus(RN487x *dv);

int RN487X_replyChecker(RN487x *dv,char *reply, char *validReply, char *errorReply);

int RN487X_commandMode(RN487x *dv);
int RN487X_quitCommandMode(RN487x *dv);

int RN487X_setSerializedDeviceName(RN487x *dv, char *name);
int RN487X_setCommandModeCaracter(RN487x *dv, char car);
int RN487X_setPrePostDelimCarStatuString(RN487x *dv, char *pre, char *post); // max 4 car 
int RN487X_setAuthenticationMode(RN487x *dv, RN487x_AuthenticationMode mode);

/// Device information setter ///
int RN487X_setAppearanceId (RN487x *dv, uint16_t id);
int RN487X_setFirmwareVersion (RN487x *dv, char *ver);
int RN487X_setHardwareRevison (RN487x *dv, char *rev);
int RN487X_setModelName (RN487x *dv, char *name);
int RN487X_setManufacturerName (RN487x *dv, char *name);
int RN487X_setSoftwareRevision (RN487x *dv, char *rev);
int RN487X_setSerialNumber (RN487x *dv, char *sn);

int RN487X_factoryConfiguataion(RN487x *dv, uint8_t clearServices);

int RN487X_setOuputPowerAdvertise(RN487x *dv, RN487x_OutputPower power);
int RN487X_setOuputPowerConnected(RN487x *dv, RN487x_OutputPower power);

int RN487X_setDeviceName(RN487x *dv, char *name); //  20max
#endif
