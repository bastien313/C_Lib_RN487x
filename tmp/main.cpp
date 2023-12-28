#include <iostream>
#include "Lib/RN487xH.h"
#include "Lib/RN487x.h"

using namespace std;

int main()
{
    RN487x_hardwareInterface hi;
    RN487x bt;


    if(RN487xH_init(&hi, "COM80") != RN487xH_ok){
        printf("Hardware init error!\n");
    }
    if(RN487x_init(&bt,&hi) != RN487x_ok){
         printf("Bt init error!\n");
    }
    printf("Bt init Ok!\n");

    cout << "Hello world!" << endl;
    return 0;
}
