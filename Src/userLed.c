#include <userLed.h>

void userLedInit(void) {

	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH |= GPIO_CRH_MODE13;
	GPIOC->CRH &= ~(GPIO_CRH_CNF13_0 | GPIO_CRH_CNF13_1);
}

void userLedToggle(bool state) {

	if (state) {

		GPIOC->BSRR = GPIO_BSRR_BS13;
	} else {

		GPIOC->BSRR = GPIO_BSRR_BR13;
	}
}
