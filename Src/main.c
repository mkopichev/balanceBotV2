/*
 * mc: STM32F103C8T6
 * imu: LSM6DSR, I2C address 0b1101011, pins SDA1 (PB9), SCL1 (PB8)
 * steppers: common cathode, left ENA+ PB3, DIR+ PB4, PUL+ PB5, right ENA+ PA3, DIR+ PA4, PUL+ PA5
 * */

#include "common.h"

extern uint8_t cpuFreqSourceChoosen;

float res;

int main(void) {

	if (!cpuFreqInit(PLL_SOURCE)) {

		NVIC_SystemReset();
		cpuFreqSourceChoosen = HSI_SOURCE;
		userLedToggle(LED_ON);
	}
	userLedInit();
	stepperABInit();
//	stepperABEnable();
	i2cInit();
	if(!imuInit()) {

		userLedToggle(LED_ON);
	}
	controlSystemInit();

	for (;;) {

		res = imuGetAngle();
	}
}
