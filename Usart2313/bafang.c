#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "bafang.h"

uint8_t packet[BUFFER_SIZE];
uint8_t state = STATE_WAITING;
uint8_t bfState = BAFANG_STATE_IDLE;
uint8_t *ptr = packet;
uint8_t len = 0;

void sendReadCmd( uint8_t cmd ) {
	uint8_t buffer[2];
	buffer[0] = BAFANG_READ;
	buffer[1] = cmd;
	USART_putbuf( buffer, sizeof(buffer) );
}

void bafangState() {
	bafangPacket_t *pkt = (bafangPacket_t *)&packet;
	switch( bfState ) {
		case BAFANG_STATE_IDLE:
			if( pkt->header.cmd == CMD_CONNECT ) {
				bfState = BAFANG_STATE_CONNECTED;
				sendReadCmd( CMD_PEDAL );
			}
			break;
		
		case BAFANG_STATE_CONNECTED:
			if( pkt->header.cmd == CMD_PEDAL ) {
				bfState = BAFANG_STATE_PEDAL;
			}
			break;
	}
}

void bafangIdle() {
	if( state == STATE_HANDLE ) {
		bafangState();
		
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
			if( (len + sizeof(bafangHeader_t) + 1) > sizeof(packet) ) {
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