#define PHASE_WELLSETUPTRIGGERLEVEL_PRE_INIT	0
#define PHASE_WELLSETUPTRIGGERLEVEL_WAIT		1
#define PHASE_WELLSETUPTRIGGERLEVEL_ACCEPT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "counter.h"
#include "mca.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);

float AmuletWellSetupTriggerLevelMenu_wellTrigger[4];

void AmuletWellSetupTriggerLevel_menu(void){
	float convertedvalue;
	char message[26];

	switch(m_iPhase){
		case PHASE_WELLSETUPTRIGGERLEVEL_PRE_INIT:
			if(m_ucClear == 23){
				m_ucClear = 0;
			}

			if(current.system == CI){
				sprintf(message, "%.2f cpm", AmuletWellSetupTriggerLevelMenu_wellTrigger[BKG_MODE]);
				SetAmuletString(100, message);

				sprintf(message, "%.2f dpm", AmuletWellSetupTriggerLevelMenu_wellTrigger[WIPE_MODE]);
				SetAmuletString(101, message);

				sprintf(message, "%.2f dpm", AmuletWellSetupTriggerLevelMenu_wellTrigger[UNRES_MODE]);
				SetAmuletString(102, message);

				convertedvalue = Mca_convertDpmToCi(AmuletWellSetupTriggerLevelMenu_wellTrigger[SEALED_MODE]);
				convertedvalue *=  1e+9;
				sprintf(message, "%.3f nCi", convertedvalue);
				SetAmuletString(103, message);
			}else{
				sprintf(message, "%.3f cps", Mca_convertDpmToBq(AmuletWellSetupTriggerLevelMenu_wellTrigger[BKG_MODE]));
				SetAmuletString(100, message);

				sprintf(message, "%.3f Bq", Mca_convertDpmToBq(AmuletWellSetupTriggerLevelMenu_wellTrigger[WIPE_MODE]));
				SetAmuletString(101, message);

				sprintf(message, "%.3f Bq", Mca_convertDpmToBq(AmuletWellSetupTriggerLevelMenu_wellTrigger[UNRES_MODE]));
				SetAmuletString(102, message);

				sprintf(message, "%.3f Bq", Mca_convertDpmToBq(AmuletWellSetupTriggerLevelMenu_wellTrigger[SEALED_MODE]));
				SetAmuletString(103, message);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLSETUPTRIGGERLEVEL_WAIT;
			break;

		case PHASE_WELLSETUPTRIGGERLEVEL_WAIT:
			break;

		case PHASE_WELLSETUPTRIGGERLEVEL_ACCEPT:
			beep_amulet();
			SetAmuletBackHTML();
			break;
	}
}
