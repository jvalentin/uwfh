#ifndef CAN_DATA_H_
#define CAN_DATA_H_

#define BATT_S1 	1
#define BATT_S2 	2

void can_write_temps(unsigned int* temps, unsigned int StringNo);

void can_write_vcell(unsigned int* vcell, unsigned int StringNo);

#endif //CAN_DATA_H_
