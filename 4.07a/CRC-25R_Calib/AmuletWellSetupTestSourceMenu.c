#define PHASE_WELLSETUPTESTNUCLIDE_PRE_INIT	0
#define PHASE_WELLSETUPTESTNUCLIDE_WAIT		1


#include <stdlib.h>
#include <string.h>
#include "crc.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void AmuletWellMainScreenMenu_getWellStandardSN(char *SN);
void AmuletWellMainScreenMenu_updateWellStandardSN(char *SN);
time_t AmuletWellMainScreenMenu_getWellStandardDate(void);
void AmuletWellMainScreenMenu_updateWellStandardDate(time_t calDate);
float AmuletWellMainScreenMenu_getWellStandardActivity(void);
void AmuletWellMainScreenMenu_updateWellStandardActivity(float act);
void AmuletWellMainScreenMenu_WellStandardMirrorUpdate(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);

char AmuletWellSetupTestSourceMenu_SN[11];
time_t AmuletWellSetupTestSourceMenu_calDate;
float AmuletWellSetupTestSourceMenu_act;

void AmuletWellSetupTestNuclide_menu(void){
	char message[25];

	switch(m_iPhase){
		case PHASE_WELLSETUPTESTNUCLIDE_PRE_INIT:
			if(m_ucClear == 22){
				AmuletWellSetupTestSourceMenu_calDate = AmuletWellMainScreenMenu_getWellStandardDate();
				if(AmuletWellSetupTestSourceMenu_calDate == (time_t)0){
					AmuletWellSetupTestSourceMenu_SN[0] = 0;
					AmuletWellSetupTestSourceMenu_act = -1;
				}else{
					AmuletWellMainScreenMenu_getWellStandardSN(AmuletWellSetupTestSourceMenu_SN);
					AmuletWellSetupTestSourceMenu_act = AmuletWellMainScreenMenu_getWellStandardActivity();
				}
				m_ucClear = 0;
			}else{
				if((AmuletWellSetupTestSourceMenu_SN[0] == 0) || (AmuletWellSetupTestSourceMenu_calDate == (time_t)0) || (AmuletWellSetupTestSourceMenu_act == -1)){
					AmuletWellMainScreenMenu_updateWellStandardSN("");
					AmuletWellMainScreenMenu_updateWellStandardDate((time_t) 0);
					AmuletWellMainScreenMenu_updateWellStandardActivity(-1);
					AmuletWellMainScreenMenu_WellStandardMirrorUpdate();
				}else{
					AmuletWellMainScreenMenu_updateWellStandardSN(AmuletWellSetupTestSourceMenu_SN);
					AmuletWellMainScreenMenu_updateWellStandardDate(AmuletWellSetupTestSourceMenu_calDate);
					AmuletWellMainScreenMenu_updateWellStandardActivity(AmuletWellSetupTestSourceMenu_act);
					AmuletWellMainScreenMenu_WellStandardMirrorUpdate();
				}
			}

			SetAmuletString(100, AmuletWellSetupTestSourceMenu_SN);

			if(AmuletWellSetupTestSourceMenu_calDate == (time_t)0){
				SetAmuletString(101, "");
			}else{
				GetExtendedTimeInfo(&AmuletWellSetupTestSourceMenu_calDate, message);
				SetAmuletString(101, message);
			}

			if(AmuletWellSetupTestSourceMenu_act == -1){
				SetAmuletString(102, "");
			}else{
				if(current.system == CI) format_activity_system2(AmuletWellSetupTestSourceMenu_act, message);
				else format_activity_system_kbq(AmuletWellSetupTestSourceMenu_act, message);
				SetAmuletString(102, message);
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLSETUPTESTNUCLIDE_WAIT;
			break;

		case PHASE_WELLSETUPTESTNUCLIDE_WAIT:
			break;
	}
}
