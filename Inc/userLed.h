#ifndef USERLED_H_
#define USERLED_H_

#include "common.h"

#define LED_ON true
#define LED_OFF false
#define LED_C13	13
#define LED_C14 14

void userLedsInit(void);
void userLedToggle(uint8_t ledChoice, bool state);

#endif /* USERLED_H_ */
