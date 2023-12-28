#ifndef RN487xH_H
#define RN487xH_H

#include "LSerie.h"

#define SERIAL_SPEED_DEFAULT 115200


typedef enum{
	RN487x_ok = 0x00, RN487x_hardwareError = 0x01, RN487x_badResponse = 0x02, RN487x_errorResponse = 0x03, RN487x_timeOut = 0x04, RN487x_disable_in_client_mode = 0x05, RN487x_receptBufferVoid = 0x06,RN487x_memoryAdressError = 0x08
}RN487x_Error;

typedef struct{
	LSerie serialLink;
}RN487x_hardwareInterface;

RN487x_Error RN487xH_init(RN487x_hardwareInterface *dv, const char * comName);
RN487x_Error RN487xH_uartWrite(RN487x_hardwareInterface *dv, const char * dataToSend, uint32_t len);
RN487x_Error RN487xH_uartWriteStr(RN487x_hardwareInterface *dv, const char *dataToSend);
RN487x_Error RN487xH_uartReadChar(RN487x_hardwareInterface *dv, const char *data);
RN487x_Error RN487xH_uartReadUntilTimeOut(RN487x_hardwareInterface *dv, char *data, uint32_t *len, uint32_t timeoutMs);
RN487x_Error RN487xH_uartReadUntilStringDetected(RN487x_hardwareInterface *dv, char *data, const char*detection, uint32_t timeoutMs);
RN487x_Error RN487xH_uartClear(RN487x_hardwareInterface *dv);
RN487x_Error RN487xH_uartSetBaudRate(RN487x_hardwareInterface *dv, uint32_t baudRate);
uint8_t RN487xH_readPinStatus1(RN487x_hardwareInterface *dv);
uint8_t RN487xH_readPinStatus2(RN487x_hardwareInterface *dv);

#endif
