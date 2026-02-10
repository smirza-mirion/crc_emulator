#define PHASE_PCQC_PRE_INIT	0
#define PHASE_PCQC_WAIT		1
#define PHASE_PCQC_ABORT	2
#define PHASE_PCQC_WAIT2	3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "message.h"

extern int m_iPhase;
extern uchar EepromRoutines_brightnessCurrent;

//char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
//char SetAmuletString(unsigned char ucIndex, char *pcValue);
volatile bool AmuletPCQC_Abort;
void EepromRoutines_sleepWake(void);

void AmuletPCQC_menu(void){
	ushort brightness, brightness2;
	unsigned char ucValue2;
	double double_value;

	switch(m_iPhase){
		case PHASE_PCQC_PRE_INIT:
			brightness = EepromRoutines_brightnessCurrent;
			if(brightness<10) brightness = 10;
			brightness *= 10;
			if(brightness>999) brightness = 999;
			if(INVERSION) brightness = 1000 - brightness;
			if (brightness<10) brightness = 10;
			SetAmuletWord(100, brightness);
			brightness2 = 900 - brightness;
			double_value = brightness2;
			double_value *= 235.0;
			double_value /= 890.0;
			double_value += 20.0;
			brightness2 = double_value;
			if(brightness2 > 255) brightness2 = 255;
			if(brightness2 < 20) brightness2 = 20;
			ucValue2 = brightness2;
			SetAmuletByte(55, ucValue2);
			EepromRoutines_sleepWake();

			//SetAmuletString(100, "PC Communications Mode");
			send_amulet_message(L_PC_COMMUNICATIONS_MODE, 100);    // "PC Communications Mode"
			//SetAmuletString(102, "Running QC tests from PC");
			send_amulet_message(L_RUNNING_QC_TESTS_FROM_PC, 102);    // "Running QC tests from PC"
			send_amulet_message(L_ABORT_QC_TESTS, 106);    // "Abort QC Tests"
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_PCQC_WAIT;
			break;

		case PHASE_PCQC_WAIT:
			break;

		case PHASE_PCQC_WAIT2:
			break;

		case PHASE_PCQC_ABORT:
			beep_amulet();
			AmuletPCQC_Abort = TRUE;
			m_iPhase = PHASE_PCQC_WAIT2;
			break;
	}
}
