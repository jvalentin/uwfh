#include "msp430.h"
#include "adc_ads.h"
#include "spi.h"
#include "batt.h"

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

//
// Function Declarations
//
void timer_init(void);
void initPortPins(void);

void clock_init (void)
{
	DCOCTL = CALDCO_12MHZ;
	BCSCTL1 = CALBC1_12MHZ;
	//BCSCTL2 = SELM_0 + DIVM_0 + DIVS_0;
}

void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
	clock_init();
	__enable_interrupt();                     // Enable interrupts
	
  initPortPins();                           // Initialize port pins
  timer_init();
//  ADC_init();
//  DAC_init();
//  UART_init();
  spi_init();

	unsigned char energize_debounce = 0;
	unsigned char energize_state = STATE_OFF;
	unsigned int precharge_delay = 0;
	unsigned int batt_temp_delay = 0;
	unsigned int val = 0;
	unsigned int temp_stk2[3*3];
	unsigned int cv_stk2[3*10];
	
	unsigned char batt_state = BATT_TEMP;
	
	unsigned char batt_cfg[6], batt_cfg_out[6*3];
	
	unsigned char batt_diag[2*3];
	
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
	
	BATT_config ( batt_cfg, 3, STACK_2 );
	//BATT_start_conv_temp ( STACK_2 );
	
  for(;;)
  {
    _BIS_SR(LPM0_bits + GIE);               // LPM0, enable interrupts
    
    /*
     * Out of sleep mode.
     */
     
     
     val = ADC_read12( ADS_PW | ADS_SINGLE | ADS_CH2 );
     
     batt_temp_delay++;
     if ( batt_temp_delay > BATT_TEMP_DELAY )
     {
     	batt_temp_delay = 0;
     	
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
	    	BATT_read_temp ( temp_stk2, 3, STACK_2 );
	    	
	    	BATT_start_conv_cv ( STACK_2 );
	    	
	    	batt_state = BATT_CV;
     	}
     	else if ( batt_state = BATT_CV )
     	{
			BATT_read_cv ( cv_stk2, 3, STACK_2 );
			
			BATT_start_conv_temp ( STACK_2 );
			
			batt_state = BATT_TEMP;
     	}
		
     }
     
     if ( IO_ENERGIZE && energize_state == STATE_OFF ) energize_debounce++;
     else energize_debounce = 0;
     
     if ( energize_state == STATE_OFF && IO_ENERGIZE && energize_debounce > DEBOUNCE_THRESH )
     {
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
     }
     
     if ( energize_state == STATE_PRE )
     {
     	precharge_delay++;
     	if ( precharge_delay > RELAY_STAGGER )
     	{
     		IO_PRE_ON;
     	}
     	
     	if ( precharge_delay > PRECHARGE_DELAY )
     	{
     		IO_POS_ON;
     		IO_PRE_OFF;
     		energize_state = STATE_ENERGIZED;
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
