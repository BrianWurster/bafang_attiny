#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "bafang.h"
#include "timers.h"
#include "debounce.h"

#define DISPLAY_SET	(0xff)
#define MPH20		(0x20)
#define MPH25		(0x28)

#define INITIAL_READ	(0)
#define STD_READ		(1)
#define VERIFY_READ		(2)

uint8_t readState = INITIAL_READ;
uint8_t readSpeed = 0;
volatile uint8_t writeSpeed = 0;

// TODO: debounce, investigate wrong values
void captureRotarySwitch() {
	if( PINB & (1<<PINB2) ) {
		writeSpeed = MPH20;
	}
	if( PINB & (1<<PINB1) ) {
		writeSpeed = MPH25;
	}
	if( PINB & (1<<PINB0) ) {
		writeSpeed = DISPLAY_SET;
	}
}

ISR(PCINT0_vect) {
	captureRotarySwitch();
}

void init() {
	// set leds as outputs
	DDRD |= (1<<3)|(1<<4)|(1<<5);
	PORTD &= ~(1<<3);
	PORTD &= ~(1<<4);
	PORTD &= ~(1<<5);
	
	PCMSK |= (1<<PCINT0)|(1<<PCINT1)|(1<<PCINT2);	// pin change mask
	GIMSK |= (1<<PCIE0);							// enable PCINT interrupt
	
	initTimer0();
	initTimer1();
	USART_Init( UBRR );
	
	_delay_ms(250);
	captureRotarySwitch();
	sendReadCmd( CMD_PEDAL );
	
	// Enable interrupts
	sei();
}

void updateSpeed( uint8_t speed ) {
	readSpeed = speed;
	
	// display set
	if( readSpeed == DISPLAY_SET ) {
		PORTD |= (1<<3);
	}
	// 20mph
	else if( readSpeed == MPH20 ) {
		PORTD |= (1<<4);
	}
	// 25mph
	if( readSpeed == MPH25 ) {
		PORTD |= (1<<5);
	}
}

int main( void ) {
	bfReadPedalCmd_t *pkt = (bfReadPedalCmd_t *)&packet;
	
	init();
	
    while( 1 ) {
		if( bfState == BAFANG_STATE_PEDALR ) {
			USART_putbuf( packet, packet[1] + (sizeof(bafangHeader_t) + 1) ); // 2-byte header + crc
			
			if( readState == INITIAL_READ ) {
				readState = STD_READ;
				updateSpeed( pkt->speedLimit );
				bfState = BAFANG_STATE_IDLE;
			}
			else if( readState == VERIFY_READ ) {
				readState = STD_READ;
				stopModal();
				updateSpeed( pkt->speedLimit );
				bfState = BAFANG_STATE_IDLE;
			} else {
				readState = VERIFY_READ;
				bfState = BAFANG_STATE_PEDALW;
			}
		}
		else if( bfState == BAFANG_STATE_PEDALW ) {
			pkt->speedLimit = writeSpeed;
			pkt->checkSum = calcCheckSum( packet, packet[1]+sizeof(bafangHeader_t) );
			
			state = STATE_WRITE_RESP;
			len = 3;
			
			USART_Transmit( BAFANG_WRITE );
			USART_putbuf( packet, packet[1] + (sizeof(bafangHeader_t) + 1) ); // 2-byte header + crc
			
			bfState = BAFANG_STATE_IDLE;
		}
		
		if( isSwitchReady() ) {
			if( bfState == BAFANG_STATE_IDLE ) {
				startModal();
				sendReadCmd( CMD_PEDAL );
			}
			setReady( 0 );
		}
	
		bafangIdle();
    }
}