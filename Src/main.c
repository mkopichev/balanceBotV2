/*
 * mc: STM32F103C8T6
 * imu: LSM6DSR, I2C address 0b1101011, pins SDA1 (PB9), SCL1 (PB8)
 * steppers: common cathode, left ENA+ PB3, DIR+ PB4, PUL+ PB5, right ENA+ PA3, DIR+ PA4, PUL+ PA5
 * */

#include "common.h"

extern uint8_t cpuFreqSourceChoosen;

int main(void) {

 	if (!cpuFreqInit(PLL_SOURCE)) {

		NVIC_SystemReset();
		userLedToggle(LED_C13, LED_ON);
	}

	userLedsInit();
 	userLedToggle(LED_C14, LED_ON);
	uartInit();
	stepperABInit();
	i2cInit();

	if (!imuInit()) {

		NVIC_SystemReset();
	}

	stepperABEnable();
	controlSystemInit();

 	userLedToggle(LED_C14, LED_OFF);

	for (;;) {

		__NOP();
	}
}
