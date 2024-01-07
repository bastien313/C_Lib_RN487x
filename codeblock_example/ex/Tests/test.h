#ifndef TESTS_H
#define TESTS_H

int remoteTest(RN487x *dv1, int *err);
int whiteListTest(RN487x *dv1, int *err);
int trasparentUartTest(RN487x *dv1,  RN487x *dv2, int *err);
int scanTest(RN487x *dv, int *err);
int readWriteServices(RN487x *dv, int *err);
int servicesTest(RN487x *dv, int *err);
int advertiseTest(RN487x *dv, int *err);
int deviceResetConfiguration(RN487x *dv, int *err);
int test_getterSetter(RN487x *dv, int *err);


#endif
