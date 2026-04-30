#include "../Inc/imu.h"

uint8_t imuInit(void) {

	uint8_t whoami;
	i2cReadRegister(LSM6DSR_ADDR, WHO_AM_I, &whoami);
	if (whoami != 0x6B) {

		return 0;
	}
	return 1;
}
