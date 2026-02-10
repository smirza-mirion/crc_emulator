#define PHASE_WELLCHANGEROI_PRE_INIT	0
#define PHASE_WELLCHANGEROI_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "nuc.h"
#include "mca.h"

extern int m_iPhase;
extern char generalNuclideID;
extern float startEV[10];
extern float endEV[10];

void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);

void AmuletWellChangeROI_menu(void){
	char message[51];
	short calcStart, calcEnd;

	switch(m_iPhase){
		case PHASE_WELLCHANGEROI_PRE_INIT:
			if(generalNuclideID == -2){
				if((startEV[0]>=0.0) && (endEV[0]>=0.0)){
					Mca_getROIChannels(startEV[0], endEV[0], &calcStart, &calcEnd, NULL);
					if((calcStart >= 0) && (calcEnd >= 0)){
						sprintf(message, "There are %d channels in the ROI", (calcEnd - calcStart) + 1);
						SetAmuletString(104, message);
						sprintf(message, "Begin Channel#: %d = %f keV", calcStart, Mca_convertChToEnergy(calcStart, NULL));
						SetAmuletString(106, message);
						sprintf(message, "End Channel#: %d = %f keV", calcEnd, Mca_convertChToEnergy(calcEnd, NULL));
						SetAmuletString(108, message);
						SetAmuletByte(101, 0xFF);
					}else{
						SetAmuletString(104, "No channels are between");
						sprintf(message, "%.1f keV - %.1f keV", startEV[0], endEV[0]);
						SetAmuletString(106, message);
						SetAmuletByte(102, 0xFF);
						startEV[0] = -1.0;
						endEV[0] = -1.0;
					}
				}

				if(startEV[0]>=0.0){
					sprintf(message, "%.1f keV", startEV[0]);
					SetAmuletString(102, message);
				}
				if(endEV[0]>=0.0){
					sprintf(message, "%.1f keV", endEV[0]);
					SetAmuletString(103, message);
				}
				SetAmuletByte(100, 0xFF);
			}else if(generalNuclideID >= 0){
				if((startEV[0]>=0.0) && (endEV[0]>=0.0)){
					Mca_getROIChannels(startEV[0], endEV[0], &calcStart, &calcEnd, NULL);
					if((calcStart>=0) && (calcEnd>=0)){
						sprintf(message, "There are %d channels in the ROI", (calcEnd - calcStart) + 1);
						SetAmuletString(104, message);
						sprintf(message, "Begin Channel#: %d = %f keV", calcStart, Mca_convertChToEnergy(calcStart, NULL));
						SetAmuletString(106, message);
						sprintf(message, "End Channel#: %d = %f keV", calcEnd, Mca_convertChToEnergy(calcEnd, NULL));
						SetAmuletString(108, message);
						SetAmuletByte(101, 0xFF);
					}else{
						SetAmuletString(104, "No channels are between");
						sprintf(message, "%.1f keV - %.1f keV", startEV[0], endEV[0]);
						SetAmuletString(106, message);
						SetAmuletByte(102, 0xFF);
						startEV[0] = -1.0;
						endEV[0] = -1.0;
					}
				}

				GetExtendedNuclideString(generalNuclideID, message);
				SetAmuletString(100, message);
				if(startEV[0]>=0.0){
					sprintf(message, "%.1f keV", startEV[0]);
					SetAmuletString(102, message);
				}
				if(endEV[0]>=0.0){
					sprintf(message, "%.1f keV", endEV[0]);
					SetAmuletString(103, message);
				}
				SetAmuletByte(100, 0xFF);
			}
			m_iPhase = PHASE_WELLCHANGEROI_WAIT;
			break;

		case PHASE_WELLCHANGEROI_WAIT:
			break;
	}
}
