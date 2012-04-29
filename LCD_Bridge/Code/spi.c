/*
 * SPI.c
 */

// Include files
#include <msp430x22x2.h>
#include "spi.h"

/*
 * Initialise SPI port
 * 	- Master, 8 bits, CKPH=1, CKPL=0, 3 wire
 */
void spi_init( void )
{
	UCB0CTL1 |= UCSWRST; //Hold peripheral in reset
	UCB0CTL0 |= UCCKPH + UCMSB + UCMST + UCSYNC;
	UCB0CTL1 |= UCSSEL_2; //SMCLK
	UCB0BR0 = 5; //prescale bit clock by 1
	UCB0BR1 = 0; //prescale bit clock by 0*256 = 0
	IE2 &= ~(UCB0TXIE | UCB0RXIE); //clear all interrupts
	
	UCB0CTL1 &= ~UCSWRST;						// SPI enable
}

void spi_set_mode ( unsigned char mode, unsigned char br1, unsigned char br0 )
{
	UCB0CTL1 |= UCSWRST; //Hold peripheral in reset
	UCB0CTL0 = mode + UCMSB + UCMST + UCSYNC;
	UCB0BR0 = br0; //prescale bit clock by 1
	UCB0BR1 = br1; //prescale bit clock by 0*256 = 0
	UCB0CTL1 &= ~UCSWRST;						// SPI enable
}

/*
 * Transmits data on SPI connection
 *	- Busy waits until entire shift is complete
 *	- On devices with hardware SPI support, this function is identical to spi_exchange,
 *	  with the execption of not returning a value
 *	- On devices with software (bit-bashed) SPI support, this function can run faster
 *	  because it does not require data reception code
 */
void spi_transmit( unsigned char data )
{
	UCB0TXBUF = data;
	while(( IFG2 & UCB0RXIFG ) == 0x00 );			// Wait for Rx completion (implies Tx is also complete)
	UCB0RXBUF;
}

/*
 * Exchanges data on SPI connection
 *	- Busy waits until entire shift is complete
 *	- This function is safe to use to control hardware lines that rely on shifting being finalised
 */
unsigned char spi_exchange( unsigned char data )
{
	while (!(IFG2 & UCB0TXIFG));            // USCI_B0 TX buffer ready?
	UCB0TXBUF = data;
	while(( IFG2 & UCB0RXIFG ) == 0x00 );			// Wait for Rx completion (implies Tx is also complete)
	return( UCB0RXBUF );
}
