#include "i2c.h"

static void i2cDelay(void) {
	volatile uint32_t i = 100;
	while (i--) {

		__NOP();
	}
}

// ========== ИНИЦИАЛИЗАЦИЯ ==========
void i2cInit(void) {

	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; // GPIOB

	GPIOB->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8 | GPIO_CRH_MODE9
			| GPIO_CRH_CNF9);
	// PB8
	GPIOB->CRH |= GPIO_CRH_MODE8_1;      // 2 MHz
	GPIOB->CRH |= GPIO_CRH_CNF8_0;       // Open-Drain
	// PB9
	GPIOB->CRH |= GPIO_CRH_MODE9_1;      // 2 MHz
	GPIOB->CRH |= GPIO_CRH_CNF9_0;       // Open-Drain

	SCL_HIGH(); // initial states
	SDA_HIGH();
	i2cDelay();
}

void i2cStart(void) {

	SDA_HIGH();
	SCL_HIGH();
	i2cDelay();
	SDA_LOW();
	i2cDelay();
	SCL_LOW();
	i2cDelay();
}

void i2cStop(void) {

	SDA_LOW();
	i2cDelay();
	SCL_HIGH();
	i2cDelay();
	SDA_HIGH();
	i2cDelay();
}

uint8_t i2cWriteByte(uint8_t data) { // send byte

	for (uint8_t i = 0; i < 8; i++) {
		if (data & 0x80) {

			SDA_HIGH();
		} else {

			SDA_LOW();
		}
		data <<= 1;
		i2cDelay();
		SCL_HIGH();
		i2cDelay();
		SCL_LOW();
		i2cDelay();
	}
	// 9-th bit
	SDA_HIGH(); // release SDA
	i2cDelay();
	SCL_HIGH();
	i2cDelay();
	uint8_t ack = SDA_READ();   // 0 = ACK, 1 = NACK
	SCL_LOW();
	i2cDelay();
	return ack;
}

uint8_t i2cReadByte(uint8_t send_ack) { // read byte

	uint8_t byte = 0;
	SDA_HIGH();   // release SDA
	for (uint8_t i = 0; i < 8; i++) {

		byte <<= 1;
		SCL_HIGH();
		i2cDelay();
		if (SDA_READ()) {

			byte |= 1;
		}
		SCL_LOW();
		i2cDelay();
	}
	if (send_ack) {

		SDA_LOW(); // if NOT last byte
	} else {

		SDA_HIGH(); // if last byte
	}
	i2cDelay();
	SCL_HIGH();
	i2cDelay();
	SCL_LOW();
	i2cDelay();
	SDA_HIGH(); // release SDA
	return byte;
}

// write len bytes to slave start from reg_addr
uint8_t i2cWriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
		uint16_t len) {

	i2cStart();
	if (i2cWriteByte((dev_addr << 1) | 0) != 0) {

		goto error;
	}
	if (i2cWriteByte(reg_addr) != 0) {

		goto error;
	}
	for (uint16_t i = 0; i < len; i++) {

		if (i2cWriteByte(data[i]) != 0) {

			goto error;
		}
	}
	i2cStop();
	return 1;
	error: i2cStop();
	return 0;
}

// read len bytes from slave start from reg_addr
uint8_t i2cReadRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
		uint16_t len) {

	i2cStart();
	if (i2cWriteByte((dev_addr << 1) | 0) != 0) {

		goto error;
	}
	if (i2cWriteByte(reg_addr) != 0) {

		goto error;
	}
	i2cStart();
	if (i2cWriteByte((dev_addr << 1) | 1) != 0) {

		goto error;
	}
	for (uint16_t i = 0; i < len; i++) {

		uint8_t ack = (i < len - 1);   // last byte - NACK
		data[i] = i2cReadByte(ack);
	}
	i2cStop();
	return 1;
	error: i2cStop();
	return 0;
}

