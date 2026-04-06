#ifndef USERLED_H_
#define USERLED_H_

#include "common.h"

#define LED_ON true
#define LED_OFF false

void userLedInit(void);
void userLedToggle(bool state);

#endif /* USERLED_H_ */
