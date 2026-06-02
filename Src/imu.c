#include "../Inc/imu.h"

float filteredAngle = 0.0f;
float gyroBias = 0.0f;

uint8_t imuInit(void) {

	uint8_t whoami;
	i2cReadRegister(LSM6DSR_ADDR, LSM6DSR_WHO_AM_I, &whoami, 1);
	if (whoami != LSM6DSR_WHO_AM_I_VAL) {

		return 0;
	}
	// ODR = 104 Гц, Full Scale = ±2g
	uint8_t ctrl1_val = (LSM6DSR_ODR_104Hz << 4) | (LSM6DSR_FS_2g << 2)
			| (0x01 << 1) | 0x02;
	if (!i2cWriteRegister(LSM6DSR_ADDR, LSM6DSR_CTRL1_XL, &ctrl1_val, 1)) {

		return 0;
	}
	// CTRL2_G: ODR = 104 Гц, FS = 1000 dps (or 2000 dps)
	uint8_t ctrl2_val = (LSM6DSR_ODR_104Hz << 4) | (0x02 << 2); // 1000 dps
	if (!i2cWriteRegister(LSM6DSR_ADDR, LSM6DSR_CTRL2_G, &ctrl2_val, 1)) {

		return 0;
	}

	delayMs(1000);

	uint8_t data[6];
	int32_t sum = 0;
	for (uint16_t i = 0; i < GYRO_CALIBRATION_SAMPLES; i++) {
		i2cReadRegister(LSM6DSR_ADDR, LSM6DSR_OUTX_L_G, data, 6);
		int16_t raw_gy = (int16_t) ((data[3] << 8) | data[2]); // axis Y
		sum += raw_gy;
		delayMs(1); // pause between measurements
	}
	gyroBias = (float) sum / GYRO_CALIBRATION_SAMPLES;

	// initialize filterdAngle
	i2cReadRegister(LSM6DSR_ADDR, LSM6DSR_OUTX_L_A, data, 6);
	int16_t rawAx = (int16_t) ((data[1] << 8) | data[0]);
	int16_t rawAz = (int16_t) ((data[5] << 8) | data[4]);
	float ax = rawAx * ACCEL_SCALE_FACTOR;
	float az = rawAz * ACCEL_SCALE_FACTOR;
	filteredAngle = atan2f(ax, az) * (180.0f / M_PI);

	return 1;
}

float imuGetAngle(void) {

	const float dt = 0.01f;

	// accel and gyro
	uint8_t data[12];
	if (!i2cReadRegister(LSM6DSR_ADDR, LSM6DSR_OUTX_L_G, data, 12)) {

		return filteredAngle;
	}

	// accel
	int16_t rawAx = (int16_t) ((data[7] << 8) | data[6]);
	int16_t rawAz = (int16_t) ((data[11] << 8) | data[10]);
	float ax = rawAx * ACCEL_SCALE_FACTOR;
	float az = rawAz * ACCEL_SCALE_FACTOR;
	float accelAngle = atan2f(ax, az) * (180.0f / M_PI);

	// gyro
	int16_t rawGy = (int16_t) ((data[3] << 8) | data[2]); // axis Y (pitch)
	float gyroRate = (rawGy - gyroBias) * GYRO_SCALE_FACTOR;
	float gyroAngle = filteredAngle + gyroRate * dt; // integrating

	static float prevAccelAngle = 0.0f;
	static uint8_t firstСall = 1;
	if (firstСall) {

		prevAccelAngle = accelAngle;
		firstСall = 0;
	}
	float accelDelta = fabsf(accelAngle - prevAccelAngle);
	const float alpha = 0.8f;
	if (accelDelta > 10.0f) {
		// mistaken data, use only gyro
		filteredAngle = gyroAngle;
	} else {

		filteredAngle = alpha * gyroAngle + (1.0f - alpha) * accelAngle;
	}
	prevAccelAngle = accelAngle;

	return filteredAngle;
}
