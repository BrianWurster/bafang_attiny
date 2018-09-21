#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "bafang.h"

uint8_t packet[32];
uint8_t state = STATE_WAITING;

void init() {
	USART_Init( UBRR );
	
	// Enable interrupts
	sei();
}

int main( void ) {
	uint8_t *ptr = packet;
	uint8_t len = 0;
	
	init();
	
    while( 1 ) {
		if( state == STATE_HANDLE ) {
			state = STATE_WAITING;
			USART_putbuf( packet, packet[1] + 3 ); // 2-byte header + crc
			ptr = packet;
		}
		
		if( rx_head == rx_tail ) {
			continue;
		}
		
		uint8_t tmp_tail = (rx_tail + 1) % BUFFER_SIZE;
		*ptr = rx_buffer[rx_tail];
		rx_tail = tmp_tail;
		
		switch( state ) {
			case STATE_WAITING:
				if( *ptr == CMD_CONNECT || *ptr == CMD_PEDAL ) {
					ptr++;
					state = STATE_LENGTH;
				}
				break;
			
			case STATE_LENGTH:
				len = *ptr;
				ptr++;
				state = STATE_DATA;
				break;
			
			case STATE_DATA:
				ptr++;
				
				if( len == 0x0 ) {
					state = STATE_HANDLE;
				}
				else {
					len = len - 1;
				}
				break;
		}
    }
}