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
		
		if( isSwitchReady() ) {
			if( bfState == BAFANG_STATE_IDLE ) {
				sendReadCmd( CMD_PEDAL );
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
			setReady( 0 );
		}
	
		bafangIdle();
    }
}