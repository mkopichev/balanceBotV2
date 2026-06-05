#include "../Inc/uart.h"

void uartInit(void) {

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	GPIOB->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10 | GPIO_CRH_MODE11
			| GPIO_CRH_CNF11);
	GPIOB->CRH |= GPIO_CRH_MODE10_1 | GPIO_CRH_CNF10_1 | GPIO_CRH_CNF11_0;
	USART3->BRR = (19UL << USART_BRR_DIV_Mantissa_Pos)
			| (8UL << USART_BRR_DIV_Fraction_Pos);
	USART3->CR1 |= USART_CR1_UE | USART_CR1_TE;
}

void uartSendByte(uint8_t byte) {

	USART3->DR = byte;
	while (!(USART3->SR & USART_SR_TXE)) {

		__NOP();
	}
}

void uartSendString(char *string) {

	while (*string) {

		uartSendByte(*string++);
	}
}

void uartSendDecimal(int32_t data) {

	uint8_t tmpMas[5] = { 0, 0, 0, 0, 0 };

	if (data < 0) {

		uartSendByte('-');
	}

	for (uint8_t i = 0; i < 5; i++) {

		tmpMas[4 - i] = data % 10;
		data /= 10;
	}
	for (uint8_t i = 0; i < 5; i++) {

		uartSendByte(tmpMas[i] + '0');
	}
}

void uartSendFloat(float value, uint8_t precision) {

	if (value < 0) {

		uartSendByte('-');
		value = -value;
	}

	uint32_t intPart = (uint32_t) value;
	uartSendDecimal(intPart);

	uartSendByte('.');
	float frac = value - intPart;
	for (uint8_t i = 0; i < precision; i++) {

		frac *= 10;
		uint8_t digit = (uint8_t) frac;
		uartSendByte('0' + digit);
		frac -= digit;
	}
}

void uartSendData(float angleValue, float outputValue, uint8_t precision) {

	uartSendString("Angle:");
	uartSendFloat(angleValue, precision);
	uartSendByte(',');
	uartSendString("Output:");
	uartSendFloat(outputValue, precision);
	uartSendString("\r\n");
}
