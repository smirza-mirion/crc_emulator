/**
 * \file
 * \details This file contains function, which calls the I2C layer and implements communications to the EEPROM
 */
/*********************************************************************************************
  MODULE:  EEProm Routines

  FILE:    EepromRoutines.c

  DATE:    07/14/04

  *********************************************************************************************/    
#include "crc.h"
#include "i2c.h"
#include "pit.h"
#include "amulet.h"

extern bool CalibratorMain_WelcomeScreen;

unsigned char m_ucVolumeMirror;
unsigned char m_ucVolumeCurrent;
uchar EepromRoutines_brightnessMirror;
uchar EepromRoutines_brightnessCurrent;
uchar EepromRoutines_sleepBrightnessMirror;
uchar EepromRoutines_sleepBrightnessCurrent;
uchar EepromRoutines_sleepTimeoutMirror;
uchar EepromRoutines_sleepTimeoutCurrent;

static bool inSleep;
static bool sleepActive;
static unsigned long int sleepTimeout;

unsigned char CurrentPageStack(void);

/**********
    write data to the EEPROM
    
    each byte sent as: high byte of address, low byte of address, data
*********/
/**
 * \details Write data to EEPROM
 * \param offset Address in the EEPROM to begin writing
 * \param data Pointer to byte data array
 * \param nbytes Number of bytes to write
 * \param iEEType 0 = Calibrator EEPROM, 1 = MCA EEPROM
 * \returns True = Successful, False = Error
 */
bool eewrite(ushort offset, uchar *data, int nbytes, int iEEType){
	uchar cmd[4];
	uchar hibyte, lobyte;
	ushort addr;
	int i;
	short device_addr;

	switch(iEEType){
		case EE_CRC:
			device_addr = EEPROM_SLAVE_ADDRESS;
			break;

		case EE_MCA:
			device_addr = EEPROM_MCA_SLAVE_ADDRESS;
			break;

		case EE_AUX:
			device_addr = EEPROM_AUX_SLAVE_ADDRESS;
			break;
	}

	//setup the i2c
	setup_i2c();

	addr = offset;
        
	//for each byte, write eeprom offset, data & delay 20msec
	for(i=0; i<nbytes; i++){
		hibyte = (uchar)(addr >> 8);
		lobyte = (uchar)(addr & 0xff);
		cmd[0] = hibyte;
		cmd[1] = lobyte;
		cmd[2] = data[i];
		if(!i2c_write(device_addr, cmd, 3, TRUE)) return FALSE;
		delay_msec(20);
		++addr;
	}
	return TRUE;
}

	
// read from the EEPROM
/**
 * \details Read data from EEPROM
 * \param data Pointer to byte data array, which receives the data from the EEPROM
 * \param offset Address in the EEPROM to begin writing
 * \param nbytes Number of bytes to read
 * \param iEEType 0 = Calibrator EEPROM, 1 = MCA EEPROM
 * \returns True = Successful, False = Error
 */
bool eeread(uchar *data, ushort offset, int nbytes, int iEEType){
	uchar cmd[10];
	uchar hibyte,lobyte;
	short device_addr;

	switch(iEEType){
		case EE_CRC:
			device_addr = EEPROM_SLAVE_ADDRESS;
			break;

		case EE_MCA:
			device_addr = EEPROM_MCA_SLAVE_ADDRESS;
			break;

		case EE_AUX:
			device_addr = EEPROM_AUX_SLAVE_ADDRESS;
			break;
	}

	//setup the i2c
	setup_i2c();

	//send offset
	hibyte = (uchar)(offset >> 8);
	lobyte = (uchar)(offset & 0xff);
	cmd[0] = hibyte;
	cmd[1] = lobyte;

	if(!i2c_write(device_addr, cmd, 2, FALSE)) return FALSE;   //error return

	// read the data
	if(!i2c_read(device_addr, data, nbytes, TRUE)) return FALSE;

	return TRUE;
}
/**
 * \details Read the buzzer volume from the EEPROM, updates the m_ucVolumeMirror variable and set the volume with this reading
 * \returns None
 */
void VolumeMirror(void){
	EE_READ(volume, (uchar *) &m_ucVolumeMirror);
	SetVolume(m_ucVolumeMirror);
}
/**
 * \details Write the buzzer volume to the EERPOM with the value in m_ucVolumeCurren, updates the m_ucVolumeMirror variable
 * \returns None
 */
void VolumeMirrorUpdate(void){
	if(m_ucVolumeMirror != m_ucVolumeCurrent){
		EE_WRITE(volume, (uchar *) &m_ucVolumeCurrent);
		m_ucVolumeMirror = m_ucVolumeCurrent;
	}
}
/**
 * \details Read the screen brightness from the EEPROM, updates the EepromRoutines_brightnessMirror and the EepromRoutines_brightnessCurrent variables
 * \returns None
 */
