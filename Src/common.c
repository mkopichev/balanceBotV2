#include "../Inc/common.h"

uint8_t cpuFreqSourceChoosen;

bool cpuFreqInit(uint8_t clockSource) {

	uint32_t timeout;

	switch (clockSource) {
	case HSI_SOURCE: // 8 MHz RC

		RCC->CR |= RCC_CR_HSION; // HSI on
		timeout = TIMEOUT;
		while (!(RCC->CR & RCC_CR_HSIRDY) && timeout--) {

			__NOP();
		}
		if (timeout == 0) {

			return false;
		}

		FLASH->ACR = FLASH_ACR_PRFTBE; // 0 wait states for FLASH

		RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2); // clear all prescalers

		RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI; // choose HSI as system clock
		timeout = TIMEOUT;
		while (((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) && timeout--) {

			__NOP();
		}
		if (timeout == 0) {

			return false;
		}

		cpuFreqSourceChoosen = HSI_SOURCE;
		break;

	case HSE_SOURCE: // 8 MHz QUARTZ

		RCC->CR |= RCC_CR_HSEON; // HSE on
		timeout = TIMEOUT;
		while (!(RCC->CR & RCC_CR_HSERDY) && timeout--) {

			__NOP();
		}
		if (timeout == 0) {

			return false;
		}

		FLASH->ACR = FLASH_ACR_PRFTBE; // 0 wait states for FLASH

		RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2); // clear all prescalers

		RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSE; // choose HSe as system clock
		timeout = TIMEOUT;
		while (((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) && timeout--) {

			__NOP();
		}
		if (timeout == 0) {

			return false;
		}

		RCC->CR &= ~RCC_CR_HSION; // HSI off

		cpuFreqSourceChoosen = HSE_SOURCE;
		break;

	case PLL_SOURCE: // 72 MHz

		RCC->CR |= RCC_CR_HSEON; // HSE on
		timeout = TIMEOUT;
		while (!(RCC->CR & RCC_CR_HSERDY) && timeout--) {

			__NOP();
		}
		if (timeout == 0) {

			return false;
		}

		RCC->CR &= ~RCC_CR_PLLON; // PLL off

		timeout = TIMEOUT;
		do {

			FLASH->ACR = FLASH_ACR_LATENCY_2 | FLASH_ACR_PRFTBE; // latency and prefetch for flash
		} while (((FLASH->ACR & (FLASH_ACR_LATENCY)) != FLASH_ACR_LATENCY_2)
				&& timeout--);
		if (timeout == 0) {

			return false;
		}

		RCC->CFGR &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2); // clear all prescalers
		RCC->CFGR |= RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2
				| RCC_CFGR_PPRE2_DIV1;

		RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL);
		RCC->CFGR |= RCC_CFGR_PLLMULL9 | (1 << RCC_CFGR_PLLSRC_Pos); // multiply by 9 and choose HSE source

		RCC->CR |= RCC_CR_PLLON; // PLL 72 MHz
		timeout = TIMEOUT;
		while (!(RCC->CR & RCC_CR_PLLRDY) && timeout--) {

			__NOP();
		}
		if (timeout == 0) {

			return false;
		}

		RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
		timeout = TIMEOUT;
		while (((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) && timeout--) {

			__NOP();
		}
		if (timeout == 0) {

			return false;
		}

		RCC->CR &= ~RCC_CR_HSION; // HSI off

		cpuFreqSourceChoosen = PLL_SOURCE;
		break;

	default:

		break;

	}
	return true;
}
