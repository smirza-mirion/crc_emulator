#define PHASE_WELLFACTORY_PRE_INIT	0
#define PHASE_WELLFACTORY_WAIT		1
#define PHASE_WELLFACTORY_SAVE		2
#define PHASE_WELLFACTORY_256		3
#define PHASE_WELLFACTORY_512		4
#define PHASE_WELLFACTORY_1024		5
#define PHASE_WELLFACTORY_2048		6
#define PHASE_WELLFACTORY_4096		7

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "mca.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;

char AmuletWellFactoryMenu_serialNum[7];
short AmuletWellFactoryMenu_highVoltage;
short AmuletWellFactoryMenu_threshold;
int AmuletWellFactoryMenu_channels;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void Mca_setSerialMirror(char *value, short detectorType);
void Mca_testDetectorChange(char *serialnum, short threshold, short hv, short installed, int num_of_channels);
void Mca_setHVMirror(short value, short detectorType);
void Mca_setThresholdMirror(short value, short detectorType);
void Mca_setChannelsMirror(int num_of_channels, short detectorType);
int MCASetThreshold(short value);
int MCASetHV(short value, bool state);
void SetAmuletBackHTML(void);

void AmuletWellFactory_menu(void){
	DETECTOR detectorMirror;
	short index;
	float hv, hvcode;
	char message[51];

	switch(m_iPhase){
		case PHASE_WELLFACTORY_PRE_INIT:
			if(m_ucClear == 44){
				DB_PopDetector(&detectorMirror, Mca_installedDetector);
				for(index=0;index<6;index++) AmuletWellFactoryMenu_serialNum[index] = detectorMirror.snum[index];
				AmuletWellFactoryMenu_serialNum[6] = 0;
				AmuletWellFactoryMenu_highVoltage = detectorMirror.hv;
				AmuletWellFactoryMenu_threshold = detectorMirror.threshold;
				AmuletWellFactoryMenu_channels = detectorMirror.num_of_channels;
				m_ucClear = 0;
			}

			SetAmuletString(100, AmuletWellFactoryMenu_serialNum);
			hvcode = AmuletWellFactoryMenu_highVoltage;

			hv = (.158974 * hvcode) + 595.0;
			//sprintf(message, "%.0f Volts (%d)", hv, AmuletWellFactoryMenu_highVoltage);
			sprintf(message, "%.0f Volts", hv);
			SetAmuletString(101, message);

			sprintf(message, "%d", AmuletWellFactoryMenu_threshold);
			SetAmuletString(103, message);

			switch(AmuletWellFactoryMenu_channels){
				case 256:
					SetAmuletByte(99, 1);
					break;

				case 512:
					SetAmuletByte(99, 2);
					break;

				case 1024:
					SetAmuletByte(99, 3);
					break;

				case 2048:
					SetAmuletByte(99, 4);
					break;

				case 4096:
					SetAmuletByte(99, 5);
					break;
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLFACTORY_WAIT;
			break;

		case PHASE_WELLFACTORY_WAIT:
			break;

		case PHASE_WELLFACTORY_SAVE:
			beep_amulet();
			Mca_testDetectorChange(AmuletWellFactoryMenu_serialNum, AmuletWellFactoryMenu_threshold, AmuletWellFactoryMenu_highVoltage, Mca_installedDetector, AmuletWellFactoryMenu_channels);
			DB_WriteWellSerialNum(AmuletWellFactoryMenu_serialNum, Mca_installedDetector);
			DB_WriteWellHV(AmuletWellFactoryMenu_highVoltage, Mca_installedDetector);
			DB_WriteWellThreshold(AmuletWellFactoryMenu_threshold, Mca_installedDetector);
			DB_WriteWellChannels(AmuletWellFactoryMenu_channels, Mca_installedDetector);
			Mca_setSerialMirror(AmuletWellFactoryMenu_serialNum, Mca_installedDetector);
			Mca_setHVMirror(AmuletWellFactoryMenu_highVoltage, Mca_installedDetector);
			Mca_setThresholdMirror(AmuletWellFactoryMenu_threshold, Mca_installedDetector);
			Mca_setChannelsMirror(AmuletWellFactoryMenu_channels, Mca_installedDetector);
			MCASetHV(AmuletWellFactoryMenu_highVoltage, Mca_getHVState());
			MCASetThreshold(AmuletWellFactoryMenu_threshold);
			SetAmuletBackHTML();
			break;

		case PHASE_WELLFACTORY_256:
			beep_amulet();
			AmuletWellFactoryMenu_channels = 256;
			m_iPhase = PHASE_WELLFACTORY_WAIT;
			break;

		case PHASE_WELLFACTORY_512:
			beep_amulet();
			AmuletWellFactoryMenu_channels = 512;
			m_iPhase = PHASE_WELLFACTORY_WAIT;
			break;

		case PHASE_WELLFACTORY_1024:
			beep_amulet();
			AmuletWellFactoryMenu_channels = 1024;
			m_iPhase = PHASE_WELLFACTORY_WAIT;
			break;

		case PHASE_WELLFACTORY_2048:
			beep_amulet();
			AmuletWellFactoryMenu_channels = 2048;
			m_iPhase = PHASE_WELLFACTORY_WAIT;
			break;

		case PHASE_WELLFACTORY_4096:
			beep_amulet();
			AmuletWellFactoryMenu_channels = 4096;
			m_iPhase = PHASE_WELLFACTORY_WAIT;
			break;
	}
}
