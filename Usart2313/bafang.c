#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "bafang.h"
#include "timers.h"

uint8_t packet[BUFFER_SIZE];
volatile uint8_t state = STATE_WAITING;
volatile uint8_t bfState = BAFANG_STATE_IDLE;
volatile uint8_t *ptr = packet;
volatile uint8_t len = 0;

uint8_t calcCheckSum( uint8_t *data, uint8_t len ) {
	uint16_t crc = 0;
	uint8_t *d = data;
	int i;
	
	for( i=0; i<len; i++ ) {
		crc += *d;
		d++;
	}
	
	crc = crc % 256;
	return (uint8_t)crc;
}

void sendReadCmd( uint8_t cmd ) {
	uint8_t buffer[2];
	buffer[0] = BAFANG_READ;
	buffer[1] = cmd;
	USART_putbuf( buffer, sizeof(buffer) );
}

void bafangState() {
	bafangPacket_t *pkt = (bafangPacket_t *)&packet;
	if( pkt->header.cmd == CMD_PEDAL ) {
		bfState = BAFANG_STATE_PEDALR;
	}
}

void bafangReset() {
	state = STATE_WAITING;
	ptr = packet;
	len = 0;
	USART_reset();
}

void bafangIdle() {
	if( state == STATE_HANDLE ) {
		bafangState();
		bafangReset();
	}
	
	if( rx_head == rx_tail ) {
		return;
	}
	
	uint8_t tmp_tail = (rx_tail + 1) % BUFFER_SIZE;
	*ptr = rx_buffer[rx_tail];
	rx_tail = tmp_tail;
	
	switch( state ) {
		case STATE_WAITING:
			if( *ptr == CMD_PEDAL ) {
				ptr++;
				state = STATE_LENGTH;
			}
			return;
		
		case STATE_LENGTH:
			len = *ptr;
			
			// prevent overflow
			if( (len + sizeof(bafangHeader_t) + 1) > sizeof(packet) ) {
				bafangReset();
				return;
			}
			
			ptr++;
			state = STATE_DATA;
			return;
		
		case STATE_DATA:
			if( len == 0x0 ) {
				state = STATE_HANDLE;
			}
			else {
				len = len - 1;
			}
			
			ptr++;
			return;
			
		case STATE_WRITE_RESP:
			// dispose of write response which has same cmd type 0x53 as pedal read
			len--;
			if( len == 0 ) {
				stopModal();
				state = STATE_WAITING;
				bafangReset();
			}
			return;
	}
}