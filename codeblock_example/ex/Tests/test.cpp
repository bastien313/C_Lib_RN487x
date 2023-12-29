#include "../Lib/RN487x.h"


int advertiseTest(RN487x *dv, int *err){
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