void BrightnessMirror(void){
	EE_READ(brightness, &EepromRoutines_brightnessMirror);
	if(EepromRoutines_brightnessMirror > 100){
		EepromRoutines_brightnessMirror = 100;
		EE_WRITE(brightness, &EepromRoutines_brightnessMirror);
	}
	else if(EepromRoutines_brightnessMirror < 10){
		EepromRoutines_brightnessMirror = 10;
		EE_WRITE(brightness, &EepromRoutines_brightnessMirror);
	}

	EepromRoutines_brightnessCurrent = EepromRoutines_brightnessMirror;
}
/**
 * \details Write the screen brightness to the EEPROM with the value in EepromRoutines_brigthnessCurrent
 * \returns None
 */
void BrightnessMirrorUpdate(void){
	if(EepromRoutines_brightnessMirror != EepromRoutines_brightnessCurrent){
		EE_WRITE(brightness, &EepromRoutines_brightnessCurrent);
		EepromRoutines_brightnessMirror = EepromRoutines_brightnessCurrent;
	}
}
/**
 * \details Read the sleep screen brightness from the EEPROM, updates the EepromRoutines_sleepBrightnessMirror and the EepromRoutines_sleepBrightnessCurrent variables
 * \returns None
 */
void SleepBrightnessMirror(void){
	EE_READ(sleepbrightness, &EepromRoutines_sleepBrightnessMirror);
	if(EepromRoutines_sleepBrightnessMirror > 100){
		EepromRoutines_sleepBrightnessMirror = 100;
		EE_WRITE(sleepbrightness, &EepromRoutines_sleepBrightnessMirror);
	}else if(EepromRoutines_sleepBrightnessMirror < 10){
		EepromRoutines_sleepBrightnessMirror = 10;
		EE_WRITE(sleepbrightness, &EepromRoutines_sleepBrightnessMirror);
	}
	EepromRoutines_sleepBrightnessCurrent = EepromRoutines_sleepBrightnessMirror;
}
/**
 * \details Write the sleep screen brightness to EEPROM with the value in EepromRoutines_sleepBrightnessCurrent
 */
void SleepBrightnessUpdate(void){
	if(EepromRoutines_sleepBrightnessMirror != EepromRoutines_sleepBrightnessCurrent){
		EE_WRITE(sleepbrightness, &EepromRoutines_sleepBrightnessCurrent);
		EepromRoutines_sleepBrightnessMirror = EepromRoutines_sleepBrightnessCurrent;
	}
}
/**
 * \details Updates the sleepTimeout variable with current time stamp
 * \returns None
 */
void SleepRefreshTimeout(void){
	sleepTimeout = EepromRoutines_sleepTimeoutCurrent;
	sleepTimeout *= 100;
	sleepTimeout *= 60;
	sleepTimeout += g_csec_tstamp;
}
/**
 * \details Read sleep timeout from EEPROM and write results into EepromRoutines_sleepTimeoutMirror and EepromRoutines_sleepTimeoutCurrent variables
 * \returns None
 */
void SleepTimeoutMirror(void){
	EE_READ(sleeptimeout, &EepromRoutines_sleepTimeoutMirror);
	if(EepromRoutines_sleepTimeoutMirror > 60){
		EepromRoutines_sleepTimeoutMirror = 60;
		EE_WRITE(sleeptimeout, &EepromRoutines_sleepTimeoutMirror);
	}
	EepromRoutines_sleepTimeoutCurrent = EepromRoutines_sleepTimeoutMirror;
	if(EepromRoutines_sleepTimeoutCurrent==0) sleepActive = FALSE;
	else{
		SleepRefreshTimeout();
		sleepActive = TRUE;
	}
}
/**
 * \details Write sleep timeout to EEPROM with the value in EepromRoutines_sleepTimeoutCurrent
 * \returns None
 */
void SleepTimeoutUpdate(void){
	if(EepromRoutines_sleepTimeoutMirror != EepromRoutines_sleepTimeoutCurrent){
		EE_WRITE(sleeptimeout, &EepromRoutines_sleepTimeoutCurrent);
		EepromRoutines_sleepTimeoutMirror = EepromRoutines_sleepTimeoutCurrent;
		if(EepromRoutines_sleepTimeoutCurrent==0) sleepActive = FALSE;
		else{
			SleepRefreshTimeout();
			sleepActive = TRUE;
		}
	}
}
/**
 * \details Test if sleepTimeout variable exceeds the current time, which indicates the screen should be put to sleep
 * \returns True = Put screen to sleep, False = Leave screen alone
 */
bool EepromRoutines_sleepNow(void){
	bool returnValue;
	if(sleepActive){
		if(inSleep) returnValue = FALSE;
		else{
			if(CalibratorMain_WelcomeScreen == 0){
				if(g_csec_tstamp > sleepTimeout){
					if((CurrentPageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]) || (CurrentPageStack() == AmuletHTMLIndex[WELLMAINSCREEN_HTM]) || (CurrentPageStack() == AmuletHTMLIndex[BETAMAINSCREEN_HTM])){
						inSleep = TRUE;
						returnValue = TRUE;
					}else{
						returnValue = FALSE;
					}
				}
				else returnValue = FALSE;
			} else returnValue = FALSE;
		}
	}else{
		returnValue = FALSE;
	}
	return returnValue;
}
/**
 * \details Wake screen from sleep by setting the inSleep variable to False
 * \returns None
 */
void EepromRoutines_sleepWake(void){
	inSleep = FALSE;
}
