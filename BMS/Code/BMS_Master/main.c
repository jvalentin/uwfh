//#include "msp430.h"
#include <msp430x22x2.h>
#include "adc_ads.h"
#include "spi.h"
#include "batt.h"
#include "can.h"
#include "can_data.h"

#include "main.h"

#define DEBOUNCE_THRESH 5
#define PRECHARGE_DELAY 3000
#define RELAY_STAGGER   500

#define BATT_TEMP_DELAY 20

#define STATE_OFF 	0
#define STATE_PRE	1
#define STATE_ENERGIZED 2

#define BATT_TEMP	0
#define BATT_CV		1

#define FAULT_THRESHOLD 10
#define GFI_FAULT_DELAY 50

//
// Function Declarations
//
void timer_init(void);

//
// Initialize port pins
//
void initPortPins(void)
{
  	IO_POS_OFF;
	IO_NEG_OFF;
	IO_PRE_OFF;
	IO_STROBE_ON;
	IO_CAR_OK;
	IO_STK1_CS_DIS;
	IO_STK2_CS_DIS;
	IO_CAN_CS_DIS;
	IO_ADC_CS_DIS;
	
  P1DIR = 0xFF;								// no inputs needed
  P2DIR = ~(PIN2+PIN1);                		// Set P2.2,1 as input
  P3DIR = ~(PIN5+PIN2);							// Set P3.5,2 as an input
  P4DIR = (char)~(0x82); //~(PIN7+PIN1);						// Set P4.7,1 as input
  P3SEL = PIN1 + PIN2 + PIN3;
  P3OUT = 0x00;
}

void clock_init (void)
{
	DCOCTL = CALDCO_12MHZ;
	BCSCTL1 = CALBC1_12MHZ;
	//BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
}

