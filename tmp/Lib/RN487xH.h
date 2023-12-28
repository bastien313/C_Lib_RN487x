#ifndef RN487xH_H
#define RN487xH_H

#include "LSerie.h"

#define SERIAL_SPEED_DEFAULT 115200

typedef enum{
	RN487xH_ok = 0x00, RN487xH_hardwareError = 0x01,RN487xH_receptBufferVoid = 0x02, RN487xH_timeOut = 0x04
}RN487xH_Error;



typedef struct{
	LSerie serialLink;
}RN487x_hardwareInterface;

RN487xH_Error RN487xH_init(RN487x_hardwareInterface *dv, char *comName);
RN487xH_Error RN487xH_uartWrite(RN487x_hardwareInterface *dv, char *dataToSend, uint32_t len);
RN487xH_Error RN487xH_uartWriteStr(RN487x_hardwareInterface *dv, char *dataToSend);
RN487xH_Error RN487xH_uartReadChar(RN487x_hardwareInterface *dv, char *data);
RN487xH_Error RN487xH_uartReadUntilTimeOut(RN487x_hardwareInterface *dv, char *data, uint32_t *len, uint32_t timeoutMs);
RN487xH_Error RN487xH_uartReadUntilStringDetected(RN487x_hardwareInterface *dv, char *data, uint32_t *len, char*detection, uint32_t timeoutMs);
RN487xH_Error RN487xH_uartClear(RN487x_hardwareInterface *dv);
RN487xH_Error RN487xH_uartSetBaudRate(RN487x_hardwareInterface *dv, uint32_t baudRate);
uint8_t RN487xH_readPinStatus1(RN487x_hardwareInterface *dv);
uint8_t RN487xH_readPinStatus2(RN487x_hardwareInterface *dv);

#endif
