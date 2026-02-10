#define PHASE_WELLMAIN_PRE_INIT	0
#define PHASE_WELLMAIN_WAIT		1
#define PHASE_WELLMAIN_PRINT	2
#define PHASE_WELLMAIN_WAIT_2_SEC 3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "i2c.h"
#include "counter.h"
#include "amulet.h"
#include "mca.h"
#include "printer.h"
#include "database.h"
#include "wipes.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern const short max_chambers;
extern REMOTE remote[];
extern time_t clock_time;
extern volatile bool DisplayActivity_SendActivity2;
extern const char *rev_num;
extern bool CalibratorMain_WelcomeScreen;
extern volatile unsigned long int g_csec_tstamp;

char wipeNucMirror[10];
char unrestrictedNucMirror[10];
char sealedNucMirror[99];
STAND wellStandardMirror;
float wellTriggerMirror[4];

void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);

void AmuletWellMainScreen_menu(void){
	short index;
	char message[51];
	static unsigned long int twosec_tstamp;

	switch(m_iPhase){
		case PHASE_WELLMAIN_PRE_INIT:
			CalibratorMain_WelcomeScreen = FALSE;
			m_ucClear = 0;
			//if(current.num_chambers>0) SetAmuletByte(100, 0xFF);

			for(index=0; index<max_chambers; index++){
				remote[index].keys_active = FALSE;
			}

			GetExtendedTimeInfoSec(&clock_time, message);
			SetAmuletString(2, message);
			DisplayActivity_SendActivity2 = TRUE;

			if(current.demo_mode){
				//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			}

			if(current.branding == 0 ) sprintf(message, "CRC-55t, %s", rev_num);
			else if(current.branding == 1) sprintf(message, "CAPRAC-t, %s", rev_num);
			else if(current.branding == 2) sprintf(message, "CAPTUS-700t, %s", rev_num);
			else if(current.branding == 3 ) sprintf(message, "CRC-77t, %s", rev_num);
			else sprintf(message, "%s", rev_num);
			SetAmuletString(100, message);
			SetAmuletByte(102, 0xFF);

			Mca_sendDetectorType();
			SetAmuletByte(75, 0xFF);

			if(Mca_isMultiDetector()) SetAmuletByte(76, 0xFF);

			if(Mca_hasWell() && Mca_hasProbe()) SetAmuletByte(103, 0xFF);
			else if(Mca_hasWell() && !Mca_hasProbe()) SetAmuletByte(104, 0xFF);
			else if(!Mca_hasWell() && Mca_hasProbe()) SetAmuletByte(105, 0xFF);

			//delayloop(201);
			//if(current.num_chambers>0) SetAmuletByte(100, 0xFF);

			if(current.num_chambers>0){
				twosec_tstamp = g_csec_tstamp + 210;
				m_iPhase = PHASE_WELLMAIN_WAIT_2_SEC;
			}
			else m_iPhase = PHASE_WELLMAIN_WAIT;
			break;

		case PHASE_WELLMAIN_WAIT_2_SEC:
			if(g_csec_tstamp > twosec_tstamp){
				SetAmuletByte(100, 0xFF);
				m_iPhase = PHASE_WELLMAIN_WAIT;
			}
			break;

		case PHASE_WELLMAIN_WAIT:
			break;

		case PHASE_WELLMAIN_PRINT:
			beep_amulet();
			graphics_test();
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_WELLMAIN_WAIT;
			break;
	}
}

void AmuletWellMainScreenMenu_wellStandardMirror(void){
	DB_PopDetectorStandard(&wellStandardMirror);
}

void AmuletWellMainScreenMenu_getWellStandardSN(char *SN){
	strcpy(SN, wellStandardMirror.sn);
}

void AmuletWellMainScreenMenu_updateWellStandardSN(char *SN){
	strcpy(wellStandardMirror.sn, SN);
}

time_t AmuletWellMainScreenMenu_getWellStandardDate(void){
	return wellStandardMirror.caldate;
}

void AmuletWellMainScreenMenu_updateWellStandardDate(time_t calDate){
	wellStandardMirror.caldate = calDate;
}

float AmuletWellMainScreenMenu_getWellStandardActivity(void){
	return wellStandardMirror.act;
}

void AmuletWellMainScreenMenu_updateWellStandardActivity(float act){
	wellStandardMirror.act = act;
}

void AmuletWellMainScreenMenu_WellStandardMirrorUpdate(void){
	DB_WriteDetectorStandard(&wellStandardMirror);
}
