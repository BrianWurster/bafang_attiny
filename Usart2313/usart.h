#ifndef USART_H_
#define USART_H_

#define BAUDRATE	(1200)
#define UBRR		((F_CPU / (BAUDRATE * 16L)) - 1)
#define BUFFER_SIZE	(32)

extern uint8_t rx_buffer[BUFFER_SIZE];
extern volatile uint8_t rx_head;
extern volatile uint8_t rx_tail;

void USART_Init( unsigned int baud );
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void USART_Tx();

#endif /* USART_H_ */