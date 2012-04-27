#include "batt.h"
#include "spi.h"
//#include "msp430.h"
#include <msp430x22x2.h>
#include "main.h"

void gen_pec ( unsigned char data, unsigned char* pec );

#define WRCFG	0x01
#define RDCFG	0x02
#define RDCV	0x04
#define RDTMP	0x0E
#define STCVAD	0x10
#define STTMPAD	0x30
#define RDDGNR	0x54

#define BATT_SPEED 100

// 5*n config bytes
void BATT_config ( unsigned char* cfg, unsigned char n, unsigned char stack )
{
	unsigned char pec = 0x41;
	unsigned int i,j;
	
	spi_set_mode ( UCCKPH, 0, BATT_SPEED );
    
    if ( stack == STACK_1 ) { IO_STK1_CS_EN; }
    else if  ( stack == STACK_2 ) { IO_STK2_CS_EN; }
    
    spi_transmit(WRCFG);
    gen_pec(WRCFG,&pec);
    spi_transmit(pec);
    
    for ( i = 0; i < n; i++ )
    {
    	pec = 0x41;
    	
    	for ( j = 0; j < 6; j++ )
    	{
    		spi_transmit(cfg[j]);
    		gen_pec(cfg[j],&pec);
    	}
    	
    	spi_transmit(pec);
    }
    
    IO_STK1_CS_DIS;    
    IO_STK2_CS_DIS;
}

void BATT_start_conv_cv ( unsigned char stack )
{
	unsigned char pec = 0x41;
	
	spi_set_mode ( UCCKPH, 0, BATT_SPEED );
    
    if ( stack == STACK_1 ) { IO_STK1_CS_EN; }
    else if  ( stack == STACK_2 ) { IO_STK2_CS_EN; }
    
    spi_transmit(STCVAD);
    gen_pec(STCVAD,&pec);
    spi_transmit(pec);
    
    IO_STK1_CS_DIS;    
    IO_STK2_CS_DIS;
}

// 10*n cell voltages
void BATT_read_cv ( unsigned int* cv, unsigned char n, unsigned char stack )
{
	unsigned char pec = 0x41;
	unsigned int i,j;
	unsigned char buf0, buf1, buf2;
	unsigned int cv0, cv1;
	
	spi_set_mode ( UCCKPH, 0, BATT_SPEED );
    
    if ( stack == STACK_1 ) { IO_STK1_CS_EN; }
    else if  ( stack == STACK_2 ) { IO_STK2_CS_EN; }
    
    spi_transmit(RDCV);
    gen_pec(RDCV,&pec);
    spi_transmit(pec);
    
    for ( i = 0; i < n; i++ )
    {
    	for ( j = 0; j < 5; j++ )
    	{
    		buf0 = spi_exchange( 0x00 );
    		buf1 = spi_exchange( 0x00 );
    		buf2 = spi_exchange( 0x00 );
    		
    		cv0 = ((unsigned int)buf0) & 0x00FF;
    		cv0 |= (((unsigned int)buf1) << 8) & 0x0F00;
    		
    		cv1 = (((unsigned int)buf1) >> 4) & 0x000F;
    		cv1 |= (((unsigned int)buf2) << 4) & 0x0FF0;
    		
    		cv0 -= 512;
    		cv0 += ( cv0 >> 1 );
    		
    		cv1 -= 512;
    		cv1 += ( cv1 >> 1 );
    		
    		cv[i*10 + j*2] = cv0;
    		cv[i*10 + j*2 + 1] = cv1;
    	}
    	pec = spi_exchange( 0x00 );
    }
    
    IO_STK1_CS_DIS;    
    IO_STK2_CS_DIS;
}

void BATT_start_conv_temp ( unsigned char stack )
{
	unsigned char pec = 0x41;
	
	spi_set_mode ( UCCKPH, 0, BATT_SPEED );
    
	if ( stack == STACK_1 )
	{
		IO_STK1_CS_EN;
	}
    else if  ( stack == STACK_2 )
    {
    	IO_STK2_CS_EN;
    }
    
    spi_transmit(STTMPAD);
    gen_pec(STTMPAD,&pec);
    spi_transmit(pec);
    
    IO_STK1_CS_DIS;    
    IO_STK2_CS_DIS;
}

