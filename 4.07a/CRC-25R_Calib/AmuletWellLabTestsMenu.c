#define PHASE_WELLLABTESTS_PRE_INIT	0
#define PHASE_WELLLABTESTS_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"

extern int m_iPhase;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);

void AmuletWellLabTests_menu(void){
	switch(m_iPhase){
		case PHASE_WELLLABTESTS_PRE_INIT:
			Mca_switchToWell();
			Mca_sendDetectorType();
			SetAmuletByte(75, 0xFF);
			m_iPhase = PHASE_WELLLABTESTS_WAIT;
			break;

		case PHASE_WELLLABTESTS_WAIT:
			break;
	}
}
