#ifndef CONTROLSYSTEM_H_
#define CONTROLSYSTEM_H_

#include "common.h"

typedef struct Controller_t {

	float kp, ki, kd;
	float integral;
	float prevError;
	float outMin;
	float outMax;
	float dt;
	float integralLim;
} Controller_t;

void controlSystemInit(void);

#endif /* CONTROLSYSTEM_H_ */
