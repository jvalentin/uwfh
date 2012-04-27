// Include files
//#include "msp430.h"
#include <msp430x22x2.h>
#include "spi.h"
#include "main.h"

unsigned char ADC_read( unsigned char config )
{
    unsigned char adc_val;
    
    spi_set_mode ( UCCKPH, 0, 5 );
    
    IO_ADC_CS_EN;
    
    spi_transmit(config);
    adc_val = (unsigned int) (spi_exchange(0)&0x7F)<<1;
    adc_val |= ((((unsigned int) spi_exchange(0)) & 0x80)>>7);
        
    IO_ADC_CS_DIS;
    
    return adc_val;	
}

// Note: to do a 12-bit read you do something like:
unsigned int ADC_read12( unsigned char config )
{
    unsigned int adc_val;
    
    spi_set_mode ( UCCKPH, 0, 5 );
    
    IO_ADC_CS_EN;
    
    spi_transmit(config);
    adc_val = (unsigned int) (spi_exchange(0)&0x7F)<<5;
    adc_val |= ((((unsigned int) spi_exchange(0)) & 0xF8)>>3);
    
    IO_ADC_CS_DIS;
    
    return adc_val;	
}         

