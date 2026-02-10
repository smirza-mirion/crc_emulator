#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crc.h"
#include "mca.h"
#include "database.h"

PROBETHYROIDUPTAKENORMAL ThyroidUptake_normal[4];

void ThyroidUptakeInitialize(void){
	DB_RetrieveTUNormal(ThyroidUptake_normal);
}

void ThyroidUptakeSaveNormal(void){
	DB_WriteTUNormal(ThyroidUptake_normal);
}


bool ThyroidUptakeCheckNormal(int hours, float uptake, int *min, int *max, bool *abnormal){
	int index;
	bool returnvalue;
	float floatmin, floatmax;

	*abnormal = FALSE;
	returnvalue = FALSE;
	for(index=0; index<4; index++){
		if(hours == ThyroidUptake_normal[index].hour){
			*min = ThyroidUptake_normal[index].min;
			*max = ThyroidUptake_normal[index].max;
			floatmin = *min;
			floatmax = *max;

			if((uptake < floatmin) || (uptake > floatmax)) *abnormal = TRUE;
			else *abnormal = FALSE;
			returnvalue = TRUE;
			break;
		}
	}

	return returnvalue;
}
