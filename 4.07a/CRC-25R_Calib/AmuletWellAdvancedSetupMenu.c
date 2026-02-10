#define PHASE_WELLADVANCEDSETUP_PRE_INIT	0
#define PHASE_WELLADVANCEDSETUP_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "database.h"
#include "amulet.h"

extern int m_iPhase;

bool AmuletWellAdvancedSetup_accepted;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);

void AmuletWellAdvancedSetup_menu(void){
	//USER user;
	DETECTOR *probe;
	DETECTOR *well;

	switch(m_iPhase){
		case PHASE_WELLADVANCEDSETUP_PRE_INIT:
			/*if(m_ucClear == 78){
				m_ucClear = 0;
				DB_GetUserFromID(current.user_id, &user);
				if((current.security_mode == 0) || (user.Role < 2 && current.security_mode == 1) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 2;
					m_ucClear = 75;
					SetAmuletHTML(LOGIN_HTM);
					PushPageStack(LOGIN_HTM);
					AmuletWellAdvancedSetup_accepted = FALSE;
					return;
				}else{
					AmuletWellAdvancedSetup_accepted = TRUE;
				}
			}

			DB_GetUserFromID(current.user_id, &user);
			if(!AmuletWellAdvancedSetup_accepted || user.Role < 2){
				SetAmuletBackHTML();
				return;
			} */

			Mca_sendDetectorType();
			SetAmuletByte(75, 0xFF);
			if(Mca_isMultiDetector()) SetAmuletByte(76, 0xFF);

			//if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) SetAmuletByte(100, 0xFF);

			if(Mca_installedDetector == DET_WELL){
				SetAmuletByte(90, 0xFF);
			}else if(Mca_installedDetector == DET_DRILLEDPROBE700){
				SetAmuletByte(90, 0xFF);
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(92, 0xFF);
				SetAmuletByte(93, 0xFF);
			}else{
				well = Mca_detectorWell700Mirror();
				probe = Mca_detectorProbe700Mirror();

				if((well->installed) == DET_WELL700){
					SetAmuletByte(90, 0xFF);
				}
				if((probe->installed) == DET_PROBE700){
					SetAmuletByte(91, 0xFF);
					SetAmuletByte(92, 0xFF);
					SetAmuletByte(93, 0xFF);
				}
			}

			SetAmuletByte(100, 0xFF);

			m_iPhase = PHASE_WELLADVANCEDSETUP_WAIT;
			break;

		case PHASE_WELLADVANCEDSETUP_WAIT:
			break;
	}
}
