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

#define IO_CAN_INT		(!(P4IN & PIN1))

#endif /*MAIN_H_*/
