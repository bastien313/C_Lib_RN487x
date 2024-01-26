#include "RN487xH.h"
#include <time.h>



RN487x_Error RN487xH_init(RN487x_hardwareInterface *dv, const char * comName){

    return RN487x_hardwareError;
}

RN487x_Error RN487xH_uartWrite(RN487x_hardwareInterface *dv, const char * dataToSend, uint32_t len){

    return RN487x_hardwareError;
}

RN487x_Error RN487xH_uartWriteStr(RN487x_hardwareInterface *dv, const char *dataToSend){
    return RN487xH_uartWrite(dv, dataToSend, strlen(dataToSend));
}

RN487x_Error RN487xH_uartReadChar(RN487x_hardwareInterface *dv, const char *data){
	return RN487x_receptBufferVoid;
}

RN487x_Error RN487xH_uartReadUntilTimeOut(RN487x_hardwareInterface *dv, char *data, uint32_t *len, uint32_t timeoutMs){

    return RN487x_ok;
}

/*
Read incoming data in data(ptr) until detection string is detected.
This function remove the detection string from data, this information will be lost.
Remove = put 0(end line) at the postion on detection string.
*/
RN487x_Error RN487xH_uartReadUntilStringDetected(RN487x_hardwareInterface *dv, char *data, const char*detection, uint32_t timeoutMs){

    return RN487x_timeOut;
}

RN487x_Error RN487xH_uartClear(RN487x_hardwareInterface *dv){

    return RN487x_ok;
}
RN487x_Error RN487xH_uartSetBaudRate(RN487x_hardwareInterface *dv, uint32_t baudRate){

    return RN487x_hardwareError;
}
uint8_t RN487xH_readPinStatus1(RN487x_hardwareInterface *dv){
    return RN487x_ok;
}
uint8_t RN487xH_readPinStatus2(RN487x_hardwareInterface *dv){
    return RN487x_ok;
}

