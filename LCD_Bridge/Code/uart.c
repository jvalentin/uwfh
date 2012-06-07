
// Include files
//#include "msp430.h"
#include <msp430x22x2.h>
#include "uart.h"

/*
 * Initialise UART
 * 	- 9600 baud, assuming 16 MHz clock
 *  - uses UCA peripheral (not B)
 * 	- Clock: SMCLK (works with no external clock input, DCO only), assumed to be 1 MHz
 */
void uart_init( void )
{
	UCA0CTL1 |= UCSSEL_2;                     // SMCLK
	
	//UCA0BR0 = 0x23;  						  // 4MHz 115200
	//UCA0BR1 = 0x00;                           // 4MHz 115200
	//UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
	
	//UCA0BR0  = 0xE2; 	//12 MHz, 9600
	//UCA0BR1  = 0x04; 	//12 MHz, 9600
	//UCA0MCTL = 0x00; 	//12 MHz, 9600
	UCA0BR0  = 0x71; //12 MHz, 19200
	UCA0BR1  = 0x02; //12 MHz, 19200
	UCA0MCTL = 0x00; //12 MHz, 19200
	//UCA0BR0  = 0x68; 	//12 MHz, 115200
	//UCA0BR1  = 0x00; 	//12 MHz, 115200
	//UCA0MCTL = 0x08; 	//12 MHz, 115200
	
	UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  
	IE2 &= 0xFC; 							  //clear RX/TX interrupts
	IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
}

/*
 * Transmits data on UART connection
 *	- Busy waits until transfer is complete
 */
void uart_transmit( unsigned char data )
{
	UCA0TXBUF = data;
	while ( ( UCA0STAT & UCBUSY ) ); // Wait for TX
}

unsigned char uart_receive( void )
{
	return UCA0RXBUF;
}


void uart_transmit_string( unsigned char* data, unsigned int len )
{
	unsigned int i;

	for ( i = 0; i < len; i++ )
	{
		uart_transmit(data[i]);
	}
}
