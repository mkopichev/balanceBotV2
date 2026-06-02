#ifndef STEPPERMOTORS_H_
#define STEPPERMOTORS_H_

#include "common.h"

#define MIN_FREQ 200UL
#define MAX_FREQ 20000UL

void stepperABEnable(void);
void stepperABDisable(void);
void stepperABInit(void);
void stepperASetSpeed(uint32_t freqHz);
void stepperBSetSpeed(uint32_t freqHz);
void stepperASetDir(bool dir);
void stepperBSetDir(bool dir);
void stepperABMove(uint32_t freqHz, bool dir);

#endif /* STEPPERMOTORS_H_ */
