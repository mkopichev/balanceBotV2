#include "../Inc/controlSystem.h"

extern int32_t motorPosition;
extern uint8_t cpuFreqSourceChoosen;
Controller_t angleControlLoop;
Controller_t positionControlLoop;
uint32_t freq;

void angleControllerInit(Controller_t *controller, float kp, float ki, float kd,
		float min, float max, float deltaTime) {

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

void positionControllerInit(Controller_t *controller, float kp, float ki, float kd,
		float min, float max, float deltaTime) {

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

	positionControllerInit(&positionControlLoop, 0.0001f, 0.00001f, 0.0f, -6.0f, 6.0f, 0.01f);
	angleControllerInit(&angleControlLoop, 10.0f, 100.0f, 0.1f, -100.0f, 100.0f, 0.01f);
}

float angleControllerUpdate(Controller_t *controller, float setpoint,
		float measurement) {

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
		// clamping anti-windup
		if (error * output > 0) {

			controller->integral -= error * controller->dt;
		}
	}
	if (output < controller->outMin) {

		output = controller->outMin;
		// clamping anti-windup
		if (error * output > 0) {

			controller->integral -= error * controller->dt;
		}
	}

	return output;
}

float positionControllerUpdate(Controller_t *controller, float setpoint,
		float measurement) {

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
		// clamping anti-windup
		if (error * output > 0) {

			controller->integral -= error * controller->dt;
		}
	}
	if (output < controller->outMin) {

		output = controller->outMin;
		// clamping anti-windup
		if (error * output > 0) {

			controller->integral -= error * controller->dt;
		}
	}

	return output;
}

void TIM1_UP_IRQHandler(void) {

	TIM1->SR &= ~TIM_SR_UIF;

	static bool startFlag = false;
	float angle = imuGetAngle();
	float angleControlOutput = 0.0f, positionControlOutput = 0.0f;
	if (fabs(angle) < 22.5f) {

		if(!startFlag) {

			startFlag = true;
			angleControlLoop.integral = 0.0f;
			positionControlLoop.integral = 0.0f;
			stepperABEnable();
		}
		positionControlOutput = positionControllerUpdate(&positionControlLoop, 0.0f, motorPosition);
		angleControlOutput = angleControllerUpdate(&angleControlLoop, positionControlOutput, angle);
	} else {

		startFlag = false;
		motorPosition = 0;
		stepperABDisable();
		return;
	}
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
