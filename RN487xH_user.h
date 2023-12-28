typedef struct{
	
	USART_TypeDef *USARTx;
	GPIO_TypeDef *status1_port;
	GPIO_TypeDef *status2_port;
	uint32_t status1_pin;
	uint32_t status2_pin;
	
}RN487x_hardwareInterface;
RN487xH_init(RN487x_hardwareInterface *dv, USART_TypeDef *usart, GPIO_TypeDef *stat1_port,uint32_t stat1_pint, GPIO_TypeDef *stat2_port,uint32_t stat2_pin);
int RN487xH_uartWrite(RN487x_hardwareInterface *dv, char *dataToSend);

int RN487xH_uartReadUntilStringDetected(RN487x_hardwareInterface *dv, char *data, char*detection, uint32_t timeoutMs);
int RN487xH_uartReadUntilTimeOut(RN487x_hardwareInterface *dv, char *data, uint32_t timeoutMs);
int RN487xH_uartReadChar(RN487x_hardwareInterface *dv, char *data, uint32_t timeoutMs);
int RN487xH_uartClear(RN487x_hardwareInterface *dv);
int RN487xH_uartSetBaudRate(RN487x_hardwareInterface *dv);
uint8_t RN487xH_readPinStatus1(RN487x_hardwareInterface *dv);
uint8_t RN487xH_readPinStatus2(RN487x_hardwareInterface *dv);