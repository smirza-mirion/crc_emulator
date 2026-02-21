#define PHASE_WELLFACTORYDETECTORS_PRE_INIT		0
#define PHASE_WELLFACTORYDETECTORS_WAIT			1
#define PHASE_WELLFACTORYDETECTORS_PROBE256		2
#define PHASE_WELLFACTORYDETECTORS_PROBE512		3
#define PHASE_WELLFACTORYDETECTORS_PROBE1024	4
#define PHASE_WELLFACTORYDETECTORS_PROBE2048	5
#define PHASE_WELLFACTORYDETECTORS_PROBE4096	6
#define PHASE_WELLFACTORYDETECTORS_PROBE		7
#define PHASE_WELLFACTORYDETECTORS_DRILLEDPROBE	8
#define PHASE_WELLFACTORYDETECTORS_PROBENONE	9
#define PHASE_WELLFACTORYDETECTORS_WELL256		10
#define PHASE_WELLFACTORYDETECTORS_WELL512		11
#define PHASE_WELLFACTORYDETECTORS_WELL1024		12
#define PHASE_WELLFACTORYDETECTORS_WELL2048		13
#define PHASE_WELLFACTORYDETECTORS_WELL4096		14
#define PHASE_WELLFACTORYDETECTORS_WELL			15
#define PHASE_WELLFACTORYDETECTORS_WELLNONE		16
#define PHASE_WELLFACTORYDETECTORS_SAVE			17

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "mca.h"
#include "database.h"
#include "amulet.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

char AmuletWellFactoryDetectorsMenu_probeSerialNum[7];
short AmuletWellFactoryDetectorsMenu_probeHighVoltage;
short AmuletWellFactoryDetectorsMenu_probeThreshold;
int AmuletWellFactoryDetectorsMenu_probeChannels;
short AmuletWellFactoryDetectorMenu_probeInstalled;
char AmuletWellFactoryDetectorsMenu_wellSerialNum[7];
short AmuletWellFactoryDetectorsMenu_wellHighVoltage;
short AmuletWellFactoryDetectorsMenu_wellThreshold;
int AmuletWellFactoryDetectorsMenu_wellChannels;
short AmuletWellFactoryDetectorMenu_wellInstalled;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void Mca_testDetectorChange(char *serialnum, short threshold, short hv, short installed, int num_of_channels);
void Mca_setSerialMirror(char *value, short detectorType);
void Mca_setHVMirror(short value, short detectorType);
void Mca_setThresholdMirror(short value, short detectorType);
void Mca_setChannelsMirror(int num_of_channels, short detectorType);
void Mca_setInstalledMirror(short installed, short detectorType);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void SetAmuletBackHTML(void);

