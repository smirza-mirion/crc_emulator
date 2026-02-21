/**
 * \file
 * \details This file handles calls from the Amulet Calibrator Serial Number Setup Screen
 */
#define PHASE_SETUPCALIBSERIAL_PRE_INIT	0
#define PHASE_SETUPCALIBSERIAL_WAIT		1
#define PHASE_SETUPCALIBSERIAL_SAVE		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "SL811.h"
#include "mca.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);

char AmuletSetupCalibSerialMenu_serial[11];

/**
 * \details Handles the Amulet Screen SetupCalibSerialNum.htm. SetupCalibSerialNum.htm setups calibrator serial number.
 * \param Amulet_Byte_ID Description
 * \param 100 Display Values
 * \returns None
 */
void AmuletSetupCalibSerial_menu(void){
	switch(m_iPhase){
		case PHASE_SETUPCALIBSERIAL_PRE_INIT:
			if(m_ucClear == 40){
				ReadSN(AmuletSetupCalibSerialMenu_serial);
				AmuletSetupCalibSerialMenu_serial[10] = 0;
				m_ucClear = 0;
			}

			SetAmuletString(100, AmuletSetupCalibSerialMenu_serial);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_SETUPCALIBSERIAL_WAIT;
			break;

		case PHASE_SETUPCALIBSERIAL_WAIT:
			break;

		case PHASE_SETUPCALIBSERIAL_SAVE:
			beep_amulet();
			WriteSN(AmuletSetupCalibSerialMenu_serial);
			DB_ReadPassword(current.password);
			current.bypass = -1;
			startup_811();
			SetAmuletBackHTML();
			break;
	}
}