//volatile uint32_t timeout;
//
//void i2cInit(void) {
//
//	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPBEN; // AFIO and PB
//
//	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN; // I2C1
//
//	AFIO->MAPR |= AFIO_MAPR_I2C1_REMAP; // I2C1 to PB8 and PB9
//
//	RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
//	RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
//
//	GPIOB->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8 | GPIO_CRH_MODE9
//			| GPIO_CRH_CNF9);
//	GPIOB->CRH |= GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_0 | GPIO_CRH_CNF8_1; // output mode and open-drain
//	GPIOB->CRH |= GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_0 | GPIO_CRH_CNF9_1;
//
//	I2C1->CR1 |= I2C_CR1_SWRST;
//	I2C1->CR1 &= ~I2C_CR1_SWRST;
//
//	I2C1->CR1 &= ~I2C_CR1_PE; // disable I2C1
//	I2C1->CR2 = 36; // 36 MHz clock frequency
//	I2C1->CCR = 45; // 400 kHz SCL freq
//	I2C1->TRISE = 12; // max rise time
//	I2C1->CR1 |= I2C_CR1_PE | I2C_CR1_ACK; // enable I2C1 and ACK
//}
//
//void i2cStart(void) {
//
//	I2C1->CR1 |= I2C_CR1_START;
//	timeout = TIMEOUT;
//	while (!(I2C1->SR1 & I2C_SR1_SB)) {
//
//		if (--timeout == 0) {
//
//			break;
//		}
//	}
//}
//
//void i2cStop(void) {
//
//	I2C1->CR1 |= I2C_CR1_STOP;
//	timeout = 1000;
//	while (--timeout) {
//
//		__NOP();
//	}
//}
//
//void i2cWriteByte(uint8_t byte) {
//
//	I2C1->DR = byte;
//	timeout = TIMEOUT;
//	while (!(I2C1->SR1 & I2C_SR1_TXE)) {
//
//		if (--timeout == 0) {
//
//			break;
//		}
//	}
//}
//
//uint8_t i2cReadByte(uint8_t ack) {
//
//	if (!ack) {
//
//		I2C1->CR1 &= ~I2C_CR1_ACK; // if last byte disable ack
//	}
//	timeout = TIMEOUT;
//	while (!(I2C1->SR1 & I2C_SR1_RXNE)) {
//
//		if (--timeout == 0) {
//
//			break;
//		}
//	}
//	uint8_t data = I2C1->DR;
//	if (!ack) {
//
//		I2C1->CR1 |= I2C_CR1_ACK;
//	}
//	return data;
//}
//
//void i2cWaitAddrAck(void) {
//
//	timeout = TIMEOUT;
//	while (!(I2C1->SR1 & I2C_SR1_ADDR)) {
//
//		if (--timeout == 0) {
//
//			break;
//		}
//	}
//	(void) I2C1->SR1;
//	(void) I2C1->SR2;
//}
//
//void i2cWriteRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
//
//	i2cStart();
//	i2cWriteByte(devAddr << 1);   // write (R/W=0)
//	i2cWaitAddrAck();
//	i2cWriteByte(regAddr);
//	i2cWriteByte(data);
//	i2cStop();
//}
//
//void i2cReadRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
//
//	i2cStart();
//	i2cWriteByte(devAddr << 1);   // write (R/W = 0)
//	i2cWaitAddrAck();
//	i2cWriteByte(regAddr);
//	i2cStart();
//	i2cWriteByte((devAddr << 1) | 1);   // read (R/W = 1)
//	i2cWaitAddrAck();
//	*data = i2cReadByte(0);
//	i2cStop();
//}
//
//void i2cReadMultiple(uint8_t devAddr, uint8_t regAddr, uint8_t *buffer,
//		uint8_t length) {
//
//	i2cStart();
//	i2cWriteByte(devAddr << 1);   // write (R/W = 0)
//	i2cWaitAddrAck();
//	i2cWriteByte(regAddr);
//	i2cStart();
//	i2cWriteByte((devAddr << 1) | 1);
//	i2cWaitAddrAck();
//	for (uint8_t i = 0; i < length; i++) {
//
//		uint8_t ack = (i < length - 1) ? 1 : 0;
//		buffer[i] = i2cReadByte(ack);
//	}
//	i2cStop();
//}
