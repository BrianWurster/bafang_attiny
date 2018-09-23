#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "bafang.h"
#include "timers.h"
#include "debounce.h"

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
	
	// Enable interrupts
	sei();
}

int main( void ) {
	bfReadPedalCmd_t *pkt = (bfReadPedalCmd_t *)&packet;
	
	init();
	
    while( 1 ) {
		if( bfState == BAFANG_STATE_PEDALR ) {
			USART_putbuf( packet, packet[1] + (sizeof(bafangHeader_t) + 1) ); // 2-byte header + crc
			bfState = BAFANG_STATE_PEDALW;
		}
		else if( bfState == BAFANG_STATE_PEDALW ) {
			pkt->speedLimit = 0xff;
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

void setLedToRotarySwitch() {
	if( PINB & (1<<PINB2) ) {
		PORTD &= ~(1<<3);
	} else {
		PORTD |= (1<<3);
	}
	
	if( PINB & (1<<PINB1) ) {
		PORTD &= ~(1<<4);
	} else {
		PORTD |= (1<<4);
	}
	
	if( PINB & (1<<PINB0) ) {
		PORTD &= ~(1<<5);
	} else {
		PORTD |= (1<<5);
	}
}

ISR(PCINT0_vect) {
	setLedToRotarySwitch();
}