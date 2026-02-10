#define PHASE_WELLSETUPBIOASSAYENTEREFF_PRE_INIT	0
#define PHASE_WELLSETUPBIOASSAYENTEREFF_WAIT		1
#define PHASE_WELLSETUPBIOASSAYENTEREFF_SAVE		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "amulet.h"
#include "bioassay.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

PROBEBIOASSAYEFFICIENCY AmuletWellSetupBioAssayEnterEfficiencyMenu_eff;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void SetAmuletBackHTML(void);

void AmuletWellSetupBioAssayEnterEfficiency_menu(void){
	char message[26];

	switch(m_iPhase){
		case PHASE_WELLSETUPBIOASSAYENTEREFF_PRE_INIT:
			if(m_ucClear == 81){
				BioAssayCopyCurrentEfficiency(&AmuletWellSetupBioAssayEnterEfficiencyMenu_eff);
				m_ucClear = 0;
			}

			if(AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I131Efficiency > 0.0) sprintf(message, "%.3f", AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I131Efficiency);
			else message[0] = 0;
			SetAmuletString(101, message);

			if(AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I125Efficiency > 0.0) sprintf(message, "%.3f", AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I125Efficiency);
			else message[0] = 0;
			SetAmuletString(102, message);

			if(AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I123Efficiency > 0.0) sprintf(message, "%.3f", AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I123Efficiency);
			else message[0] = 0;
			SetAmuletString(103, message);

			if(AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I131I125Contamination > 0.0) sprintf(message, "%.5f", AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I131I125Contamination);
			else message[0] = 0;
			SetAmuletString(104, message);

			if(AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I123I125Contamination > 0.0) sprintf(message, "%.5f", AmuletWellSetupBioAssayEnterEfficiencyMenu_eff.I123I125Contamination);
			else message[0] = 0;
			SetAmuletString(105, message);

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLSETUPBIOASSAYENTEREFF_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAYENTEREFF_WAIT:
			break;

		case PHASE_WELLSETUPBIOASSAYENTEREFF_SAVE:
			BioAssaySaveEfficiency(&AmuletWellSetupBioAssayEnterEfficiencyMenu_eff, TRUE);
			SetAmuletBackHTML();
			break;

	}
}
