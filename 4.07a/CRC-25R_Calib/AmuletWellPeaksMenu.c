#define PHASE_WELLPEAKS_PRE_INIT	0
#define PHASE_WELLPEAKS_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);

extern int m_iPhase;
extern SPEC_MEAS spec_meas;

void AmuletWellPeaks_menu(void){
	char acMsg[100];
	PEAK peak[40];
	short index, jndex, dummynp1;
	long counts;

	switch(m_iPhase){
		float keV;

		case PHASE_WELLPEAKS_PRE_INIT:
			jndex = 0;
			Mca_getPeaks(&(peak[0]), &dummynp1, FALSE);
			Mca_dedupPeaks(peak, dummynp1, spec_meas.num_of_channels);
			for(index=0; index<40; index++){
				if((peak[index].height>0) && (peak[index].centroid>0)){
					if(jndex<20){
						keV = Mca_convertChToEnergy(peak[index].centroid, NULL);
						counts = peak[index].height;
						sprintf(acMsg, "%d) %f keV (%ld)", jndex+1, keV, counts);
						SetAmuletString(100+jndex, acMsg);
						SetAmuletByte(100+jndex, 0xFF);
						jndex++;
					}
				}
			}
			m_iPhase = PHASE_WELLPEAKS_WAIT;
			break;

		case PHASE_WELLPEAKS_WAIT:
			break;
	}
}
