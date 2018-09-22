#include <avr/io.h>
#include <avr/interrupt.h>
#include "timers.h"
#include "debounce.h"

// process the timer1 compare here (1ms)
ISR(TIMER1_COMPA_vect) {
	checkSwitch();
}

/*
f = fclk � (2 � N � (1 + OCR1A))
OCR1A = ( fclk / ( f * 2 * N) ) - 1
Where:
f = timer period Hz ((1/f)*1000)/2 = timer in ms
fclk = system clock frequency Hz
N = prescaler division factor
OCR1A = value in OCR1A register
*/

// 16-bit timer
void initTimer1() {
	//TCCR1B |= (1<<WGM12) | (1<<CS12); // ctc mode, prescaler /256
	TCCR1B |= (1<<WGM12) | (1<<CS12) | (1<<CS10); // ctc mode, prescaler /1024
	
	//OCR1A = 0x24;		  // 5ms resolution @ 1843200 /256
	//OCR1A = 0x06;		  // 1ms resolution @ 1843200 /256
	//OCR1A = 0x0D;		  // 1ms resolution @ 14745600 /1024
	OCR1A = 0x4e;		  // 5ms resolution @ 14745600 /1024
	
	// enable compare interrupt
	TIMSK |= (1 << OCIE1A);
	// initialize counter
	TCNT1 = 0;
}