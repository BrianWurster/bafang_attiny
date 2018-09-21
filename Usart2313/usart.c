#include <avr/io.h>
#include <avr/interrupt.h>
#include "usart.h"

uint8_t rx_buffer[BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;

void USART_Init( unsigned int baud ) {
	// Set baud rate
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	
	// Enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<RXCIE)|(1<<TXEN);
	
	// Set frame format: 8N1
	UCSRC = (1<<UCSZ0)|(1<<UCSZ1);
}

void USART_Transmit( unsigned char data ) {
	// Wait for empty transmit buffer
	while ( !( UCSRA & (1<<UDRE)) );
	
	// Put data into buffer, sends the data
	UDR = data;
}

unsigned char USART_Receive( void ) {
	// Wait for data to be received
	while ( !(UCSRA & (1<<RXC)) );
	
	// Get and return received data from buffer
	return UDR;
}

void USART_Tx() {
	if( rx_head == rx_tail ) {
		return;
	}
	
	uint8_t tmp_tail = (rx_tail + 1) % BUFFER_SIZE;
	USART_Transmit( rx_buffer[rx_tail] );
	rx_tail = tmp_tail;
}

// Called when USART completes receiving data
ISR( USART_RX_vect ) {
	// Read UDR register
	uint8_t data = UDR;
	uint8_t tmp_head = (rx_head + 1) % BUFFER_SIZE;

	// Check for frame error, data overrun, parity error
	if( (UCSRA & ((1 << FE) | (1 << DOR) | (1 << UPE))) == 0 ) {
		// No error occurred
		rx_buffer[rx_head] = data;
		rx_head = tmp_head;
	}
}