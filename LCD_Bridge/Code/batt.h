#ifndef BATT_H_
#define BATT_H_

#include "bits.h"

#define STACK_1		1
#define STACK_2		2

#define BATT_CDC_OFF		0
#define BATT_CDC_OFF_13ms	1

#define BATT_CELL10		BIT3

#define BATT_WDT		BIT7

#define BATT_MC1		BIT4
#define BATT_MC2		BIT5
#define BATT_MC3		BIT6
#define BATT_MC4		BIT7

#define BATT_MC5		BIT0
#define BATT_MC6		BIT1
#define BATT_MC7		BIT2
#define BATT_MC8		BIT3
#define BATT_MC9		BIT4
#define BATT_MC10		BIT5
#define BATT_MC11		BIT6
#define BATT_MC12		BIT7

// VUV = (UV)/(16*1.5 mV)+31
// VOV = (OV)/(16*1.5 mV)+32

// 3.3V
#define BATT_VUV		168

// 4.1V
#define BATT_VOV			207

void BATT_init ( void );

void BATT_config ( unsigned char* cfg, unsigned char n, unsigned char stack );  // 6*n config bytes
void BATT_start_conv_cv ( unsigned char stack );
void BATT_read_cv ( unsigned int* cv, unsigned char n, unsigned char stack );   // 10*n cell voltages
void BATT_start_conv_temp ( unsigned char stack );
void BATT_read_temp ( unsigned int* temp, unsigned char n, unsigned char stack ); // (2 external, 1 internal)*n temperatures

void BATT_read_cfg ( unsigned char* buf, unsigned char n, unsigned char stack ); // 6*n config bytes
void BATT_read_diag ( unsigned char* buf, unsigned char n, unsigned char stack ); // 2*n diagnostic bytes

#endif /*BATT_H_*/
