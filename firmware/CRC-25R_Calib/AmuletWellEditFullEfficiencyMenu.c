#define PHASE_WELLEDITFULLEFF_PRE_INIT	0
#define PHASE_WELLEDITFULLEFF_WAIT		1
#define PHASE_WELLEDITFULLEFF_SAVE		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

float AmuletWellEditFullEfficiency_fullEfficiency;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);

void AmuletWellEditFullEfficiency_menu(void){
	char message[26];

	switch(m_iPhase){
		case PHASE_WELLEDITFULLEFF_PRE_INIT:
			if(m_ucClear == 39){
				AmuletWellEditFullEfficiency_fullEfficiency = Mca_getFullEfficiency();
				m_ucClear = 0;
			}
			sprintf(message, "%.3f %%", AmuletWellEditFullEfficiency_fullEfficiency);
			SetAmuletString(100, message);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLEDITFULLEFF_WAIT;
			break;

		case PHASE_WELLEDITFULLEFF_WAIT:
			break;

		case PHASE_WELLEDITFULLEFF_SAVE:
			beep_amulet();
			Mca_updateFullEfficiency(AmuletWellEditFullEfficiency_fullEfficiency);
			Mca_fullEfficiencyMirrorUpdate();
			SetAmuletBackHTML();
			break;
	}
}
