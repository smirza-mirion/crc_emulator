/**
 * \file
 * \details This file handles calls from the Amulet Sleep Screen
 */
#define PHASE_SLEEP_PRE_INIT	0
#define PHASE_SLEEP_WAIT		0x46
#define PHASE_SLEEP_WAKE		0xAD
#define PHASE_SLEEP_WAKE2		0xAE
#define PHASE_SLEEP_WAKE3		0xAF

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletWord(unsigned char ucIndex, short unsigned int uiValue);
void CallHTML(unsigned char ucValue);
void EepromRoutines_sleepWake(void);
unsigned char BasePageStack(void);

extern int m_iPhase;
extern uchar EepromRoutines_brightnessCurrent;
extern uchar EepromRoutines_sleepBrightnessCurrent;

/**
 * \details Handles the Amulet Screen Sleep.htm. Sleep.htm is a blank screen, which is display when the system sleeps.
 * \param Amulet_Byte_ID Description
 * \param 76 Change Brightness 0xFF = TOGGLE
 * \param Amulet_Word_ID Description
 * \param 76 Brightness
 * \returns None
 */
void AmuletSleep_menu(void){
	ushort brightness, brightness2;
	unsigned char ucValue2;
	double double_value;

	switch(m_iPhase){
		case PHASE_SLEEP_PRE_INIT:
			brightness = EepromRoutines_sleepBrightnessCurrent;
			if(brightness<10) brightness = 10;
			brightness *= 10;
			if(brightness>999) brightness = 999;
			if(INVERSION) brightness = 1000 - brightness;
			if(brightness<10) brightness = 10;
			SetAmuletWord(76, brightness);
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
			SetAmuletByte(76, 0xFF);
			m_iPhase = PHASE_SLEEP_WAIT;
			break;

		case PHASE_SLEEP_WAIT:
			break;

		case PHASE_SLEEP_WAKE:
			brightness = EepromRoutines_brightnessCurrent;
			if(brightness<10) brightness = 10;
			brightness *= 10;
			if(brightness>999) brightness = 999;
			if(INVERSION) brightness = 1000 - brightness;
			if(brightness<10) brightness = 10;
			SetAmuletWord(76, brightness);
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
			SetAmuletByte(77, 0xFF);
			m_iPhase = PHASE_SLEEP_WAKE2;
			break;

		case PHASE_SLEEP_WAKE2:
			break;

		case PHASE_SLEEP_WAKE3:
			EepromRoutines_sleepWake();
			if(BasePageStack() == AmuletHTMLIndex[MAINSCREEN_HTM]){
				CallHTML(SET_HTML_MAINSCREEN);
			}else if(BasePageStack() == AmuletHTMLIndex[WELLMAINSCREEN_HTM]){
				CallHTML(SET_HTML_WELLMAINSCREEN);
			}else if(BasePageStack() == AmuletHTMLIndex[BETAMAINSCREEN_HTM]){
				CallHTML(SET_HTML_BETAMAINSCREEN);
			}else{
				CallHTML(SET_HTML_MAINSCREEN);
			}
			break;
	}
}
