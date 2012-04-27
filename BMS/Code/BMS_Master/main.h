#ifndef MAIN_H_
#define MAIN_H_

#include "bits.h"

#define IO_ENERGIZE	(!(P4IN & PIN7))

#define IO_POS_ON	P1OUT |= PIN1;
#define IO_POS_OFF	P1OUT &= ~PIN1;

#define IO_NEG_ON	P1OUT |= PIN2;
#define IO_NEG_OFF	P1OUT &= ~PIN2;

#define IO_PRE_ON	P1OUT |= PIN3;
#define IO_PRE_OFF	P1OUT &= ~PIN3;

#define IO_STROBE_ON	P4OUT |= PIN5;
#define IO_STROBE_OFF	P4OUT &= ~PIN5;

#define IO_RESET_CAR	P4OUT |= PIN6;
#define IO_CAR_OK		P4OUT &= ~PIN6;

#define IO_STK1_CS_DIS	P3OUT |= PIN0;
#define IO_STK1_CS_EN	P3OUT &= ~PIN0;

#define IO_STK2_CS_DIS	P4OUT |= PIN2;
#define IO_STK2_CS_EN	P4OUT &= ~PIN2;

#define IO_CAN_CS_DIS	P4OUT |= PIN0;
#define IO_CAN_CS_EN	P4OUT &= ~PIN0;

#define IO_ADC_CS_DIS	P4OUT |= PIN3;
#define IO_ADC_CS_EN	P4OUT &= ~PIN3;

//
// Initialize port pins
//
inline void initPortPins(void)
{
  P1DIR = 0xFF;								// no inputs needed
  P2DIR = ~(PIN2+PIN1);                		// Set P2.2,1 as input
  P3DIR = ~(PIN5+PIN2);							// Set P3.5,2 as an input
  P4DIR = ~(0x82); //~(PIN7+PIN1);						// Set P4.7,1 as input
  P3SEL = PIN1 + PIN2 +PIN3;

	IO_POS_OFF;
	IO_NEG_OFF;
	IO_PRE_OFF;
	IO_STROBE_ON;
	IO_CAR_OK;
	IO_STK1_CS_DIS;
	IO_STK2_CS_DIS;
	IO_CAN_CS_DIS;
	IO_ADC_CS_DIS;
}

#endif /*MAIN_H_*/
