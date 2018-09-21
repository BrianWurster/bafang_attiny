#include <avr/io.h>
#include <avr/interrupt.h>
#include "bafang.h"
#include "usart.h"

uint8_t packet[32];
uint8_t state = STATE_WAITING;
uint8_t *ptr = packet;
uint8_t len = 0;

void bafangIdle() {
	if( state == STATE_HANDLE ) {
		USART_putbuf( packet, packet[1] + 3 ); // 2-byte header + crc
		
		state = STATE_WAITING;
		ptr = packet;
		len = 0;
	}
	
	if( rx_head == rx_tail ) {
		return;
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
			return;
		
		case STATE_LENGTH:
			len = *ptr;
			
			// prevent overflow
			if( len + 3 > sizeof(packet) ) {
				state = STATE_WAITING;
				ptr = packet;
				len = 0;
				return;
			}
			
			ptr++;
			state = STATE_DATA;
			return;
		
		case STATE_DATA:
			ptr++;
			
			if( len == 0x0 ) {
				state = STATE_HANDLE;
			}
			else {
				len = len - 1;
			}
			return;
	}
}