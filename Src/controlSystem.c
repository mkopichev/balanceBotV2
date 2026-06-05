#include "../Inc/controlSystem.h"

extern int32_t motorPosition;
extern uint8_t cpuFreqSourceChoosen;
Controller_t anglePID;
Controller_t positionPID;
uint32_t freq;

void pidInit(Controller_t *controller, float kp, float ki, float kd, float min,
		float max, float deltaTime) {

	controller->kp = kp;
	controller->ki = ki;
	controller->kd = kd;
	controller->integral = 0.0f;
	controller->prevError = 0.0f;
	controller->outMin = min;
	controller->outMax = max;
	controller->dt = deltaTime;
	controller->integralLim = 1.0f * max;
}

float pidUpdate(Controller_t *controller, float setpoint, float measurement) {

	float error = setpoint - measurement;

	// proportional part
	float P = controller->kp * error;

	// integral part
	controller->integral += controller->ki * error * controller->dt;
	if ((controller->integral > controller->integralLim)
			|| (controller->integral < -controller->integralLim)) {

		(controller->integral > controller->integralLim) ?
				(controller->integral = controller->integralLim) :
				(controller->integral = -controller->integralLim);
	}
	float I = controller->integral;

	// differential part
	float D = controller->kd * (error - controller->prevError) / controller->dt;
	controller->prevError = error;

	float output = P + I + D;

	if (output > controller->outMax) {

		output = controller->outMax;
	}
	if (output < controller->outMin) {

		output = controller->outMin;
	}
	return output;
}

void controlSystemInit(void) {

	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

	switch (cpuFreqSourceChoosen) {
	case HSI_SOURCE:
		TIM1->PSC = 80 - 1;
		break;
	case HSE_SOURCE:
		TIM1->PSC = 80 - 1;
		break;
	case PLL_SOURCE:
		TIM1->PSC = 720 - 1; // 100 kHz
		break;
	}
	TIM1->ARR = 1000; // 10 ms timer period
	TIM1->DIER = TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM1_UP_IRQn);
	NVIC_SetPriority(TIM1_UP_IRQn, 15);
	TIM1->CR1 = TIM_CR1_CEN;

	pidInit(&positionPID, 0.0001f, 0.00001f, 0.0f, -6.0f, 100.0f, 0.01f);
	pidInit(&anglePID, 10.0f, 100.0f, 0.1f, -100.0f, 100.0f, 0.01f);
}

void TIM1_UP_IRQHandler(void) {

	TIM1->SR &= ~TIM_SR_UIF;

	static bool startFlag = false;
	float angle = imuGetAngle();
	float angleControlOutput = 0.0f, positionControlOutput = 0.0f;
	if (fabs(angle) < 22.5f) {

		if (!startFlag) {

			startFlag = true;
			anglePID.integral = 0.0f;
			positionPID.integral = 0.0f;
			anglePID.prevError = 0.0f;
			positionPID.prevError = 0.0f;
			stepperABEnable();
		}
	} else {

		startFlag = false;
		motorPosition = 0;
		stepperABDisable();
		return;
	}

	// outer loop
	positionControlOutput = pidUpdate(&positionPID, 0.0f,
			(float) motorPosition);
	// inner loop
	angleControlOutput = pidUpdate(&anglePID, positionControlOutput, angle);

	uartSendData(angle, angleControlOutput, 3);

	freq = 0; // convert pidout to step frequency
	bool direction = (angleControlOutput < 0) ? false : true;
	float absOut = fabs(angleControlOutput);
	const float deadband = 0.2f;
	if (absOut > deadband) {

		float t = (absOut - deadband) / (100.0f - deadband);
		if (t > 1.0f) {

			t = 1.0f;
		}
		freq = MIN_FREQ + (uint32_t) (t * (MAX_FREQ - MIN_FREQ));
	} else {

		freq = 0;
	}
	stepperABMove(freq, direction);
}
