#include "i2c.h"

volatile uint32_t timeout;

void i2cInit(void) {

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN; // PB8, PB9 and AFIO
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP;

	GPIOB->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8 | GPIO_CRH_MODE9
			| GPIO_CRH_CNF9);
	GPIOB->CRH |= (GPIO_CRH_MODE8 | GPIO_CRH_CNF8); // output mode and open-drain
	GPIOB->CRH |= (GPIO_CRH_MODE9 | GPIO_CRH_CNF9);

	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST; // reset
	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;

	I2C1->CR1 &= ~I2C_CR1_PE; // disable I2C1
	I2C1->CR2 = 36; // 36 MHz clock frequency
	I2C1->CCR = 45; // 400 kHz SCL freq
	I2C1->TRISE = 37; // max rise time
	I2C1->CR1 |= I2C_CR1_PE | I2C_CR1_ACK; // enable I2C1 and ACK
}

void i2cStart(void) {

	I2C1->CR1 |= I2C_CR1_START;
	timeout = TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_SB)) {

		if (--timeout == 0) {

			break;
		}
	}
}

void i2cStop(void) {

	I2C1->CR1 |= I2C_CR1_STOP;
	volatile uint16_t delay = 3600;
	while (--delay) {

		__NOP();
	}
}

void i2cWriteByte(uint8_t byte) {

	I2C1->DR = byte;
	timeout = TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_TXE)) {

		if (--timeout == 0) {

			break;
		}
	}
}

uint8_t i2cReadByte(uint8_t ack) {

	if (!ack) {

		I2C1->CR1 &= ~I2C_CR1_ACK; // if last byte disable ack
	}
	timeout = TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_RXNE)) {

		if (--timeout == 0) {

			break;
		}
	}
	uint8_t data = I2C1->DR;
	if (!ack) {

		I2C1->CR1 |= I2C_CR1_ACK;
	}
	return data;
}

void i2cWriteRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data) {

	i2cStart();
	i2cWriteByte(devAddr << 1);   // write (R/W=0)
	timeout = TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_ADDR)) {

		if (--timeout == 0) {

			break;
		}
	}
	(void) I2C1->SR1;
	(void) I2C1->SR2;
	i2cWriteByte(regAddr);
	i2cWriteByte(data);
	i2cStop();
}

void i2cReadRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {

	i2cStart();
	i2cWriteByte(devAddr << 1);   // write (R/W = 0)
	timeout = TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_ADDR)) {

		if (--timeout == 0) {

			break;
		}
	}
	(void) I2C1->SR1;
	(void) I2C1->SR2;
	i2cWriteByte(regAddr);
	i2cStart();
	i2cWriteByte((devAddr << 1) | 1);   // read (R/W = 1)
	timeout = TIMEOUT;
	while (!(I2C1->SR1 & I2C_SR1_ADDR)) {

		if (--timeout == 0) {

			break;
		}
	}
	(void) I2C1->SR1;
	(void) I2C1->SR2;
	*data = i2cReadByte(0);
	i2cStop();
}
