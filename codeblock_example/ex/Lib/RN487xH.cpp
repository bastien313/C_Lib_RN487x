#include "RN487xH.h"
#include <time.h>

void printDebug(const char *base, const char *bufferAdd, unsigned int len){
    char strDebug[1024];
    char strTmp[20];
    strDebug[0] = 0;
    strTmp[0] = 0;

    strcat(strDebug, base);
    //strcat(strDebug,"|");
    for(unsigned int i = 0; i<len; i++){
        if(bufferAdd[i] == '\r')
            sprintf(strTmp,"\\r");
        else if(bufferAdd[i] == '\n')
            sprintf(strTmp,"\\n");
        else
            sprintf(strTmp,"%c",bufferAdd[i]);
        strcat(strDebug,strTmp);
    }
    //strcat(strDebug,"|");
    //strcat(strDebug,"\n");
    puts(strDebug);
}

RN487x_Error RN487xH_init(RN487x_hardwareInterface *dv, const char * comName){
    if(dv->serialLink.open(comName, SERIAL_SPEED_DEFAULT)){
        printDebug("@Open Ok",0,0);
        return RN487x_ok;
    }
    printDebug("@Open Err",0,0);
    return RN487x_hardwareError;
}

RN487x_Error RN487xH_uartWrite(RN487x_hardwareInterface *dv, const char * dataToSend, uint32_t len){
    int result = dv->serialLink.sendData((DWORD)len, (LPBYTE)dataToSend);
    char strDeb[50];

    if(result < 0 ){
        //Debug
        sprintf(strDeb, "@<TX>Err(%d):", result);
        printDebug(strDeb,dataToSend,len);
        //
        return RN487x_hardwareError;
    }
    if(result == len){
        //Debug
        sprintf(strDeb, "@<TX>(%d):", result);
        printDebug(strDeb,dataToSend,len);
        //
        return RN487x_ok;
    }
    //Debug
    sprintf(strDeb, "@<TX>Missing(%d):", result);
    printDebug(strDeb,dataToSend,len);
    //
    return RN487x_hardwareError;
}

RN487x_Error RN487xH_uartWriteStr(RN487x_hardwareInterface *dv, const char *dataToSend){
    return RN487xH_uartWrite(dv, dataToSend, strlen(dataToSend));
}

RN487x_Error RN487xH_uartReadChar(RN487x_hardwareInterface *dv, const char *data){
    int result = dv->serialLink.receiveData(1, (LPBYTE)data);
    if(result < 0 ){
        return RN487x_hardwareError;
    }
    if(result == 1){
        return RN487x_ok;
    }
    return RN487x_receptBufferVoid;
}

RN487x_Error RN487xH_uartReadUntilTimeOut(RN487x_hardwareInterface *dv, char *data, uint32_t *len, uint32_t timeoutMs){
    double initialTime = (double)clock();
    char strDeb[50];
    *len = 0;

    while( (((double)clock() - initialTime)/(CLOCKS_PER_SEC/1000)) < (double) timeoutMs){
        int result = RN487xH_uartReadChar(dv, &data[*len]);
        if(result == RN487x_ok){
            (*len)++;
        }else if(result == RN487x_hardwareError){
            sprintf(strDeb, "@<RX>Err(%d):",result);
            printDebug(strDeb, data, *len);
            return RN487x_hardwareError;
        }
    }
    sprintf(strDeb, "@<RX>(%d):",*len);
    printDebug(strDeb, data, *len);
    return RN487x_ok;
}

/*
Read incoming data in data(ptr) until detection string is detected.
This function remove the detection string from data, this information will be lost.
Remove = put 0(end line) at the postion on detection string.
*/
RN487x_Error RN487xH_uartReadUntilStringDetected(RN487x_hardwareInterface *dv, char *data, const char*detection, uint32_t timeoutMs){
    double initialTime = (double)clock();
    char strDeb[1024];
    unsigned int len = 0;
    data[len] = 0; // make valid string

    while( (((double)clock() - initialTime)/(CLOCKS_PER_SEC/1000)) < (double) timeoutMs){
        int result = RN487xH_uartReadChar(dv, &data[len]);
        if(result == RN487x_hardwareError){
            sprintf(strDeb, "@<RX>ErrStr(%d):",result);
            printDebug(strDeb, data, len);

            return RN487x_hardwareError;
        }
        if(result == RN487x_ok){
            len++;
            data[len] = 0; // make data array as valid string.
            char *detPos = strstr(data, detection);
            if(detPos){
                sprintf(strDeb, "@<RX>MatchStr(%d):",len);
                printDebug(strDeb, data, len);
                *detPos = 0;

                return RN487x_ok;
            }
        }
    }
    sprintf(strDeb, "@<RX>TimeOutStr(%d):",len);
    printDebug(strDeb, data, len);
    return RN487x_timeOut;
}

RN487x_Error RN487xH_uartClear(RN487x_hardwareInterface *dv){
    char readedChar;
    char debugOut[1024];
    char debClear[30];
    unsigned int idDeb = 0;
    while(RN487xH_uartReadChar(dv, &readedChar) == RN487x_ok){
        debugOut[idDeb] = readedChar;
        idDeb++;
    }
    sprintf(debClear, "@Clear(%d):", idDeb);
    printDebug(debClear, debugOut, idDeb);
    return RN487x_ok;
}
RN487x_Error RN487xH_uartSetBaudRate(RN487x_hardwareInterface *dv, uint32_t baudRate){
    if (dv->serialLink.setSpeed((DWORD)baudRate)){
        printDebug("@Baud Ok", 0, 0);
        return RN487x_ok;
    }
    printDebug("@Baud Err", 0, 0);
    return RN487x_hardwareError;
}
uint8_t RN487xH_readPinStatus1(RN487x_hardwareInterface *dv){
    return RN487x_ok;
}
uint8_t RN487xH_readPinStatus2(RN487x_hardwareInterface *dv){
    return RN487x_ok;
}