void AmuletWellFactoryDetectors_menu(void){
	DETECTOR detectorMirror;
	short index;
	float hv, hvcode;
	char message[51];

	switch(m_iPhase){
		case PHASE_WELLFACTORYDETECTORS_PRE_INIT:
			SetAmuletLine(400, 85, 400, 522, 0x00007F, 0x01);

			if(m_ucClear == 72){
				DB_PopDetector(&detectorMirror, DET_PROBE700);
				for(index=0; index<6; index++) AmuletWellFactoryDetectorsMenu_probeSerialNum[index] = detectorMirror.snum[index];
				AmuletWellFactoryDetectorsMenu_probeSerialNum[6] = 0;
				AmuletWellFactoryDetectorsMenu_probeHighVoltage = detectorMirror.hv;
				AmuletWellFactoryDetectorsMenu_probeThreshold = detectorMirror.threshold;
				AmuletWellFactoryDetectorsMenu_probeChannels = detectorMirror.num_of_channels;
				AmuletWellFactoryDetectorMenu_probeInstalled = detectorMirror.installed;
				DB_PopDetector(&detectorMirror, DET_WELL700);
				for(index=0; index<6; index++) AmuletWellFactoryDetectorsMenu_wellSerialNum[index] = detectorMirror.snum[index];
				AmuletWellFactoryDetectorsMenu_wellSerialNum[6] = 0;
				AmuletWellFactoryDetectorsMenu_wellHighVoltage = detectorMirror.hv;
				AmuletWellFactoryDetectorsMenu_wellThreshold = detectorMirror.threshold;
				AmuletWellFactoryDetectorsMenu_wellChannels = detectorMirror.num_of_channels;
				AmuletWellFactoryDetectorMenu_wellInstalled = detectorMirror.installed;
				m_ucClear = 0;
			}

			SetAmuletString(101, AmuletWellFactoryDetectorsMenu_probeSerialNum);
			hvcode = AmuletWellFactoryDetectorsMenu_probeHighVoltage;
			hv = .263158 * hvcode;
			sprintf(message, "%.0f Volts", hv);
			SetAmuletString(102, message);
			sprintf(message, "%d", AmuletWellFactoryDetectorsMenu_probeThreshold);
			SetAmuletString(103, message);
			switch(AmuletWellFactoryDetectorsMenu_probeChannels){
				case 256:
					SetAmuletByte(96, 1);
					break;
				case 512:
					SetAmuletByte(96, 2);
					break;
				case 1024:
					SetAmuletByte(96, 3);
					break;
				case 2048:
					SetAmuletByte(96, 4);
					break;
				case 4096:
					SetAmuletByte(96, 5);
					break;
			}
			if(AmuletWellFactoryDetectorMenu_probeInstalled == DET_PROBE700) SetAmuletByte(97, 1);
			else if(AmuletWellFactoryDetectorMenu_probeInstalled == DET_DRILLEDPROBE700) SetAmuletByte(97, 2);
			else{
				AmuletWellFactoryDetectorMenu_probeInstalled = DET_EMPTY;
				SetAmuletByte(97, 3);
			}

			SetAmuletString(104, AmuletWellFactoryDetectorsMenu_wellSerialNum);
			hvcode = AmuletWellFactoryDetectorsMenu_wellHighVoltage;
			hv = .263158 * hvcode;
			sprintf(message, "%.0f Volts", hv);
			SetAmuletString(105, message);
			sprintf(message, "%d", AmuletWellFactoryDetectorsMenu_wellThreshold);
			SetAmuletString(106, message);
			switch(AmuletWellFactoryDetectorsMenu_wellChannels){
				case 256:
					SetAmuletByte(98, 1);
					break;
				case 512:
					SetAmuletByte(98, 2);
					break;
				case 1024:
					SetAmuletByte(98, 3);
					break;
				case 2048:
					SetAmuletByte(98, 4);
					break;
				case 4096:
					SetAmuletByte(98, 5);
					break;
			}
			if(AmuletWellFactoryDetectorMenu_wellInstalled == DET_WELL700) SetAmuletByte(99, 1);
			else{
				AmuletWellFactoryDetectorMenu_wellInstalled = DET_EMPTY;
				SetAmuletByte(99, 2);
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_WAIT:
			break;

		case PHASE_WELLFACTORYDETECTORS_PROBE256:
		case PHASE_WELLFACTORYDETECTORS_PROBE512:
		case PHASE_WELLFACTORYDETECTORS_PROBE1024:
		case PHASE_WELLFACTORYDETECTORS_PROBE2048:
		case PHASE_WELLFACTORYDETECTORS_PROBE4096:
			beep_amulet();
			AmuletWellFactoryDetectorsMenu_probeChannels = 256;
			AmuletWellFactoryDetectorsMenu_probeChannels <<= (m_iPhase - PHASE_WELLFACTORYDETECTORS_PROBE256);
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_PROBE:
			beep_amulet();
			AmuletWellFactoryDetectorMenu_probeInstalled = DET_PROBE700;
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_DRILLEDPROBE:
			beep_amulet();
			AmuletWellFactoryDetectorMenu_probeInstalled = DET_DRILLEDPROBE700;
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_PROBENONE:
			beep_amulet();
			AmuletWellFactoryDetectorMenu_probeInstalled = DET_EMPTY;
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_WELL256:
		case PHASE_WELLFACTORYDETECTORS_WELL512:
		case PHASE_WELLFACTORYDETECTORS_WELL1024:
		case PHASE_WELLFACTORYDETECTORS_WELL2048:
		case PHASE_WELLFACTORYDETECTORS_WELL4096:
			beep_amulet();
			AmuletWellFactoryDetectorsMenu_wellChannels = 256;
			AmuletWellFactoryDetectorsMenu_wellChannels <<= (m_iPhase - PHASE_WELLFACTORYDETECTORS_WELL256);
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_WELL:
			beep_amulet();
			AmuletWellFactoryDetectorMenu_wellInstalled = DET_WELL700;
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_WELLNONE:
			beep_amulet();
			AmuletWellFactoryDetectorMenu_wellInstalled = DET_EMPTY;
			m_iPhase = PHASE_WELLFACTORYDETECTORS_WAIT;
			break;

		case PHASE_WELLFACTORYDETECTORS_SAVE:
			beep_amulet();
			// Test for one installed detector
			if((AmuletWellFactoryDetectorMenu_probeInstalled == DET_EMPTY) && (AmuletWellFactoryDetectorMenu_wellInstalled == DET_EMPTY)){
				Amulet_DisplayError("Detector Settings Error", "One Detector must be installed", TRUE);
				return;
			}

			// Test for only one drilled probe detector
			if((AmuletWellFactoryDetectorMenu_probeInstalled == DET_DRILLEDPROBE700) && (AmuletWellFactoryDetectorMenu_wellInstalled == DET_WELL700)){
				Amulet_DisplayError("Detector Settings Error", "Drilled Probe can not be installed with a Well", TRUE);
				return;
			}

			Mca_testDetectorChange(AmuletWellFactoryDetectorsMenu_probeSerialNum, AmuletWellFactoryDetectorsMenu_probeThreshold, AmuletWellFactoryDetectorsMenu_probeHighVoltage, DET_PROBE700, AmuletWellFactoryDetectorsMenu_probeChannels);
			Mca_testDetectorChange(AmuletWellFactoryDetectorsMenu_wellSerialNum, AmuletWellFactoryDetectorsMenu_wellThreshold, AmuletWellFactoryDetectorsMenu_wellHighVoltage, DET_WELL700, AmuletWellFactoryDetectorsMenu_wellChannels);
			DB_WriteWellSerialNum(AmuletWellFactoryDetectorsMenu_probeSerialNum, DET_PROBE700);
			DB_WriteWellSerialNum(AmuletWellFactoryDetectorsMenu_wellSerialNum, DET_WELL700);
			DB_WriteWellHV(AmuletWellFactoryDetectorsMenu_probeHighVoltage, DET_PROBE700);
			DB_WriteWellHV(AmuletWellFactoryDetectorsMenu_wellHighVoltage, DET_WELL700);
			DB_WriteWellThreshold(AmuletWellFactoryDetectorsMenu_probeThreshold, DET_PROBE700);
			DB_WriteWellThreshold(AmuletWellFactoryDetectorsMenu_wellThreshold, DET_WELL700);
			DB_WriteWellChannels(AmuletWellFactoryDetectorsMenu_probeChannels, DET_PROBE700);
			DB_WriteWellChannels(AmuletWellFactoryDetectorsMenu_wellChannels, DET_WELL700);
			DB_WriteWellInstalled(AmuletWellFactoryDetectorMenu_probeInstalled, DET_PROBE700);
			DB_WriteWellInstalled(AmuletWellFactoryDetectorMenu_wellInstalled, DET_WELL700);
			Mca_setSerialMirror(AmuletWellFactoryDetectorsMenu_probeSerialNum, DET_PROBE700);
			Mca_setSerialMirror(AmuletWellFactoryDetectorsMenu_wellSerialNum, DET_WELL700);
			Mca_setHVMirror(AmuletWellFactoryDetectorsMenu_probeHighVoltage, DET_PROBE700);
			Mca_setHVMirror(AmuletWellFactoryDetectorsMenu_wellHighVoltage, DET_WELL700);
			Mca_setThresholdMirror(AmuletWellFactoryDetectorsMenu_probeThreshold, DET_PROBE700);
			Mca_setThresholdMirror(AmuletWellFactoryDetectorsMenu_wellThreshold, DET_WELL700);
			Mca_setChannelsMirror(AmuletWellFactoryDetectorsMenu_probeChannels, DET_PROBE700);
			Mca_setChannelsMirror(AmuletWellFactoryDetectorsMenu_wellChannels, DET_WELL700);
			Mca_setInstalledMirror(AmuletWellFactoryDetectorMenu_probeInstalled, DET_PROBE700);
			Mca_setInstalledMirror(AmuletWellFactoryDetectorMenu_wellInstalled, DET_WELL700);

			if((AmuletWellFactoryDetectorMenu_probeInstalled == DET_EMPTY) || (AmuletWellFactoryDetectorMenu_wellInstalled == DET_EMPTY)){
				if(AmuletWellFactoryDetectorMenu_probeInstalled == DET_EMPTY) Mca_switchDetector(AmuletWellFactoryDetectorMenu_wellInstalled);
				else Mca_switchDetector(AmuletWellFactoryDetectorMenu_probeInstalled);
			}else{
				if(Mca_installedDetector == DET_WELL700) Mca_switchDetector(AmuletWellFactoryDetectorMenu_wellInstalled);
				else Mca_switchDetector(AmuletWellFactoryDetectorMenu_probeInstalled);
			}

			SetAmuletBackHTML();
			break;
	}
}
