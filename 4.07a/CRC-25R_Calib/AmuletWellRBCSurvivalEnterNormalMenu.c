#define PHASE_WELLRBCSURVIVALENTERNORMAL_PRE_INIT	0
#define PHASE_WELLRBCSURVIVALENTERNORMAL_WAIT		1
#define PHASE_WELLRBCSURVIVALENTERNORMAL_SAVE		2
#define PHASE_WELLRBCSURVIVALENTERNORMAL_CLEAR_ROW	3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "amulet.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern float RBCSurvival_minNormal, RBCSurvival_maxNormal;
bool AmuletWellRBCSurvivalEnterNormal_enter;
float AmuletWellRBCSurvivalEnterNormal_min, AmuletWellRBCSurvivalEnterNormal_max;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void RBCSurvivalSaveNormal(void);

void AmuletWellRBCSurvivalEnterNormal_menu(void){
	float tempfloat;
	char message[26];

	switch(m_iPhase){
		case PHASE_WELLRBCSURVIVALENTERNORMAL_PRE_INIT:
			if(m_ucClear == 104){
				AmuletWellRBCSurvivalEnterNormal_min = RBCSurvival_minNormal;
				AmuletWellRBCSurvivalEnterNormal_max = RBCSurvival_maxNormal;
				m_ucClear = 0;
			}

			if((AmuletWellRBCSurvivalEnterNormal_min != -1) && (AmuletWellRBCSurvivalEnterNormal_max != -1)){
				if(AmuletWellRBCSurvivalEnterNormal_min > AmuletWellRBCSurvivalEnterNormal_max){
					tempfloat = AmuletWellRBCSurvivalEnterNormal_min;
					AmuletWellRBCSurvivalEnterNormal_min = AmuletWellRBCSurvivalEnterNormal_max;
					AmuletWellRBCSurvivalEnterNormal_max = tempfloat;
				}
			}

			if(AmuletWellRBCSurvivalEnterNormal_min == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%.1f", AmuletWellRBCSurvivalEnterNormal_min);
			}
			SetAmuletString(100, message);

			if(AmuletWellRBCSurvivalEnterNormal_max == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%.1f", AmuletWellRBCSurvivalEnterNormal_max);
			}
			SetAmuletString(101, message);

			if(AmuletWellRBCSurvivalEnterNormal_enter) SetAmuletByte(100, 0xFF);
			else SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_WELLRBCSURVIVALENTERNORMAL_WAIT;
			break;

		case PHASE_WELLRBCSURVIVALENTERNORMAL_WAIT:
			break;

		case PHASE_WELLRBCSURVIVALENTERNORMAL_SAVE:
			if((AmuletWellRBCSurvivalEnterNormal_min != -1) || (AmuletWellRBCSurvivalEnterNormal_max != -1)){
				if((AmuletWellRBCSurvivalEnterNormal_min == -1) || (AmuletWellRBCSurvivalEnterNormal_max == -1)){
					Amulet_DisplayError("RBC Survival", "Range is incomplete", TRUE);
					beep_amulet();
					return;
				}
			}

			RBCSurvival_minNormal = AmuletWellRBCSurvivalEnterNormal_min;
			RBCSurvival_maxNormal = AmuletWellRBCSurvivalEnterNormal_max;

			// Save to Database
			RBCSurvivalSaveNormal();
			SetAmuletBackHTML();
			break;

		case PHASE_WELLRBCSURVIVALENTERNORMAL_CLEAR_ROW:
			AmuletWellRBCSurvivalEnterNormal_min = -1;
			AmuletWellRBCSurvivalEnterNormal_max = -1;
			SetAmuletString(100, "");
			SetAmuletString(101, "");
			SetAmuletByte(101, 0xFF);
			beep_amulet();
			m_iPhase = PHASE_WELLRBCSURVIVALENTERNORMAL_WAIT;
			break;
	}
}
