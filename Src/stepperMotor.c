#include <stepperMotors.h>

// for enabling - reset ENA pin
void stepperABEnable(void) {

	GPIOA->ODR &= ~GPIO_ODR_ODR3;
	GPIOB->ODR &= ~GPIO_ODR_ODR3;
}

void stepperABDisable(void) {

	GPIOA->ODR |= GPIO_ODR_ODR3;
	GPIOB->ODR |= GPIO_ODR_ODR3;
}

void stepperABInit(void) {

	RCC->APB2ENR |=
	RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	GPIOA->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
	GPIOA->CRL |= GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5;
	GPIOA->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5);
	// for using PB3 and 4, we need to get rid of JTAG functionality
	AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;
	GPIOB->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5);
	GPIOB->CRL |= GPIO_CRL_MODE3 | GPIO_CRL_MODE4 | GPIO_CRL_MODE5;
	GPIOB->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_CNF4 | GPIO_CRL_CNF5);

	stepperABDisable();

	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;
	TIM2->PSC = 72 - 1; // 1 MHz
	TIM2->ARR = 0xFFFF;
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM2->CR1 |= TIM_CR1_CEN | TIM_CR1_URS | TIM_CR1_ARPE;

	TIM3->PSC = 72 - 1; // 1 MHz
	TIM3->ARR = 0xFFFF;
	TIM3->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM3_IRQn);
	TIM3->CR1 |= TIM_CR1_CEN | TIM_CR1_URS | TIM_CR1_ARPE;
}

// set speed for motor A (TIM2)
void stepperASetSpeed(uint32_t freqHz) {

	if (freqHz == 0) {

		stepperABDisable();
		return;
	}
	stepperABEnable();
	if (freqHz < MIN_FREQ) {

		freqHz = MIN_FREQ;
	}
	if (freqHz > MAX_FREQ) {

		freqHz = MAX_FREQ;
	}
	uint32_t arr = (1000000UL / (2 * freqHz)) - 1;
	if (arr > 0xFFFF) {

		arr = 0xFFFF;
	}
	if (arr < 1) {

		arr = 1;
	}
	TIM2->ARR = (uint16_t) arr;
}

// set speed for motor B (TIM3)
void stepperBSetSpeed(uint32_t freqHz) {

	if (freqHz == 0) {

		stepperABDisable();
		return;
	}
	stepperABEnable();
	if (freqHz < MIN_FREQ) {

		freqHz = MIN_FREQ;
	}
	if (freqHz > MAX_FREQ) {

		freqHz = MAX_FREQ;
	}
	uint32_t arr = (1000000UL / (2 * freqHz)) - 1;
	if (arr > 0xFFFF) {

		arr = 0xFFFF;
	}
	if (arr < 1) {

		arr = 1;
	}
	TIM3->ARR = (uint16_t) arr;
}

// choose direction with DIR pin
void stepperASetDir(bool dir) {

	if (!dir) {

		GPIOA->BSRR = GPIO_BSRR_BS4;
	} else {

		GPIOA->BSRR = GPIO_BSRR_BR4;
	}
}

void stepperBSetDir(bool dir) {

	if (dir) {

		GPIOB->BSRR = GPIO_BSRR_BS4;
	} else {

		GPIOB->BSRR = GPIO_BSRR_BR4;
	}
}

void TIM2_IRQHandler(void) {

	if (TIM2->SR & TIM_SR_UIF) {

		TIM2->SR &= ~TIM_SR_UIF;
		GPIOA->ODR ^= GPIO_ODR_ODR5;
	}
}

void TIM3_IRQHandler(void) {

	if (TIM3->SR & TIM_SR_UIF) {

		TIM3->SR &= ~TIM_SR_UIF;
		GPIOB->ODR ^= GPIO_ODR_ODR5;
	}
}

void stepperABMove(uint32_t freqHz, bool dir) {

	stepperASetSpeed(freqHz);
	stepperBSetSpeed(freqHz);
	stepperASetDir(dir);
	stepperBSetDir(dir);
}
