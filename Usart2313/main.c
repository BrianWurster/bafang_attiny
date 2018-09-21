#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

void init() {
	USART_Init( UBRR );
	
	// Enable interrupts
	sei();
}

int main( void ) {
	init();
	
    while( 1 ) {
		if( rx_head == rx_tail ) {
			continue;
		}
		
		uint8_t tmp_tail = (rx_tail + 1) % BUFFER_SIZE;
		USART_Transmit( rx_buffer[rx_tail] );
		rx_tail = tmp_tail;
    }
}