#define PHASE_WELLSETUPSEALED2_PRE_INIT	0
#define PHASE_WELLSETUPSEALED2_WAIT		1


#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "nuc.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

void AmuletWellSetupSealed2_menu(void){
	switch(m_iPhase){
		case PHASE_WELLSETUPSEALED2_PRE_INIT:
			m_ucClear = 0;
			m_iPhase = PHASE_WELLSETUPSEALED2_WAIT;
			break;

		case PHASE_WELLSETUPSEALED2_WAIT:
			break;
	}
}
