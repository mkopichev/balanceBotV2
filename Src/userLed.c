#include "userLed.h"

void userLedsInit(void) {

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH |= GPIO_CRH_MODE13_1 | GPIO_CRH_MODE14_1;
	GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_CNF14);
	userLedToggle(LED_C13, LED_OFF);
	userLedToggle(LED_C14, LED_OFF);
}

void userLedToggle(uint8_t ledChoice, bool state) {

	if (state) {

		if (ledChoice == LED_C13) {

			GPIOC->BSRR = GPIO_BSRR_BR13;
		} else {

			GPIOC->BSRR = GPIO_BSRR_BS14;
		}
	} else {

		if (ledChoice == LED_C13) {

			GPIOC->BSRR = GPIO_BSRR_BS13;
		} else {

			GPIOC->BSRR = GPIO_BSRR_BR14;
		}
	}
}
