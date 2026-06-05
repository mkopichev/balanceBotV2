#ifndef UART_H_
#define UART_H_

#include "common.h"

void uartInit(void);
void uartSendString(char *string);
void uartSendDecimal(int32_t data);
void uartSendFloat(float value, uint8_t precision);
void uartSendData(float angleValue, float outputValue, uint8_t precision);

#endif /* UART_H_ */
