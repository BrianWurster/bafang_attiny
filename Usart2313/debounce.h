#ifndef DEBOUNCE_H_
#define DEBOUNCE_H_

#define BUTTON_PIN		(2)		// PORTD

#define CHECK_MSEC		(5)		// Read hardware every 5 msec
#define PRESS_MSEC		(10)	// Stable time before registering pressed
#define RELEASE_MSEC	(20)	// Stable time before registering released

void checkSwitch();
uint8_t isSwitchReady();
void setReady( uint8_t ready );

#endif /* DEBOUNCE_H_ */