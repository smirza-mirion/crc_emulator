#define PHASE_WELLSETUP_PRE_INIT	0
#define PHASE_WELLSETUP_WAIT		1
#define PHASE_WELLSETUP_TEST_VOLUME		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucVolumeCurrent;
extern uchar EepromRoutines_brightnessCurrent;
extern short tone_duration;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletWord(unsigned char ucIndex, short unsigned int uiValue);

void AmuletWellSetup_menu(void){
	ushort brightness, brightness2;
	unsigned char ucValue;
	double double_value;

	switch(m_iPhase){
		case PHASE_WELLSETUP_PRE_INIT:
			m_ucClear = 0;
			SetAmuletByte(101, m_ucVolumeCurrent);
			SetAmuletByte(102, EepromRoutines_brightnessCurrent);
			SetAmuletByte(100, 0xFF);

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
			SetAmuletByte(59, 0xFF);
			m_iPhase = PHASE_WELLSETUP_WAIT;
			break;

		case PHASE_WELLSETUP_WAIT:
			break;

		case PHASE_WELLSETUP_TEST_VOLUME:
			if(tone_duration == 0){
				tone_duration = 32767;
				start_tone();
			}else{
				tone_duration = 1;
			}
			m_iPhase = PHASE_WELLSETUP_WAIT;
			break;
	}
}
