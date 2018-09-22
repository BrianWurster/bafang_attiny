#include <avr/io.h>
#include "debounce.h"

// debounce switch vars
uint8_t debouncedKeyPress = 0; // holds the debounced state of the key
uint8_t keyPressed = 0;
uint8_t keyChanged = 0;
volatile uint8_t swReady = 0; // set when switch is pressed and debounced
uint8_t timerCount = 0;

// This function reads the key state from the hardware
uint8_t rawKeyPressed() {
	// button is active low
	if( PIND & (1<<BUTTON_PIN) ) {
		return 0;
	}
	return 1;
}

// service routine called every CHECK MSEC to debounce both edges
// key_changed signals the switch has changed from open to closed, or the reverse.
// key_pressed is the current debounced state of the switch.
void debounceSwitch( uint8_t *key_changed, uint8_t *key_pressed ) {

	static uint8_t swCount = RELEASE_MSEC / CHECK_MSEC;
	uint8_t rawState;
	*key_changed = 0;
	*key_pressed = debouncedKeyPress;
	rawState = rawKeyPressed();
	
	if( rawState == debouncedKeyPress ) {
		// set the timer which allows a change from current state
		if( debouncedKeyPress == 1 ) {
			swCount = PRESS_MSEC / CHECK_MSEC;
			} else {
			swCount = RELEASE_MSEC / CHECK_MSEC;
		}
	}
	else {
		// key has changed - wait for new state to become stable
		if( --swCount == 0 ) {
			// timer expired - accept the change
			debouncedKeyPress = rawState;
			*key_changed = 1;
			*key_pressed = debouncedKeyPress;
			
			// reset the timer
			if( debouncedKeyPress == 1 ) {
				swCount = PRESS_MSEC / CHECK_MSEC;
				} else {
				swCount = RELEASE_MSEC / CHECK_MSEC;
			}
		}
	}
}

void checkSwitch() {
	if( timerCount == CHECK_MSEC ) {
		debounceSwitch( &keyChanged, &keyPressed );
		if( keyChanged && keyPressed ) {
			swReady = 1;
		}
		timerCount = 0;
	}
	timerCount++;
}

uint8_t isSwitchReady() {
	return swReady;
}

void setReady( uint8_t ready ) {
	swReady = ready;
}