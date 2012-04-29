//#include "msp430.h"
#include <msp430x22x2.h>
#include "can_data.h"
#include "spi.h"
#include "itoa.h"
#include "can.h"

//Write set of 9 temperature readings from a single string to CAN bus
//Assume order of message is batt_temp,batt_temp,ambient_temp x3
void can_write_temps(unsigned int* temps, unsigned int StringNo)
{
	int tmax = 0x0000;   //battery temp
	int tamax = 0x0000;  //ambient temp
	int i;
	//First find all max/min temperatures
	for(i=0; i<3; i++)
	{
		if(temps[3*i]  >tmax)  tmax  = temps[3*i];
		if(temps[3*i+1]>tmax)  tmax  = temps[3*i+1];
		if(temps[3*i+2]>tamax) tamax = temps[3*i+2];
	}
	//Prepare a CAN message for these temperatures
	if(StringNo == BATT_S1) 	{can_push_ptr->address = BMS_S1_CAN_BASE;}
	else 					 	{can_push_ptr->address = BMS_S2_CAN_BASE;}
	can_push_ptr->address += BMS_TEMP;
	can_push_ptr->data.data_u16[0] = tmax;
	can_push_ptr->data.data_u16[1] = tamax;
	can_push();
    can_transmit();
}

void can_write_vcell(unsigned int* vcell, unsigned int StringNo)
{
	//Prepare a CAN message for these cell voltages
	if(StringNo == BATT_S1) 	{can_push_ptr->address = BMS_S1_CAN_BASE;}
	else 					 	{can_push_ptr->address = BMS_S2_CAN_BASE;}
	can_push_ptr->address += BMS_VMINMAX;
	can_push_ptr->data.data_u16[0] = 123;
	can_push_ptr->data.data_u16[1] = 123;
	can_push();
    can_transmit();
}
