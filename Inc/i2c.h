/*
 * i2c.h
 *
 *  Created on: Apr 28, 2026
 *      Author: GET-5
 */

#ifndef I2C_H_
#define I2C_H_

#include "common.h"

#define LSM6DSR_ADDR 0x6A

void i2cInit(void);
void i2cWriteRegister(uint8_t devAddr, uint8_t regAddr, uint8_t data);
void i2cReadRegister(uint8_t devAddr, uint8_t regAddr, uint8_t *data);

#endif /* I2C_H_ */
