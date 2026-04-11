#ifndef STEPPERMOTORS_H_
#define STEPPERMOTORS_H_

#include "common.h"

#define MIN_FREQ 100UL
#define MAX_FREQ 20000UL
#define SETPOINT_MULTIPLIER (MAX_FREQ / 255UL)

void stepperABEnable(void);
void stepperABDisable(void);
void stepperABInit(void);
void stepperASetSpeed(uint8_t setpoint);
void stepperBSetSpeed(uint8_t setpoint);
void stepperASetDir(bool dir);
void stepperBSetDir(bool dir);
void stepperABMove(uint8_t setpoint, bool dir);

#endif /* STEPPERMOTORS_H_ */