void main(void)
{
	
	unsigned char energize_debounce = 0;
	unsigned char energize_state = STATE_OFF;
	unsigned int precharge_delay = 0;
	unsigned int batt_temp_delay = 0;
	unsigned int isense1, isense2;
	unsigned char gfi_ok;
	
	unsigned int temp_stk1[3*3];
	unsigned int cv_stk1[3*10];
	unsigned int temp_stk2[3*3];
	unsigned int cv_stk2[3*10];
	
	unsigned char batt_state = BATT_TEMP;
	
	unsigned char batt_cfg[6], batt_cfg_out[6*3];
	
	unsigned char batt_diag[2*3];
	unsigned int i;
	
	unsigned int fault = 0;
	unsigned int fault_count = 0;
	unsigned int tripped = 0;
	unsigned int gfi_fault = 0;
	
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	
	//Delay to allow 3.3V rail to fully rise, essential for CAN part
	//because MSP430 will turn on at 1.8V or less
	//Especially is 3.3V voltage supervisor is not installed!
	for(i=0; i<65000; i++)
		asm("nop");
		
	clock_init();
	__enable_interrupt();                     // Enable interrupts
	
  initPortPins();                           // Initialize port pins
  timer_init();
//  ADC_init();
//  DAC_init();
//  uart_init();
  spi_init();
  can_init(CAN_BITRATE_250);
	
	//batt_cfg[0] = BATT_CDC_OFF_13ms | BATT_CELL10;
	//batt_cfg[1] = 0x0;
	//batt_cfg[2] = 0xF0;
	//batt_cfg[3] = 0xFF;
	//batt_cfg[4] = BATT_VUV;
	//batt_cfg[5] = BATT_VOV;
	
	batt_cfg[0] = 0xE9;
	batt_cfg[1] = 0x0;
	batt_cfg[2] = 0x0;
	batt_cfg[3] = 0x0;
	batt_cfg[4] = BATT_VUV;
	batt_cfg[5] = BATT_VOV;
	
	BATT_config ( batt_cfg, 3, STACK_1 );
	BATT_config ( batt_cfg, 3, STACK_2 );
	//BATT_start_conv_temp ( STACK_2 );
	
  for(;;)
  {
    _BIS_SR(LPM0_bits + GIE);               // LPM0, enable interrupts
    
    /*
     * Out of sleep mode.
     */
     isense1 = ADC_read12( ADS_PW | ADS_SINGLE | ADS_CH2 );
     isense2 = ADC_read12( ADS_PW | ADS_SINGLE | ADS_CH0 );
     
     gfi_ok = IO_GFD_OK;
     
     batt_temp_delay++;
     if ( batt_temp_delay > BATT_TEMP_DELAY )
     {
     	batt_temp_delay = 0;
     	
     	//Send a test can message
     //spi_set_mode ( UCCKPH, 0, 5 );
     //can_write_vcell(cv_stk2, BATT_S1);
     	
     	BATT_read_diag ( batt_diag, 3, STACK_1 );
     	BATT_read_cfg ( batt_cfg_out, 3, STACK_1 );
     	
     	if ( batt_cfg_out[0] & BATT_WDT 
     	  || batt_cfg_out[6] & BATT_WDT
     	  || batt_cfg_out[12] & BATT_WDT )
     	{
     		BATT_config ( batt_cfg, 3, STACK_1 );
     	}
     	
     	BATT_read_diag ( batt_diag, 3, STACK_2 );
     	BATT_read_cfg ( batt_cfg_out, 3, STACK_2 );
     	
     	if ( batt_cfg_out[0] & BATT_WDT 
     	  || batt_cfg_out[6] & BATT_WDT
     	  || batt_cfg_out[12] & BATT_WDT )
     	{
     		BATT_config ( batt_cfg, 3, STACK_2 );
     	}
     	  	
     	
     	if ( batt_state == BATT_TEMP )
     	{
     		BATT_read_temp ( temp_stk1, 3, STACK_1 );
	    	BATT_read_temp ( temp_stk2, 3, STACK_2 );
	    	
	    	BATT_start_conv_cv ( STACK_1 );
	    	BATT_start_conv_cv ( STACK_2 );
	    	
	    	batt_state = BATT_CV;
     	}
     	else if ( batt_state == BATT_CV )
     	{
     		BATT_read_cv ( cv_stk1, 3, STACK_1 );
			BATT_read_cv ( cv_stk2, 3, STACK_2 );
			
			BATT_start_conv_temp ( STACK_1 );
			BATT_start_conv_temp ( STACK_2 );
			
			batt_state = BATT_TEMP;
			
	     	spi_set_mode ( UCCKPH, 0, 5 );
	     	can_write_vstats(BATT_S1, cv_stk1, temp_stk1, isense1);
	     	can_write_vstats(BATT_S2, cv_stk2, temp_stk2, isense2);

	     	//can_write_vcell(BATT_S1, cv_stk1);
	     	//can_write_vcell(BATT_S2, cv_stk2);

	     	can_write_temps(BATT_S1, temp_stk1);
	     	can_write_temps(BATT_S2, temp_stk2);
			
			for ( i = 0; i < 30; i++ )
		     {
		     		if ( cv_stk1[i] > 4100 || cv_stk1[i] < 3300 )
		     		{
						fault = 1;
		     		}
		     		
		     		if ( cv_stk2[i] > 4100 || cv_stk2[i] < 3300 )
		     		{
						fault = 1;
		     		}
		     }
		     
		     for ( i = 0; i < 3; i++ )
		     {
		     		if ( temp_stk1[i*3 + 1] > 50 )
		     		{
						fault = 1;
		     		}
		     		
		     		if ( temp_stk2[i*3 + 1] > 50 )
		     		{
						fault = 1;
		     		}
		     }
		     
		     if ( isense1 > 2475 || isense1 < 1948 )
		     {
		     	fault = 1;
		     }
		     
		     if ( isense2 < 1416 || isense2 > 2150 )
		     {
		     	fault = 1;
		     }
		     
		     if ( !(gfi_ok) )
		     {
		     	gfi_fault++;
		     }
		     else
		     {
		     	gfi_fault = 0;
		     }
		     
		     if ( gfi_fault > GFI_FAULT_DELAY )
		     {
		     	fault = 1;
		     }
		     			     
		     
		     if ( fault )
		     {
		     	fault_count++;
		     }
		     else
		     {
		     	fault_count = 0;
		     }
		     
		     fault = 0;
     	}

     }
     
     if ( IO_ENERGIZE && energize_state == STATE_OFF ) energize_debounce++;
     else energize_debounce = 0;
     
     if ( energize_state == STATE_OFF && IO_ENERGIZE && energize_debounce > DEBOUNCE_THRESH )
     {
     	IO_STROBE_ON;
 		energize_state = STATE_PRE;
 		IO_NEG_ON;
 		precharge_delay = 0;
 	}
    
    if ( !IO_ENERGIZE )
    {
 		energize_state = STATE_OFF;
 		IO_POS_OFF;
 		IO_NEG_OFF;
 		IO_PRE_OFF;
 		IO_STROBE_OFF;
     }
     
     if ( energize_state == STATE_PRE )
     {
     	precharge_delay++;
     	if ( precharge_delay > RELAY_STAGGER )
     	{
     		IO_STROBE_ON;
     		IO_PRE_ON;
     	}
     	
     	if ( precharge_delay > PRECHARGE_DELAY )
     	{
     		IO_STROBE_ON;
     		IO_POS_ON;
     		IO_PRE_OFF;
     		energize_state = STATE_ENERGIZED;
     	}
     }
     
     if ( fault_count >= FAULT_THRESHOLD || tripped )
     {
     	tripped = 1;
     	IO_FAULT;
		IO_POS_OFF;
 		IO_NEG_OFF;
 		IO_PRE_OFF;
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
  P4OUT ^=  PIN4;
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