// (2 external, 1 internal)*n temperatures
void BATT_read_temp ( unsigned int* temp, unsigned char n, unsigned char stack )
{
	unsigned char pec = 0x41;
	unsigned int i,j;
	unsigned char buf[5];
	unsigned int temp0, temp1, temp2;
	
	spi_set_mode ( UCCKPH, 0, BATT_SPEED );
    
    if ( stack == STACK_1 ) { IO_STK1_CS_EN; }
    else if  ( stack == STACK_2 ) { IO_STK2_CS_EN; }
    
    spi_transmit(RDTMP);
    gen_pec(RDTMP,&pec);
    spi_transmit(pec);
    
    for ( i = 0; i < n; i++ )
    {
    	for ( j = 0; j < 5; j++ )
    	{
    		buf[j] = spi_exchange( 0xAA );
    	}
    	pec = spi_exchange( 0xAA );
    	
    	temp0 = (unsigned int)buf[0] & 0x00FF;
    	temp0 |= (((unsigned int)buf[1])<<8 ) & 0x0F00;
    	
    	temp1 =  (((unsigned int)buf[1])>>4) & 0x000F;
    	temp1 |= (((unsigned int)buf[2])<<4) & 0x0FF0;
    	
    	temp2 =  ((unsigned int)buf[3]) & 0x00FF;
    	temp2 |= (((unsigned int)buf[4])<<8 ) & 0x0F00;
    	
    	temp0 = (unsigned int) (( -((int)temp0-58) / 17 ) + 114 );		// no-contact battery end
    	temp1 = (unsigned int) (( -((int)temp1-58) / 17 ) + 114 );		// contact battery end
    	temp2 = (temp2 - 512)*3/16 - 273;	// on-chip temp sensor
    	
    	temp[i*3]   = temp0;
    	temp[i*3+1] = temp1;
    	temp[i*3+2] = temp2;
    }
    
    IO_STK1_CS_DIS;    
    IO_STK2_CS_DIS;
} 

void BATT_read_diag ( unsigned char* buf, unsigned char n, unsigned char stack )
{
	unsigned char pec = 0x41;
	unsigned int i,j;
	
	spi_set_mode ( UCCKPH, 0, BATT_SPEED );
    
    if ( stack == STACK_1 ) { IO_STK1_CS_EN; }
    else if  ( stack == STACK_2 ) { IO_STK2_CS_EN; }
    
    spi_transmit(RDDGNR);
    gen_pec(RDDGNR,&pec);
    spi_transmit(pec);
    
    for ( i = 0; i < n; i++ )
    {
    	for ( j = 0; j < 2; j++ )
    	{
    		buf[i*2 + j] = spi_exchange( 0x00 );
    	}
    	pec = spi_exchange( 0x00 );
    }
    
    IO_STK1_CS_DIS;    
    IO_STK2_CS_DIS;
}


void BATT_read_cfg ( unsigned char* buf, unsigned char n, unsigned char stack )
{
	unsigned char pec = 0x41;
	unsigned int i,j;
	
	spi_set_mode ( UCCKPH, 0, BATT_SPEED );
    
    if ( stack == STACK_1 ) { IO_STK1_CS_EN; }
    else if  ( stack == STACK_2 ) { IO_STK2_CS_EN; }
    
    spi_transmit(RDCFG);
    gen_pec(RDCFG,&pec);
    spi_transmit(pec);
    
    for ( i = 0; i < n; i++ )
    {
    	for ( j = 0; j < 6; j++ )
    	{
    		buf[i*6 + j] = spi_exchange( 0x00 );
    	}
    	pec = spi_exchange( 0x00 );
    }
    
    IO_STK1_CS_DIS;    
    IO_STK2_CS_DIS;
}

void gen_pec ( unsigned char data, unsigned char* pec )
{
	unsigned char in0, pec1, pec2;
	int i;

	for ( i = 0; i < 8; i++, data = data << 1 )
	{
		unsigned char datain = (data>>7)&1;
		unsigned char pec_7 = ((*pec)>>7)&1;
		unsigned char pec_1 = ((*pec)>>1)&1;
		unsigned char pec_0 = ((*pec))&1;

		in0 = datain ^ pec_7;
		pec1 = pec_0 ^ in0;
		pec2 = pec_1 ^ in0;
		*pec = (((*pec)<<1)&0xF8) | pec2<<2 | pec1<<1 | in0;
	}
}
