/*
 * i2c.h
 *
 *  Created on: Apr 28, 2026
 *      Author: GET-5
 */

#ifndef I2C_H_
#define I2C_H_

#define SCL_PIN     (1UL << 8)   // PB8
#define SDA_PIN     (1UL << 9)   // PB9
#define I2C_PORT    GPIOB

#define SCL_HIGH()  (I2C_PORT->BSRR = SCL_PIN)
#define SCL_LOW()   (I2C_PORT->BRR  = SCL_PIN)
#define SDA_HIGH()  (I2C_PORT->BSRR = SDA_PIN)
#define SDA_LOW()   (I2C_PORT->BRR  = SDA_PIN)
#define SDA_READ()  ((I2C_PORT->IDR & SDA_PIN) != 0)

#include "common.h"

void i2cInit(void);
uint8_t i2cWriteRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
		uint16_t len);
uint8_t i2cReadRegister(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data,
		uint16_t len);

#endif /* I2C_H_ */
