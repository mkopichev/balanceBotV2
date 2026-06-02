#include "../Inc/controlSystem.h"

extern uint8_t cpuFreqSourceChoosen;
Controller_t balanceBotCtrl;
float pidOutput;

void controllerInit(Controller_t *controller, float kp, float ki, float kd,
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
	TIM1->ARR = 1000; // 10 ms period
	TIM1->DIER = TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM1_UP_IRQn);
	TIM1->CR1 = TIM_CR1_CEN;

	controllerInit(&balanceBotCtrl, 0.3f, 0.0f, 0.0f, -100.0f, 100.0f,
			10.0f / 1000.0f);
}

float controllerUpdate(Controller_t *controller, float setpoint,
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
	float angle = imuGetAngle();
	float output = controllerUpdate(&balanceBotCtrl, 0.0f, angle);

	uint32_t freq = 0; // convert pidout to step frequency
	bool direction = (output < 0) ? false : true;
	float absOut = fabs(output);
	const float deadband = 0.5f;
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
