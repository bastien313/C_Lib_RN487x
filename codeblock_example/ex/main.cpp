#include <iostream>
#include "Lib/RN487xH.h"
#include "Lib/RN487x.h"

using namespace std;

int main()
{
    RN487x_hardwareInterface hi;
    RN487x bt;


    if(RN487xH_init(&hi, "COM80") != RN487x_ok){
        printf("Hardware init error!\n");
    }
    if(RN487x_init(&bt,&hi) != RN487x_ok){
         printf("Bt init error!\n");
    }else{
         printf("Bt init Ok!\n");
    }

    for(unsigned int i = 0; i< 10; i++){
            if(RN487x_quitCommandMode(&bt) != RN487x_ok){
                printf("error quit");
                break;
            }
            if(RN487x_commandMode(&bt) != RN487x_ok){
                printf("error cmd");
                break;
            }
        }
    return 0;
}
