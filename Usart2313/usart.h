#ifndef USART_H_
#define USART_H_

#define BAUDRATE	(1200)
#define UBRR		((F_CPU / (BAUDRATE * 16L)) - 1)
#define BUFFER_SIZE	(28)

extern uint8_t rx_buffer[BUFFER_SIZE];
extern volatile uint8_t rx_head;
extern volatile uint8_t rx_tail;

void USART_Init( unsigned int baud );
uint8_t USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
int USART_putbuf( uint8_t* buffer, int len );
//uint8_t USART_putc( uint8_t c );

#endif /* USART_H_ */