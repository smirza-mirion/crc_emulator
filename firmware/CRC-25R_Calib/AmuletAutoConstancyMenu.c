/**
 * \file
 * \details This file handles calls from the Amulet AutoConstancy Test Screen
 */
#define PHASE_AUTOCONSTANCY_PRE_INIT	0
#define PHASE_AUTOCONSTANCY_WAIT		1
#define PHASE_AUTOCONSTANCY_PRINT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "i2c.h"
#include "nuc.h"
#include "printer.h"
#include "message.h"

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void GetShortNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
short prdailysec(bool okdata,short acc_ret);
short Amulet_printAccuracyAndConstancy(bool printconstancy);

extern int m_iPhase;
extern unsigned char m_ucClear;
extern REMOTE remote[9];
extern CURRENT  current;
extern MEASUREMENT measurement[];
extern short test_num;
typedef struct cntest_data CNTESTDATA;
struct cntest_data{
	short nuc_index;
	char strng[10];
	short kun;
};
extern CNTESTDATA test_data[20];
extern bool AmuletDailyMenu_okdata;
extern bool AmuletAccuracy_daily;

/**
 * \details Handles the Amulet Screen AutoConstancy.htm. AutoConstancy.htm displays the results of the autoconstancy test
 * \param Amulet_Byte_ID Description
 * \param 90 Language (STATE) English = 0, French = 1
 * \param 100 0xFF = Display Report
 * \param 101 0xFF = Display Line 1
 * \param 102 0xFF = Display Line 2
 * \param 103 0xFF = Display Line 3
 * \param 104 0xFF = Display Line 4
 * \param 105 0xFF = Display Line 5
 * \param 106 0xFF = Display Line 6
 * \param 107 0xFF = Display Line 7
 * \param 108 0xFF = Display Line 8
 * \param 109 0xFF = Display Line 9
 * \param 110 0xFF = Display Line 10
 * \param 111 0xFF = Display Line 11
 * \param 112 0xFF = Display Line 12
 * \param Amulet_String_ID Description
 * \param 90 Title
 * \param 100_101 Line 1
 * \param 102_103 Line 2
 * \param 104_105 Line 3
 * \param 106_107 Line 4
 * \param 108_109 Line 5
 * \param 110_111 Line 6
 * \param 112_113 Line 7
 * \param 114_115 Line 8
 * \param 116_117 Line 9
 * \param 118_119 Line 10
 * \param 120_121 Line 11
 * \param 122_123 Line 12
 * \returns None
 */
void AmuletAutoConstancy_menu(void){
	short ch_num = current.main_chamber;
	short ch_type;
	char message[52], nuclidestring[52], actstr[10], actamulet[10];
	short index, jndex, kndex;
	short constch[12], linecount;
	NUCDATA dummy;
	static char line[12][52];
	short iret;

	switch(m_iPhase){
		case PHASE_AUTOCONSTANCY_PRE_INIT:
			SetAmuletByte(90, current.language);
			send_amulet_message(L_AUTOCONSTANCY, 90);    // "AutoConstancy"

			if(m_ucClear==29){
				test_num = 0;
				remote[8].exists = FALSE;
				ch_type = chamber_type(ch_num);
				EE_READ(constancych, (uchar *) &constch);
				linecount = 0;
				for(index=0; index<12; index++){
					if(constch[index]>=0){
						if(NuclideData_getEffectiveResponse(constch[index], ch_type)!=0.0){
							set_nuclide_data(constch[index], 8);
							display_activity(8);
							strcpy(actstr, &(measurement[8].actstr[0]));
							test_data[linecount].nuc_index = constch[index];
							strncpy(test_data[linecount].strng, measurement[8].actstr, 6);
							test_data[linecount].kun = measurement[8].kun;
							kndex = 0;
							for(jndex=0; jndex<10; jndex++) actamulet[jndex] = 0;
							for(jndex=0; jndex<10; jndex++){
								if(actstr[jndex]!=0x20){
									actamulet[kndex] = actstr[jndex];
									kndex++;
								}
							}
							sprintf(message, "%d) ", linecount+1);
							GetShortNuclideInfo(constch[index], &dummy, nuclidestring);
							strcat(message, nuclidestring);
							strcat(message, "    ");
							strcat(message, actamulet);
							strcpy(&(line[linecount][0]), message);
							send_to_amulet_string((2*linecount)+100, message);
							linecount++;
						}
					}
				}
				if(linecount!=0){
					test_num = linecount - 1;
					SetAmuletByte(100, 0xFF);
					for(index=0; index<linecount; index++){
						SetAmuletByte(101+index, 0xFF);
					}
					//if(current.printer!=NONE_PRINTER) SetAmuletByte(113, 0xFF);
					if((current.printer!=NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
						SetAmuletByte(113, 0xFF);
				}
				m_ucClear = 0;
			}else{
				for(index=0; index<=test_num; index++){
					send_to_amulet_string((2*index)+100, &(line[index][0]));
					SetAmuletByte(101+index, 0xFF);
				}
				//if(current.printer!=NONE_PRINTER) SetAmuletByte(113, 0xFF);
				if((current.printer!=NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(113, 0xFF);
			}
			m_iPhase = PHASE_AUTOCONSTANCY_WAIT;
			break;

		case PHASE_AUTOCONSTANCY_WAIT:
			break;

		case PHASE_AUTOCONSTANCY_PRINT:
			beep_amulet();
			if(AmuletAccuracy_daily){
				iret = prdailysec(AmuletDailyMenu_okdata, 1);
			}else{
				iret = Amulet_printAccuracyAndConstancy(TRUE);
			}
			if(iret==1) dosig(current.printer);
			SetAmuletByte(113,0xFF);
			m_iPhase = PHASE_AUTOCONSTANCY_WAIT;
			break;
	}
}
