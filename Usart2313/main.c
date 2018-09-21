#include <avr/io.h>
#include "usart.h"

void init() {
	USART_Init( UBRR );
}

int main( void ) {
	init();
	
    while( 1 ) {
		for( uint8_t i=0; i<0xFF; i++ ) {
			USART_Transmit( i );
		}
    }
}