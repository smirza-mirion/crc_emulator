#define PHASE_WELLSETUPBIOASSAY_PRE_INIT	0
#define PHASE_WELLSETUPBIOASSAY_WAIT		1
#define PHASE_WELLSETUPBIOASSAY_I131ACTIVE	2
#define PHASE_WELLSETUPBIOASSAY_I125ACTIVE	3
#define PHASE_WELLSETUPBIOASSAY_I123ACTIVE	4
#define PHASE_WELLSETUPBIOASSAY_SAVE		5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "amulet.h"
#include "bioassay.h"
#include "mca.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucCheckboxState;
extern CURRENT current;

PROBEBIOASSAYSETTING AmuletWellSetupBioAssayMenu_bioAssaySetting;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletWellSetupBioAssay_menu(void){
	char message[100], activity_string[30];

	switch(m_iPhase){
		case PHASE_WELLSETUPBIOASSAY_PRE_INIT:
			if(m_ucClear == 80){
				AmuletWellSetupBioAssayMenu_bioAssaySetting.CountTime = BioAssayCountTime();
				AmuletWellSetupBioAssayMenu_bioAssaySetting.ProbeDistance = BioAssayProbeDistance();
				AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Active = BioAssayI131Active();
				AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Active = BioAssayI125Active();
				AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Active = BioAssayI123Active();
				AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold = BioAssayI131Threshold();
				AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold = BioAssayI125Threshold();
				AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold = BioAssayI123Threshold();
				m_ucClear = 0;
			}

			sprintf(message, "%d", AmuletWellSetupBioAssayMenu_bioAssaySetting.CountTime);
			SetAmuletString(101, message);

			sprintf(message, "%d", AmuletWellSetupBioAssayMenu_bioAssaySetting.ProbeDistance);
			SetAmuletString(102, message);

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Active) SetAmuletByte(71, 2);
			else SetAmuletByte(71, 1);

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Active) SetAmuletByte(72, 4);
			else SetAmuletByte(72, 3);

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Active) SetAmuletByte(73, 6);
			else SetAmuletByte(73, 5);

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold < 0.0) message[0] = 0;
			else{
				if(current.system == CI) format_activity_system2(AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold, message);
				else format_activity_system_kbq(AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold, message);
			}
			SetAmuletString(103, message);

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold < 0.0) message[0] = 0;
			else{
				if(current.system == CI) format_activity_system2(AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold, message);
				else format_activity_system_kbq(AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold, message);
			}
			SetAmuletString(104, message);

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold < 0.0) message[0] = 0;
			else{
				if(current.system == CI) format_activity_system2(AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold, message);
				else format_activity_system_kbq(AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold, message);
			}
			SetAmuletString(105, message);

			if(BioAssayI131Efficiency() <= 0.0) message[0] = 0;
			else sprintf(message, "%.3f%%", BioAssayI131Efficiency());
			SetAmuletString(106, message);

			if(BioAssayI125Efficiency() <= 0.0) message[0] = 0;
			else sprintf(message, "%.3f%%", BioAssayI125Efficiency());
			SetAmuletString(107, message);

			if(BioAssayI123Efficiency() <= 0.0) message[0] = 0;
			else sprintf(message, "%.3f%%", BioAssayI123Efficiency());
			SetAmuletString(108, message);

			if(BioAssayI131I125Contamination() <= 0.0) message[0] = 0;
			else sprintf(message, "%.5f", BioAssayI131I125Contamination());
			SetAmuletString(109, message);

			if(BioAssayI123I125Contamination() <= 0.0) message[0] = 0;
			else sprintf(message, "%.5f", BioAssayI123I125Contamination());
			SetAmuletString(110, message);

			SetAmuletByte(100, 0xFF);

			if(Mca_hasProbe()) SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_WELLSETUPBIOASSAY_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAY_WAIT:
			break;

		case PHASE_WELLSETUPBIOASSAY_I131ACTIVE:
			if(m_ucCheckboxState == 1) AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Active = FALSE;
			else if(m_ucCheckboxState == 2) AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Active = TRUE;
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_WELLSETUPBIOASSAY_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAY_I125ACTIVE:
			if(m_ucCheckboxState == 3) AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Active = FALSE;
			else if(m_ucCheckboxState == 4) AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Active = TRUE;
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_WELLSETUPBIOASSAY_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAY_I123ACTIVE:
			if(m_ucCheckboxState == 5) AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Active = FALSE;
			else if(m_ucCheckboxState == 6) AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Active = TRUE;
			m_ucCheckboxState = 0;
			m_iPhase = PHASE_WELLSETUPBIOASSAY_WAIT;
			break;

		case PHASE_WELLSETUPBIOASSAY_SAVE:
			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold <= .00000001){
				if(current.system == CI) format_activity_system2(.00000001, activity_string);
				else format_activity_system_kbq(.00000001, activity_string);
				sprintf(message, "I131 Trigger must be greater than %s", activity_string);
				Amulet_DisplayError("BioAssay Setup", message, TRUE);
				return;
			}

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I131Threshold >= .001){
				if(current.system == CI) format_activity_system2(.001, activity_string);
				else format_activity_system_kbq(.001, activity_string);
				sprintf(message, "I131 Trigger must be less than %s", activity_string);
				Amulet_DisplayError("BioAssay Setup", message, TRUE);
				return;
			}

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold <= .00000001){
				if(current.system == CI) format_activity_system2(.00000001, activity_string);
				else format_activity_system_kbq(.00000001, activity_string);
				sprintf(message, "I125 Trigger must be greater than %s", activity_string);
				Amulet_DisplayError("BioAssay Setup", message, TRUE);
				return;
			}

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I125Threshold >= .001){
				if(current.system == CI) format_activity_system2(.001, activity_string);
				else format_activity_system_kbq(.001, activity_string);
				sprintf(message, "I125 Trigger must be less than %s", activity_string);
				Amulet_DisplayError("BioAssay Setup", message, TRUE);
				return;
			}

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold <= .00000001){
				if(current.system == CI) format_activity_system2(.00000001, activity_string);
				else format_activity_system_kbq(.00000001, activity_string);
				sprintf(message, "I123 Trigger must be greater than %s", activity_string);
				Amulet_DisplayError("BioAssay Setup", message, TRUE);
				return;
			}

			if(AmuletWellSetupBioAssayMenu_bioAssaySetting.I123Threshold >= .001){
				if(current.system == CI) format_activity_system2(.001, activity_string);
				else format_activity_system_kbq(.001, activity_string);
				sprintf(message, "I123 Trigger must be less than %s", activity_string);
				Amulet_DisplayError("BioAssay Setup", message, TRUE);
				return;
			}
			BioAssaySaveSetting(&AmuletWellSetupBioAssayMenu_bioAssaySetting);
			SetAmuletBackHTML();
			break;
	}
}
