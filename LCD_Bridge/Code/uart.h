#ifndef UART_H_
#define UART_H_

//Public variables
//none?


//Public function prototypes

/*
 * Initialise UART
 * 	- 9600 baud, assuming 1 MHz clock
 *  - uses UCA peripheral (not B)
 * 	- Clock: SMCLK (works with no external clock input, DCO only), assumed to be 1 MHz
 */
void uart_init( void );

/*
 * Transmits data on UART connection
 *	- Busy waits until transfer is complete
 */
void uart_transmit( unsigned char data );
void uart_transmit_string( unsigned char* data, unsigned int len );
/*
 * Returns the current value in the UART buffer
 */
unsigned char uart_receive( void );

#endif /*UART_H_*/
