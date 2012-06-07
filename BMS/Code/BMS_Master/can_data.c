//#include "msp430.h"
#include <msp430x22x2.h>
#include "can_data.h"
#include "spi.h"
#include "can.h"

//Write set of 3 temperature readings from a single string to CAN bus
void can_write_temps(unsigned int StringNo, unsigned int* temp)
{
	//Prepare a CAN message for these temperatures
	if(StringNo == BATT_S1) 	{can_push_ptr->address = BMS_S1_CAN_BASE;}
	else 					 	{can_push_ptr->address = BMS_S2_CAN_BASE;}
	can_push_ptr->address += BMS_TEMP;
	can_push_ptr->data.data_u16[0] = temp[1];
	can_push_ptr->data.data_u16[1] = temp[4];
	can_push_ptr->data.data_u16[2] = temp[7];
	can_push_ptr->status = 6;
	can_push();
    can_transmit();
}

void can_write_vcell(unsigned int StringNo, unsigned int* vcell)
{
	unsigned int i;
	//Prepare a CAN message for these cell voltages
	if(StringNo == BATT_S1) 	{can_push_ptr->address = BMS_S1_CAN_BASE;}
	else 					 	{can_push_ptr->address = BMS_S2_CAN_BASE;}
	can_push_ptr->address += BMS_CV;

	for ( i = 0; i < 30; i += 4 )
	{
		can_push_ptr->data.data_u16[0] = vcell[i];
		can_push_ptr->data.data_u16[1] = vcell[i+1];
		if ( i+2 < 30 )
		{
			can_push_ptr->data.data_u16[2] = vcell[i+2];
			can_push_ptr->data.data_u16[3] = vcell[i+3];
		}
		can_push_ptr->status = 8;
		can_push();
	    can_transmit();
		can_push_ptr->address++;
	}
}

void can_write_vstats(unsigned int StringNo, unsigned int* vcell, unsigned int* temperature, unsigned int isense)
{
	unsigned int i, a;

	//Prepare a CAN message for these cell voltages
	if(StringNo == BATT_S1) 	{can_push_ptr->address = BMS_S1_CAN_BASE;}
	else 					 	{can_push_ptr->address = BMS_S2_CAN_BASE;}
	can_push_ptr->address += BMS_STAT;

	// Sum up all cell voltages in stack
	a = 0;
	for ( i = 0; i < 30; i++ )
	{
		a += ( vcell[i] / 10 );
	}
	can_push_ptr->data.data_u16[0] = a;

	// Average temperature

	a = temperature[1] + temperature[4] + temperature[7];
	a /= 3;
	can_push_ptr->data.data_u16[1] = a;

	// Current measurement

	can_push_ptr->data.data_u16[2] = isense;

	can_push_ptr->status = 6;
	can_push();
    can_transmit();
}
