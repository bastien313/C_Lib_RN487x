#include "RN487xH.h"
#include <time.h>

void printDebug(char *base, char *bufferAdd, unsigned int len){
    char strDebug[1024];
    char strTmp[20];

    strcat(strDebug, base);
    for(unsigned int i = 0; i<len; i++){
        sprintf(strTmp,"%c",bufferAdd[i]);
        strcat(strDebug,strTmp);
    }
    strcat(strDebug,"@debend@\n");
    printf(strDebug);
}

RN487xH_Error RN487xH_init(RN487x_hardwareInterface *dv, char *comName){
    if(dv->serialLink.open(comName, SERIAL_SPEED_DEFAULT)){
        printDebug("Open Ok\n",0,0);
        return RN487xH_ok;
    }
    printDebug("Open Err\n",0,0);
    return RN487xH_hardwareError;
}

RN487xH_Error RN487xH_uartWrite(RN487x_hardwareInterface *dv, char *dataToSend, uint32_t len){
    int result = dv->serialLink.sendData((DWORD)len, (LPBYTE)dataToSend);
    char strDeb[50];

    if(result < 0 ){
        //Debug
        sprintf(strDeb, "<TX>Err(%d), ", result);
        printDebug(strDeb,dataToSend,len);
        //
        return RN487xH_hardwareError;
    }
    if(result == len){
        //Debug
        sprintf(strDeb, "<TX>(%d), ", result);
        printDebug(strDeb,dataToSend,len);
        //
        return RN487xH_ok;
    }
    //Debug
    sprintf(strDeb, "<TX>Missing(%d), ", result);
    printDebug(strDeb,dataToSend,len);
    //
    return RN487xH_hardwareError;
}

RN487xH_Error RN487xH_uartWriteStr(RN487x_hardwareInterface *dv, char *dataToSend){
    return RN487xH_uartWrite(dv, dataToSend, strlen(dataToSend));
}

RN487xH_Error RN487xH_uartReadChar(RN487x_hardwareInterface *dv, char *data){
    int result = dv->serialLink.receiveData(1, (LPBYTE)data);
    if(result < 0 ){
        return RN487xH_hardwareError;
    }
    if(result == 1){
        return RN487xH_ok;
    }
    return RN487xH_receptBufferVoid;
}

RN487xH_Error RN487xH_uartReadUntilTimeOut(RN487x_hardwareInterface *dv, char *data, uint32_t *len, uint32_t timeoutMs){
    double initialTime = (double)clock();
    char strDeb[50];
    *len = 0;

    while( (((double)clock() - initialTime)/(CLOCKS_PER_SEC/1000)) < (double) timeoutMs){
        int result = RN487xH_uartReadChar(dv, &data[*len]);
        if(result < 0){
            sprintf(strDeb, "<RX>Err(%d),",result);
            printDebug(strDeb, data, *len);
            return RN487xH_hardwareError;
        }
        if(result > 0){
            (*len)++;
        }
    }
    sprintf(strDeb, "<RX>(%d),",*len);
    printDebug(strDeb, data, *len);
    return RN487xH_ok;
}

RN487xH_Error RN487xH_uartReadUntilStringDetected(RN487x_hardwareInterface *dv, char *data, uint32_t *len, char*detection, uint32_t timeoutMs){
    double initialTime = (double)clock();
    char strDeb[50];
    *len = 0;
    while( (((double)clock() - initialTime)/(CLOCKS_PER_SEC/1000)) < (double) timeoutMs){
        int result = RN487xH_uartReadChar(dv, &data[*len]);
        if(result < 0){
            sprintf(strDeb, "<RX>ErrStr(%d),",result);
            printDebug(strDeb, data, *len);
            return RN487xH_hardwareError;
        }
        if(result > 0){
            (*len)++;
            data[*len] = 0; // make data array as valid string.
            if( strstr(data, detection)){
                sprintf(strDeb, "<RX>MatchStr(%d),",*len);
                printDebug(strDeb, data, *len);
                return RN487xH_ok;
            }
        }
    }
    sprintf(strDeb, "<RX>TmoutStr(%d),",*len);
    printDebug(strDeb, data, *len);
    return RN487xH_timeOut;
}

RN487xH_Error RN487xH_uartClear(RN487x_hardwareInterface *dv){
    char readedChar;
    char debugOut[1024];
    unsigned int idDeb = 0;
    while(RN487xH_uartReadChar(dv, &readedChar) == RN487xH_ok){
        debugOut[idDeb] = readedChar;
        idDeb++;
    }
    printDebug("Clear: ", debugOut, idDeb);
    return RN487xH_ok;
}
RN487xH_Error RN487xH_uartSetBaudRate(RN487x_hardwareInterface *dv, uint32_t baudRate){
    if (dv->serialLink.setSpeed((DWORD)baudRate)){
        printDebug("Baud Ok ", 0, 0);
        return RN487xH_ok;
    }
    printDebug("Baud Err ", 0, 0);
    return RN487xH_hardwareError;
}
uint8_t RN487xH_readPinStatus1(RN487x_hardwareInterface *dv){
    return RN487xH_ok;
}
uint8_t RN487xH_readPinStatus2(RN487x_hardwareInterface *dv){
    return RN487xH_ok;
}

