#ifndef ADC_ADS_H
#define ADC_ADS_H

/*ADS8341 defines*/
#define ADS_PW                  0x80
#define ADS_NPD                 0x03
#define ADS_8B                  0x08
#define ADS_SINGLE              0x04
#define ADS_CH0                 0x10
#define ADS_CH1                 0x50
#define ADS_CH2                 0x20
#define ADS_CH3                 0x60


/*
 * Reads data from the ADS8341 ADC
 *	- Pass it the channel and conversion info as an arguement, get an 8-bit result back
 * Eg: to do a single 8-bit read, single-ended (not differential), from channel 3 of ADC:
 *    readAdc( ADS_PW | ADS_8B | ADS_SINGLE | ADS_CH2 );
 */
unsigned char ADC_read( unsigned char config );

unsigned int ADC_read12( unsigned char config );


#endif /*ADC_ADS_H*/
