#define PHASE_WELLMEASUREMENTS_PRE_INIT	0
#define PHASE_WELLMEASUREMENTS_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"

extern int m_iPhase;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);

void AmuletWellMeasurements_menu(void){
	switch(m_iPhase){
		case PHASE_WELLMEASUREMENTS_PRE_INIT:
			Mca_sendDetectorType();
			SetAmuletByte(75, 0xFF);

			if(Mca_isMultiDetector()) SetAmuletByte(76, 0xFF);
			if(Mca_hasWell()) SetAmuletByte(100, 0xFF);
			if(Mca_installedDetector == DET_DRILLEDPROBE700){
				SetAmuletByte(101, 0x00);
				SetAmuletByte(102, 0xFF);
			}else{
				SetAmuletByte(102, 0x00);
				SetAmuletByte(101, 0xFF);
			}
			m_iPhase = PHASE_WELLMEASUREMENTS_WAIT;
			break;

		case PHASE_WELLMEASUREMENTS_WAIT:
			break;
	}
}
