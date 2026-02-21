#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crc.h"
#include "mca.h"
#include "database.h"

float RBCSurvival_minNormal, RBCSurvival_maxNormal;

void RBCSurvivalInitialize(void){
	DB_RetrieveRBCSurvivalNormal(&RBCSurvival_minNormal, &RBCSurvival_maxNormal);

	if(((RBCSurvival_minNormal < 0) && (RBCSurvival_maxNormal > 0)) || ((RBCSurvival_minNormal > 0) && (RBCSurvival_maxNormal < 0))){
		RBCSurvival_minNormal = -1;
		RBCSurvival_maxNormal = -1;
		DB_WriteRBCSurvivalNormal(RBCSurvival_minNormal, RBCSurvival_maxNormal);
	}
}

void RBCSurvivalSaveNormal(void){
	DB_WriteRBCSurvivalNormal(RBCSurvival_minNormal, RBCSurvival_maxNormal);
}
