#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>
#include <stdbool.h>
#include <stepperMotors.h>
#include <stdbool.h>
#include "stm32f103xb.h"
#include "userLed.h"
#include "delayMs.h"

#define HSI_SOURCE 0
#define HSE_SOURCE 1
#define PLL_SOURCE 2
#define TIMEOUT 1000000UL

bool cpuFreqInit(uint8_t clockSource);

#endif /* COMMON_H_ */
