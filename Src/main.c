/*
 * mc: STM32F103C8T6
 * imu: L3GD20, I2C address 0b1101011, pins SDA1 (PB9), SCL1 (PB8)
 * steppers: common cathode, left ENA+ PB3, DIR+ PB4, PUL+ PB5, right ENA+ PA3, DIR+ PA4, PUL+ PA5
 * */

#include "common.h"

extern uint8_t cpuFreqSourceChoosen;

int main(void) {

	if (!cpuFreqInit(PLL_SOURCE)) {

		NVIC_SystemReset();
		cpuFreqSourceChoosen = HSI_SOURCE;
	}
	userLedInit();
	stepperABInit();
	stepperABEnable();
	for (;;) {

		for (uint8_t i = 0; i < 255; i++) {

			stepperABMove(i, false);
			delayMs(10);
		}
		for (uint8_t i = 0; i < 255; i++) {

			stepperABMove(255 - i, false);
			delayMs(10);
		}
		for (uint8_t i = 0; i < 255; i++) {

			stepperABMove(i, true);
			delayMs(10);
		}
		for (uint8_t i = 0; i < 255; i++) {

			stepperABMove(255 - i, true);
			delayMs(10);
		}
	}
}
