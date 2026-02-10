#define PHASE_WELLMANUAL_PRE_INIT	0
#define PHASE_WELLMANUAL_WAIT		1
#define PHASE_WELLMANUAL_CLEARLIN	2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"

extern int m_iPhase;
extern float AmuletWellAutoCalibrate_linMeas[5];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);

bool AmuletWellManual_showClearEu152LinCorr;

void AmuletWellManual_menu(void){
	char message[26];
	float hv, hvcode;

	switch(m_iPhase){
		case PHASE_WELLMANUAL_PRE_INIT:
			sprintf(message, "%d", Mca_getGain1());
			SetAmuletString(100, message);

			sprintf(message, "%d", Mca_getGain2());
			SetAmuletString(101, message);

			sprintf(message, "%d", Mca_getZeroOpAmpOffset());
			SetAmuletString(102, message);

			sprintf(message, "%d", Mca_getThreshold());
			SetAmuletString(103, message);

			hvcode = Mca_getHVValue();
			if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
			else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;
			sprintf(message, "%.0f Volts (%.0f)", hv, hvcode);
			SetAmuletString(104, message);

			//sprintf(message, "%d", Mca_getAcquireTime());
			//SetAmuletString(105, message);

			//if(Mca_getHVState()) SetAmuletByte(101, 0xFF);
			//else SetAmuletByte(101, 0x00);

			//if(Mca_getTimeType() == COUNT_REAL) SetAmuletByte(102, 0x00);
			//else SetAmuletByte(102, 0xFE);

			SetAmuletByte(100, 0xFF);

			if(AmuletWellManual_showClearEu152LinCorr == TRUE) SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_WELLMANUAL_WAIT;
			break;

		case PHASE_WELLMANUAL_WAIT:
			break;

		case PHASE_WELLMANUAL_CLEARLIN:
			AmuletWellAutoCalibrate_linMeas[1] = -999.0;
			AmuletWellAutoCalibrate_linMeas[2] = -999.0;
			AmuletWellAutoCalibrate_linMeas[3] = -999.0;
			Amulet_DisplayNotification("Eu152 Linearity Correction", "Eu152 Linearity Correction settings will be cleared,\nwhen the Auto Calibration is accepted", TRUE);
			return;
	}
}
