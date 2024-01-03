#include <iostream>
#include "Lib/RN487xH.h"
#include "Lib/RN487x.h"
#include "Tests/test.h"

using namespace std;

int main()
{
    RN487x_hardwareInterface hi;
    RN487x bt;

    if(RN487xH_init(&hi, "COM79") != RN487x_ok){
        printf("Hardware init error!\n");
        return -1;
    }else{
        printf("Hardware Ok\n");
    }
    if(RN487x_init(&bt,&hi) != RN487x_ok){
         printf("Bt init error!\n");
         return -1;
    }else{
         printf("Bt init Ok!\n");
    }


    int err;
    int testId;

    testId = deviceResetConfiguration(&bt, &err);
    if(err != RN487x_ok){
        printf("Error (%d) on test (%d)\n", err, testId);
        return -1;
    }else{
        printf("Reset succes!\n");
    }

  /*  testId = test_getterSetter(&bt, &err);
    if(err != RN487x_ok){
        printf("Error (%d) on test (%d)\n", err, testId);
        return -1;
    }else{
        printf("Test succes!\n");
    }*/
    /*RN487x_commandMode(&bt);
    RN487x_Error rnErr = RN487x_writeIO(&bt, P2_4, 0x01);
    if(rnErr != RN487x_ok){
        printf("Io pin err(%d)!\n",(int)rnErr);
    }*/

    /*testId = advertiseTest(&bt, &err);
    if(err != RN487x_ok){
        printf("Error (%d) on test (%d)\n", err, testId);
        return -1;
    }else{
        printf("advertise succes!\n");
    }*/


    testId = servicesTest(&bt, &err);
    if(err != RN487x_ok){
        printf("Error (%d) on test (%d)\n", err, testId);
        return -1;
    }else{
        printf("Services succes!\n");
    }

   /* RN487x_commandMode(&bt);
    err = RN487x_listServices(&bt);
    if(err != RN487x_ok){
        printf("Error (%d) on list service\n", err);
        return -1;
    }else{
        printf("Services succes!\n");

    }

    testId = readWriteServices(&bt, &err);
    if(err != RN487x_ok){
        printf("Error (%d) on rw services (%d)\n", err, testId);
        return -1;
    }else{
        printf("Services succes!\n");
    }*/




    return 0;
}
