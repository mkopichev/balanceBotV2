#ifndef IMU_H_
#define IMU_H_

#include "common.h"

#define LSM6DSR_ADDR 				0x6A

#define LSM6DSR_WHO_AM_I			0x0F
#define LSM6DSR_CTRL1_XL     		0x10
#define LSM6DSR_CTRL2_G      		0x11
#define LSM6DSR_OUTX_L_G    		0x22
#define LSM6DSR_OUTX_L_A     		0x28
#define LSM6DSR_WHO_AM_I_VAL		0x6B

#define LSM6DSR_ODR_104Hz    		0x04
#define LSM6DSR_FS_2g        		0x00

#define ACCEL_SCALE_FACTOR   		0.000061f
#define GYRO_SCALE_FACTOR	 		0.035f
#define GYRO_CALIBRATION_SAMPLES	250

uint8_t imuInit(void);
float imuGetAngle(void);

#endif /* IMU_H_ */
