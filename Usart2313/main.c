#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "bafang.h"

void init() {
	USART_Init( UBRR );
	
	// Enable interrupts
	sei();
}

int main( void ) {
	init();
	
    while( 1 ) {
		bafangIdle();
    }
}