#ifndef TESTS_H
#define TESTS_H


int readWriteServices(RN487x *dv, int *err);
int servicesTest(RN487x *dv, int *err);
int advertiseTest(RN487x *dv, int *err);
int deviceResetConfiguration(RN487x *dv, int *err);
int test_getterSetter(RN487x *dv, int *err);


#endif
