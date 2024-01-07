#include "../Lib/RN487x.h"


int remoteTest(RN487x *dv1, int *err){
    RN487x_Error rnErr;
    int testId = 0;
    char myName[100];

    RN487x_commandMode(dv1);
    *err = (int)RN487x_isInCommandMode(dv1);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }

    rnErr = RN487x_getDeviceName(dv1, myName);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_startScan(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    BleutoothScanDevice sdv;
    sdv.name[0] = 0;
    while(!strstr(sdv.name, "RN4870")){
        rnErr = RN487x_scanGetNextEntry(dv1, &sdv, NULL, NULL, 1000);
        if(rnErr != RN487x_timeOut && rnErr != RN487x_ok){
            *err = (int)rnErr;
            return testId;
        }
    }

    rnErr = RN487x_stopScan(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_connectByAdress(dv1, sdv.addrType,sdv.address);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    /*rnErr = RN487x_connectLastBondedDevice(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }*/

    RN487x_commandMode(dv1);
    rnErr = RN487x_bondDevice(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_enterRemoteMode(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }


    rnErr = RN487x_setDeviceName(dv1, "littleRocket");
        if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    char tmp[1000];

    rnErr = RN487x_getDeviceName(dv1, tmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(strcmp(tmp, "littleRocket")){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_exitRemoteMode(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_disconnect(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    RN487x_commandMode(dv1);
    rnErr = RN487x_getDeviceName(dv1, tmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(strcmp(tmp, myName)){
        *err = -1; // missmatch
        return testId;
    }


    *err = rnErr;
    return testId;

}

int whiteListTest(RN487x *dv1, int *err){
    RN487x_Error rnErr;
    int testId = 0;

    RN487x_commandMode(dv1);
    *err = (int)RN487x_isInCommandMode(dv1);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;

    char data[1024];
    rnErr = RN487x_readWhiteList(dv1,data);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    printf("WhiteList: %s\n", data);
    testId++;

    rnErr = RN487x_startScan(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    BleutoothScanDevice sdv;
    sdv.name[0] = 0;
    while(!strstr(sdv.name, "RN4870")){
        rnErr = RN487x_scanGetNextEntry(dv1, &sdv, NULL, NULL, 1000);
        if(rnErr != RN487x_timeOut && rnErr != RN487x_ok){
            *err = (int)rnErr;
            return testId;
        }
    }

    rnErr = RN487x_stopScan(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_connectByAdress(dv1, sdv.addrType,sdv.address);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    RN487x_commandMode(dv1);
    rnErr = RN487x_bondDevice(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    rnErr = RN487x_whiteListBondedDevice(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_disconnect(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }



    rnErr = RN487x_readWhiteList(dv1,data);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    printf("WhiteList: %s\n", data);
    testId++;
    rnErr = RN487x_listBondedDevice(dv1,data);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    printf("bonded: %s\n", data);
    testId++;



    *err = rnErr;
    return testId;
}

int trasparentUartTest(RN487x *dv1,  RN487x *dv2, int *err){
    RN487x_Error rnErr;
    int testId = 0;

    RN487x_commandMode(dv1);
    *err = (int)RN487x_isInCommandMode(dv1);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;

    rnErr = RN487x_quitCommandMode(dv2);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_startScan(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    BleutoothScanDevice sdv;
    sdv.name[0] = 0;

    while(!strstr(sdv.name, "RN4870")){
        rnErr = RN487x_scanGetNextEntry(dv1, &sdv, NULL, NULL, 1000);
        if(rnErr != RN487x_timeOut && rnErr != RN487x_ok){
            *err = (int)rnErr;
            return testId;
        }
    }

    rnErr = RN487x_stopScan(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_connectByAdress(dv1, sdv.addrType,sdv.address);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    char dataread[1024];
    uint32_t lenread;
    RN487xH_uartReadUntilTimeOut(dv2->hardwareInterface, dataread,&lenread, (uint32_t)1000); //clear
    RN487xH_uartWriteStr(dv1->hardwareInterface, "Salut bastien, coment va tu?\r");

    rnErr = RN487xH_uartReadUntilStringDetected(dv2->hardwareInterface, dataread, "\r", 1000);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    if(strcmp(dataread, "Salut bastien, coment va tu?")){
        *err = -1; // missmatch
        return testId;
    }

    RN487xH_uartReadUntilTimeOut(dv2->hardwareInterface, dataread,&lenread, (uint32_t)1000); //clear
    RN487xH_uartWriteStr(dv1->hardwareInterface, "Salut bas$$$tien, coment va tu?\r");

    rnErr = RN487xH_uartReadUntilStringDetected(dv2->hardwareInterface, dataread, "\r", 1000);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    if(strcmp(dataread, "Salut bas$$$tien, coment va tu?")){
        *err = -1; // missmatch
        return testId;
    }

    RN487x_commandMode(dv1);

    rnErr = RN487x_disconnect(dv1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }





    *err = rnErr;
    return testId;

}

int scanTest(RN487x *dv, int *err){

    RN487x_Error rnErr;
    int testId = 0;

    RN487x_commandMode(dv);
    *err = (int)RN487x_isInCommandMode(dv);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;

    rnErr = RN487x_startScan(dv);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    Sleep(2000);

    unsigned int scanId = 0;
    while(1){
        BleutoothScanDevice sdv;
        char uudiBuff[1024];
        char broadcastPayload[1024];
        //printf("ID:%d",scanId);
        rnErr = RN487x_scanGetNextEntry(dv, &sdv, uudiBuff, broadcastPayload, 1000);
        if(rnErr == RN487x_timeOut){
            printf(".\n");
        }else if(rnErr == RN487x_ok){
           printf("\nAddress:%s\nrssi:%d\nName:%s\nuuids:%s\nAdrtype:%d\nConnectable:%d\nPayload:%s\n"
                  ,sdv.address, sdv.rssi, sdv.name,uudiBuff,sdv.addrType,sdv.connectable,broadcastPayload);
        }else{
                *err = (int)rnErr;
            return testId;
        }
        scanId++;

        if(scanId >= 100){
            rnErr = RN487x_stopScan(dv);
            *err = rnErr;
            return testId;
        }
    }

    *err = rnErr;
    return testId;
}

int readWriteServices(RN487x *dv, int *err){
    RN487x_Error rnErr;
    int testId = 0;

    RN487x_commandMode(dv);
    *err = (int)RN487x_isInCommandMode(dv);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;


    /*Service must exist.
        BleutoothSerivce_clear(&service);
    strcpy(service.uuid, "0102030405060708090A0B0C0D0E0F0F");
    strcpy(service.characteristic[0].uuid, "0101010101010101010101010101010F");
    service.characteristic[0].size = 10;
    service.characteristic[0].flag = btc_read | btc_notify;
    strcpy(service.characteristic[1].uuid, "0A0B0C0101010101010101010101010F");
    service.characteristic[1].size = 2;
    service.characteristic[1].flag = btc_write;
    */

    BleutoothCharacteristics *writeChar;
    BleutoothCharacteristics *readChar;

    writeChar = RN487x_getCharacteristcsStructureByUuid(RN487x_getServicesStructureByUuid(dv,"0102030405060708090A0B0C0D0E0F0F"),
                                                        "0A0B0C0101010101010101010101010F", btc_write);

    readChar = RN487x_getCharacteristcsStructureByUuid(RN487x_getServicesStructureByUuid(dv,"0102030405060708090A0B0C0D0E0F0F"),
                                                        "0101010101010101010101010101010F", btc_read);

    if(!writeChar){
        *err = -2; // not in command mode
        return testId;
    }
    if(!readChar){
        *err = -2; // not in command mode
        return testId;
    }
    uint8_t dataRead [100];
    uint8_t dataWrite [100];
    dataWrite[0] = '0';
    dataWrite[1] = 'A';
    dataWrite[2] = 'C';
    dataWrite[3] = 'C';
    while(1){

        rnErr = RN487x_readCaracteristic(dv, writeChar, dataRead, 2);
        if(rnErr != RN487x_ok){
            *err = (int)rnErr;
            return testId;
        }
        dataRead[2] = 0;
        printf("Read = %s\n", (char*)dataRead);

        rnErr = RN487x_writeCaracteristic(dv, readChar, dataWrite, 10);
        if(rnErr != RN487x_ok){
            *err = (int)rnErr;
            return testId;
        }
        dataWrite[0]++;
        if(dataWrite[0] > '9'){
            dataWrite[0] = '0';
        }
        Sleep(500);
    }

    *err = (int)rnErr;
    testId++;
    return testId;
}

int servicesTest(RN487x *dv, int *err){
    RN487x_Error rnErr;
    int testId = 0;
    RN487x_commandMode(dv);
    *err = (int)RN487x_isInCommandMode(dv);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;
    BleutoothSerivce service;

    BleutoothSerivce_clear(&service);
    strcpy(service.uuid, "0102030405060708090A0B0C0D0E0F0F");
    strcpy(service.characteristic[0].uuid, "0101010101010101010101010101010F");
    service.characteristic[0].size = 10;
    service.characteristic[0].flag = btc_read | btc_notify;
    strcpy(service.characteristic[1].uuid, "0A0B0C0101010101010101010101010F");
    service.characteristic[1].size = 2;
    service.characteristic[1].flag = btc_write;

    rnErr = RN487x_createServerServices(dv, &service);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    BleutoothSerivce_clear(&service);
    strcpy(service.uuid, "1802");
    strcpy(service.characteristic[0].uuid, "0C0C");
    service.characteristic[0].size = 10;
    service.characteristic[0].flag = btc_read | btc_notify;
    strcpy(service.characteristic[1].uuid, "0D0D");
    service.characteristic[1].size = 2;
    service.characteristic[1].flag = btc_write;

    rnErr = RN487x_createServerServices(dv, &service);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_listServices(dv);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    *err = (int)rnErr;
    testId++;
    return testId;

}

int advertiseTest(RN487x *dv, int *err){
    RN487x_Error rnErr;
    int testId = 0;
    RN487x_commandMode(dv);
    *err = (int)RN487x_isInCommandMode(dv);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;

    rnErr = RN487x_clearAdvertiseContent(dv,1,'A');
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;


    rnErr = RN487x_clearAdvertiseContent(dv,0,'A');
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    uint8_t advContent[5] = {0x18,0x99,0xAA,0x45,0x78};

    rnErr = RN487x_appendAdvertiseContent(dv,0xFF, advContent, 5, 0,'A');
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    uint8_t advContent2[2] = {0xFB,0xCA};

    rnErr = RN487x_appendAdvertiseContent(dv,0xFF, advContent2, 2, 1,'A');
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    *err = (int)rnErr;
    testId++;
    return testId;



}

int deviceResetConfiguration(RN487x *dv, int *err){
    RN487x_Error rnErr;
    int testId = 0;
    RN487x_commandMode(dv);
    *err = (int)RN487x_isInCommandMode(dv);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;

    rnErr = RN487x_factoryConfiguration(dv, 1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

        *err = (int)rnErr;
    return testId;
}
/*
Reset device configuration
*/
int test_getterSetter(RN487x *dv, int *err){
    RN487x_Error rnErr;
    int testId = 0;
    uint32_t errInt;
    char buffTmp [1024];
    char buffTmp2 [1024];
    uint32_t retUint32;
    uint16_t retUint16;
    uint16_t retUint16_1;
    uint16_t retUint16_2;
    uint16_t retUint16_3;
    RN487x_OutputPower retOp;




    RN487x_commandMode(dv);
    *err = (int)RN487x_isInCommandMode(dv);
    if(!err){
        *err = -2; // not in command mode
        return testId;
    }
    testId++;

    rnErr = RN487x_setSerializedDeviceName(dv, "Raphaelo");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getPrePostDelimCarStatuString(dv, buffTmp, buffTmp2);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "%") || strcmp(buffTmp2, "%")){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_setAppearanceId(dv, 5896);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getAppearanceId(dv, &retUint32);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(retUint32 != 5896){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_setFirmwareVersion(dv, "v1.26");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getFirmwareVersion(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "v1.26")){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_setHardwareRevison(dv, "v2.48");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getHardwareRevison(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "v2.48")){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_setModelName(dv, "bigRocket");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getModelName(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "bigRocket")){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_setManufacturerName(dv, "oxiRocket");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getManufacturerName(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "oxiRocket")){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_setSoftwareRevision(dv, "v0.13");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getSoftwareRevision(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "v0.13")){
        *err = -1; // missmatch
        return testId;
    }

    rnErr = RN487x_setSerialNumber(dv, "007");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getSerialNumber(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "007")){
        *err = -1; // missmatch
        return testId;
    }


    rnErr = RN487x_setOuputPowerAdvertise(dv, dbm_m10);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getOuputPowerAdvertise(dv, &retOp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retOp != dbm_m10){
        *err = (int)rnErr;
        return testId;
    }
    testId++;


    rnErr = RN487x_setOuputPowerConnected(dv, dbm_m15);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getOuputPowerConnected(dv, &retOp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retOp != dbm_m15){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_setDeviceName(dv, "littleRocket");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getDeviceName(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;
    if(strcmp(buffTmp, "littleRocket")){
        *err = -1; // missmatch
        return testId;
    }


    rnErr = RN487x_setSecurityPin(dv, "001854");
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getSecurityPin(dv, buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(strcmp(buffTmp, "001854")){
        *err = -1; // missmatch
        return testId;
    }
    testId++;

    rnErr = RN487x_setDeviceConfiguration(dv, 0x0234);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getDeviceConfiguration(dv, &retUint16);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retUint16 != 0x0234){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_setBitDeviceConfiguration(dv, no_beacon_scan);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_getDeviceConfiguration(dv, &retUint16);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retUint16 != 0x1234){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_resetBitDeviceConfiguration(dv, data_length_extension_dle);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }

    rnErr = RN487x_getDeviceConfiguration(dv, &retUint16);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retUint16 != 0x1224){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_setDefaultService(dv, 0x80);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getDefaultService(dv, (uint8_t*)buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(((uint8_t*)buffTmp)[0] != 0x80){
            //printf("%d",buffTmp[0]);
        *err = -1;
        return testId;
    }
    testId++;

       rnErr = RN487x_setBitDefaultService(dv, uart_transparent);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getDefaultService(dv, (uint8_t*)buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(((uint8_t*)buffTmp)[0] != 0xC0){
            //printf("%d",buffTmp[0]);
        *err = -1;
        return testId;
    }
    testId++;

           rnErr = RN487x_resetBitDefaultService(dv, device_information);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getDefaultService(dv, (uint8_t*)buffTmp);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(((uint8_t*)buffTmp)[0] != 0x40){
            //printf("%d",buffTmp[0]);
        *err = -1;
        return testId;
    }
    testId++;

    rnErr = RN487x_setCentralConectionParameters(dv, 0x1234, 0x5432, 0x9918, 0x3254);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getCentralConectionParameters(dv, &retUint16, &retUint16_1, &retUint16_2, &retUint16_3 );
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retUint16 != 0x1234 || retUint16_1 != 0x5432 ||retUint16_2 != 0x9918 ||retUint16_3 != 0x3254 ){
        *err = -1;
        return testId;
    }
    testId++;


    rnErr = RN487x_setAdvertiseIntervalA(dv, 0x4331, 0x0021, 0x1999);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getAdvertiseIntervalA(dv, &retUint16, &retUint16_1, &retUint16_2 );
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retUint16 != 0x4331 || retUint16_1 != 0x0021 ||retUint16_2 != 0x1999 ){
        *err = -1;
        return testId;
    }
    testId++;


    rnErr = RN487x_setAdvertiseIntervalB(dv, 0x4331);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getAdvertiseIntervalB(dv, &retUint16 );
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retUint16 != 0x4331){
        *err = -1;
        return testId;
    }
    testId++;

    /*rnErr = RN487x_setLowPowerMode(dv, 1);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getLowPowerMode(dv, &retUint32);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(retUint32 != 1){
        *err = -1;
        return testId;
    }
    testId++;*/

    RN487x_AuthenticationMode ret_auth;
    rnErr = RN487x_setAuthenticationMode(dv, keyboard_display);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    testId++;

    rnErr = RN487x_getAuthenticationMode(dv, &ret_auth);
    if(rnErr != RN487x_ok){
        *err = (int)rnErr;
        return testId;
    }
    if(ret_auth != keyboard_display){
        *err = -1;
        return testId;
    }
    testId++;

    *err = (int)rnErr;
    return testId;

}

//int test_getterSetter(RN487x *dv, int *err){
