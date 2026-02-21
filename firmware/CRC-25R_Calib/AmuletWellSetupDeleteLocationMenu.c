#define PHASE_WELLSETUPDELETELOCATION_PRE_INIT	0
#define PHASE_WELLSETUPDELETELOCATION_WAIT		1
#define PHASE_WELLSETUPDELETELOCATION_DELETE	2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "wipes.h"
#include "mca.h"
#include "nuc.h"
#include "database.h"
#include "wipes.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern int AmuletWellSetupLocations_currentIndex;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void ThresholdToStr(char *message, char *TypeName, float threshold);

static WELLWIPELOCATION delLocation;

void AmuletWellSetupDeleteLocation_menu(void){
	WELLWIPELOCATION *activeLocation;
	char message[52];

	switch(m_iPhase){
		case PHASE_WELLSETUPDELETELOCATION_PRE_INIT:
			if(m_ucClear == 60){
				if(AmuletWellSetupLocations_currentIndex < 0){
					m_ucClear = 0;
					SetAmuletBackHTML();
					return;
				}else{
					activeLocation = ActiveWipeLocations();
					DB_CopyWipeLocation(&delLocation, &(activeLocation[AmuletWellSetupLocations_currentIndex]));
				}
				m_ucClear = 0;
			}

			SetAmuletString(100, delLocation.Name);
			SetAmuletString(102, delLocation.WellWipeTypeName);
			ThresholdToStr(message, delLocation.WellWipeTypeName, delLocation.Threshold);
			SetAmuletString(104, message);
			sprintf(message, "%d sec", delLocation.CountTime);
			SetAmuletString(106, message);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLSETUPDELETELOCATION_WAIT;
			break;

		case PHASE_WELLSETUPDELETELOCATION_WAIT:
			break;

		case PHASE_WELLSETUPDELETELOCATION_DELETE:
			beep_amulet();
			DeleteWipeLocation(&delLocation);
			SetAmuletBackHTML();
			return;

	}
}
