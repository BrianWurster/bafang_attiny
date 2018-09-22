#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "bafang.h"
#include "timers.h"
#include "debounce.h"

void init() {
	initTimer1();
	USART_Init( UBRR );
	
	// Enable interrupts
	sei();
}

int main( void ) {
	init();
	
    while( 1 ) {
		if( isSwitchReady() ) {
			setReady( 0 );
		}
	
		bafangIdle();
		
		/*if( rx_head == rx_tail ) {
			continue;
		}
		
		uint8_t tmp_tail = (rx_tail + 1) % BUFFER_SIZE;
		USART_putc( rx_buffer[rx_tail] );
		rx_tail = tmp_tail;*/
    }
}