#define PHASE_WELLSETUPBIOASSAYENTERROI_PRE_INIT	0
#define PHASE_WELLSETUPBIOASSAYENTERROI_WAIT		1
#define PHASE_WELLSETUPBIOASSAYENTERROI_SAVE		2
#define PHASE_WELLSETUPBIOASSAYENTERROI_DEFAULT		3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "amulet.h"
#include "bioassay.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern float I131Lower_keV, I131Upper_keV, I125Lower_keV, I125Upper_keV, I123Lower_keV, I123Upper_keV;

float AmuletWellSetupBioAssayEnterROI_I131Lower_keV;
float AmuletWellSetupBioAssayEnterROI_I131Upper_keV;
float AmuletWellSetupBioAssayEnterROI_I125Lower_keV;
float AmuletWellSetupBioAssayEnterROI_I125Upper_keV;
float AmuletWellSetupBioAssayEnterROI_I123Lower_keV;
float AmuletWellSetupBioAssayEnterROI_I123Upper_keV;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void SetAmuletBackHTML(void);

void AmuletWellSetupBioAssayEnterROI_menu(void){
	float dummy;
	char message[26];

	switch(m_iPhase){
		case PHASE_WELLSETUPBIOASSAYENTERROI_PRE_INIT:
			if(m_ucClear == 86){
				AmuletWellSetupBioAssayEnterROI_I131Lower_keV = I131Lower_keV;
				AmuletWellSetupBioAssayEnterROI_I131Upper_keV = I131Upper_keV;
				AmuletWellSetupBioAssayEnterROI_I125Lower_keV = I125Lower_keV;
				AmuletWellSetupBioAssayEnterROI_I125Upper_keV = I125Upper_keV;
				AmuletWellSetupBioAssayEnterROI_I123Lower_keV = I123Lower_keV;
				AmuletWellSetupBioAssayEnterROI_I123Upper_keV = I123Upper_keV;

				m_ucClear = 0;
			}

			if(AmuletWellSetupBioAssayEnterROI_I131Lower_keV > AmuletWellSetupBioAssayEnterROI_I131Upper_keV){
				dummy = AmuletWellSetupBioAssayEnterROI_I131Lower_keV;
				AmuletWellSetupBioAssayEnterROI_I131Lower_keV = AmuletWellSetupBioAssayEnterROI_I131Upper_keV;
				AmuletWellSetupBioAssayEnterROI_I131Upper_keV = dummy;
			}

			if(AmuletWellSetupBioAssayEnterROI_I125Lower_keV > AmuletWellSetupBioAssayEnterROI_I125Upper_keV){
				dummy = AmuletWellSetupBioAssayEnterROI_I125Lower_keV;
				AmuletWellSetupBioAssayEnterROI_I125Lower_keV = AmuletWellSetupBioAssayEnterROI_I125Upper_keV;
				AmuletWellSetupBioAssayEnterROI_I125Upper_keV = dummy;
			}

			if(AmuletWellSetupBioAssayEnterROI_I123Lower_keV > AmuletWellSetupBioAssayEnterROI_I123Upper_keV){
				dummy = AmuletWellSetupBioAssayEnterROI_I123Lower_keV;
				AmuletWellSetupBioAssayEnterROI_I123Lower_keV = AmuletWellSetupBioAssayEnterROI_I123Upper_keV;
				AmuletWellSetupBioAssayEnterROI_I123Upper_keV = dummy;
			}

			sprintf(message, "%.1f keV", AmuletWellSetupBioAssayEnterROI_I131Lower_keV);
			SetAmuletString(101, message);

			sprintf(message, "%.1f keV", AmuletWellSetupBioAssayEnterROI_I131Upper_keV);
			SetAmuletString(102, message);

			sprintf(message, "%.1f keV", AmuletWellSetupBioAssayEnterROI_I125Lower_keV);
			SetAmuletString(103, message);

			sprintf(message, "%.1f keV", AmuletWellSetupBioAssayEnterROI_I125Upper_keV);
			SetAmuletString(104, message);

			sprintf(message, "%.1f keV", AmuletWellSetupBioAssayEnterROI_I123Lower_keV);
			SetAmuletString(105, message);

			sprintf(message, "%.1f keV", AmuletWellSetupBioAssayEnterROI_I123Upper_keV);
			SetAmuletString(106, message);

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLSETUPBIOASSAYENTERROI_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAYENTERROI_WAIT:
			break;

		case PHASE_WELLSETUPBIOASSAYENTERROI_SAVE:
			I131Lower_keV = AmuletWellSetupBioAssayEnterROI_I131Lower_keV;
			I131Upper_keV = AmuletWellSetupBioAssayEnterROI_I131Upper_keV;
			I125Lower_keV = AmuletWellSetupBioAssayEnterROI_I125Lower_keV;
			I125Upper_keV = AmuletWellSetupBioAssayEnterROI_I125Upper_keV;
			I123Lower_keV = AmuletWellSetupBioAssayEnterROI_I123Lower_keV;
			I123Upper_keV = AmuletWellSetupBioAssayEnterROI_I123Upper_keV;
			BioAssayUpdateEfficiencyFromROI(I131Lower_keV, I131Upper_keV, I125Lower_keV, I125Upper_keV, I123Lower_keV, I123Upper_keV);
			BioAssayWriteROI();
			SetAmuletBackHTML();
			break;

		case PHASE_WELLSETUPBIOASSAYENTERROI_DEFAULT:
			beep_amulet();
			AmuletWellSetupBioAssayEnterROI_I131Lower_keV = 292.0;
			AmuletWellSetupBioAssayEnterROI_I131Upper_keV = 437.0;
			AmuletWellSetupBioAssayEnterROI_I125Lower_keV = 20.0;
			AmuletWellSetupBioAssayEnterROI_I125Upper_keV = 50.0;
			AmuletWellSetupBioAssayEnterROI_I123Lower_keV = 143.0;
			AmuletWellSetupBioAssayEnterROI_I123Upper_keV = 191.0;
			m_iPhase = PHASE_WELLSETUPBIOASSAYENTERROI_PRE_INIT;
			break;
	}
}
