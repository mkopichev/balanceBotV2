#include "../Inc/delayMs.h"

extern uint8_t cpuFreqSourceChoosen;
static bool sysTickInitDone = false;
uint32_t msCounter = 0;

void sysTickInit(void) {

	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk
			| SysTick_CTRL_ENABLE_Msk;
	switch (cpuFreqSourceChoosen) {
	case HSI_SOURCE:
		SysTick->LOAD = 8000 - 1;
		break;
	case HSE_SOURCE:
		SysTick->LOAD = 8000 - 1;
		break;
	case PLL_SOURCE:
		SysTick->LOAD = 72000 - 1;
		break;
	}
	SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
	sysTickInitDone = true;
}

void delayMs(uint32_t value) {

	if (!sysTickInitDone) {

		sysTickInit();
	}
	msCounter = 0;
	while (msCounter < value) {

		__NOP();
	}
}

void SysTick_Handler(void) { // happens once in 1 ms

	msCounter++;
}
