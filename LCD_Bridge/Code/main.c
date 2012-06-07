//#include "msp430.h"
#include <msp430x22x2.h>
#include "adc_ads.h"
#include "spi.h"
#include "batt.h"
#include "can.h"
#include "itoa.h"

#include "main.h"

//
// Function Declarations
//
void timer_init(void);

//
// Initialize port pins
//
void initPortPins(void)
{
  P1DIR = 0xFF;								// no inputs needed
  P2DIR = ~(PIN2+PIN1);                		// Set P2.2,1 as input
  P3DIR = ~(PIN5+PIN2);							// Set P3.5,2 as an input
  P4DIR = (char)~(0x82); //~(PIN7+PIN1);						// Set P4.7,1 as input
  P3SEL = PIN1 + PIN2 + PIN3 + PIN4 + PIN5;
  P3OUT = 0x00;

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

void tx ( void )
{
		can_push_ptr->address =  BMS_S1_CAN_BASE;
		can_push_ptr->data.data_u16[0] = 123;
		can_push_ptr->data.data_u16[1] = 456;
		can_push_ptr->data.data_u16[2] = 789;
		can_push_ptr->status = 6;
		can_push();
	  can_transmit();
}

void clock_init (void)
{
	DCOCTL = CALDCO_12MHZ;
	BCSCTL1 = CALBC1_12MHZ;
	//BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
}


void main(void)
{
	char RPM[5] = {'0','0','0','0','0'};
	char CUR[3] = {'0','0','0'};
	char REG[3] = {'0','0','0'};
	char VOLT[4] = {'0','0','0','0'};
	char TEMP[2] = {'0','0'};
	unsigned int i;
	
	unsigned int a;

	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	//Delay to allow 3.3V rail to fully rise, essential for CAN part
	//because MSP430 will turn on at 1.8V or less
	//Especially is 3.3V voltage supervisor is not installed!
	for(i=0; i<65000; i++)
		asm("nop");
	
	clock_init();
	__enable_interrupt();                     // Enable interrupt
	
  initPortPins();                           // Initialize port pins
//  timer_init();
//  ADC_init();
//  DAC_init();
  uart_init();
  spi_init();
  spi_set_mode ( UCCKPH, 0, 5 );
  can_init(CAN_BITRATE_250);
  
  //can_sleep();
  //can_wake();
  
  while(1)
  {
  	//Check if a CAN message has arrived
  	if( IO_CAN_INT ) //interrupt is low, message has arrived
  	{
  		can_receive();
  		if( can.status != CAN_ERROR )
  		{
  			if( can.address == (BMS_S1_CAN_BASE+BMS_STAT) )
  			{
  				a = can.data.data_u16[0] / 10;
  				itoa ( a, VOLT, 10 );

  				a = can.data.data_u16[1];
  				//itoa ( a, TEMP, 10 );


  				uart_transmit_string ( RPM, 5 );
  				uart_transmit(',');
  				uart_transmit_string ( CUR, 3 );
  				uart_transmit(',');
  				uart_transmit_string ( REG, 3 );
  				uart_transmit(',');
  				uart_transmit_string ( VOLT, 4 );
  				uart_transmit(',');
  				uart_transmit('0');
  				uart_transmit('0');
  				//uart_transmit('\n');
  			}
  		}
  	}
  }
}

// Timer Interrupt Service Routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  //P2OUT ^= PIN2+PIN1;                       // Toggle P2.2,1

  LPM0_EXIT;
}

#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B (void)
{
  //P4OUT ^=  PIN4;
}

// Initialize TimerA to wake up processor at 2kHz
void timer_init(void)
{
  
  CCR0 = 1483;
  TACTL = TASSEL_2 + ID_3 + MC_1;                  // SMCLK, upmode
  
  TBCCR0 = 1058;
  TBCTL = TASSEL_2 + ID_1;

//  TBCCR0 = 1058;
//  TBCTL = TASSEL_2 + ID_1;
  
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  TBCCTL0 = CCIE;                             // CCR0 interrupt enabled
}
