#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "usart.h"

uint8_t rx_buffer[BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;

//uint8_t tx_buffer[BUFFER_SIZE];
//volatile uint8_t tx_head = 0;
//volatile uint8_t tx_tail = 0;

void USART_Init( unsigned int baud ) {
	// Set baud rate
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	
	// Enable receiver and transmitter
	UCSRB = (1<<RXEN)|(1<<RXCIE)|(1<<TXEN);
	
	// Set frame format: 8N1
	UCSRC = (1<<UCSZ0)|(1<<UCSZ1);
	USART_reset();
}

void USART_reset() {
	cli();
	rx_head = 0;
	rx_tail = 0;
	memset( rx_buffer, 0, sizeof(rx_buffer) );
	sei();
}

uint8_t USART_Transmit( unsigned char data ) {
	// Wait for empty transmit buffer
	while ( !( UCSRA & (1<<UDRE)) );
	
	// Put data into buffer, sends the data
	UDR = data;
	return 1;
}

unsigned char USART_Receive( void ) {
	// Wait for data to be received
	while ( !(UCSRA & (1<<RXC)) );
	
	// Get and return received data from buffer
	return UDR;
}

// Called when USART completes receiving data
ISR( USART_RX_vect ) {
	// Read UDR register
	uint8_t data = UDR;
	uint8_t tmp_head = (rx_head + 1) % BUFFER_SIZE;
	if( tmp_head == rx_tail ) {
		// rx buffer full, drop byte
		return;
	}

	// Check for frame error, data overrun, parity error
	if( (UCSRA & ((1 << FE) | (1 << DOR) | (1 << UPE))) == 0 ) {
		// No error occurred
		rx_buffer[rx_head] = data;
		rx_head = tmp_head;
	}
}

int USART_putbuf( uint8_t* buffer, int len ) {
	int i = 0;
	int sent = 0;
	uint8_t *c = buffer;
	
	 for( i=0; i<len; i++ ) {
		 //if( USART_putc( *c ) ) {
		 if( USART_Transmit( *c ) ) {
			 sent++;
		 } else {
			 break;
		 }
		 c++;
	 }
	
	return sent;
}

// memory requirements are too high for interrupt based TX
/*uint8_t USART_putc( uint8_t c ) {
	uint8_t tmp_head = (tx_head + 1) % BUFFER_SIZE;
	if( tmp_head == tx_tail ) {
		// tx buffer full
		return 0;
	}
	
	tx_buffer[tx_head] = c;
	tx_head = tmp_head;
	UCSRB |= (1<<UDRIE);
	return 1;
}

// Called when the Data Register Empty
ISR( USART_UDRE_vect ) {
	if( tx_tail == tx_head ) {
		// need to disable interrupt if no more data
		UCSRB &= ~(1<<UDRIE); 
		return;
	}
	
	UDR = tx_buffer[tx_tail];
	tx_tail = (tx_tail + 1) % BUFFER_SIZE;
}*/