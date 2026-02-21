#define PHASE_ONESTRIP_PRE_INIT 0
#define PHASE_ONESTRIP_POST_INIT 1
#define PHASE_ONESTRIP_ENABLE_ACCEPT 2
#define PHASE_ONESTRIP_READ 3
#define PHASE_ONESTRIP_ACCEPT 4
#define PHASE_ONESTRIP_REPORT 5
#define PHASE_ONESTRIP_FINISH 6
#define PHASE_ONESTRIP_PRINT  7

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "printer.h"
#include "chambfac.h"
#include "nuc.h"

typedef struct qcmeas QCMEAS;
struct qcmeas{
	float meas;
	float result;
	char actstr[12];
};
extern QCMEAS qcmeas[6];
extern CURRENT  current;
extern int m_iPhase;
extern unsigned char m_ucClear;
extern MEASUREMENT measurement[];
extern CHAMBER chamber[];

void send_to_amulet_string(uchar ucIndex, char message0[]);
void trim(char *acByte);
unsigned char PopPageStack(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void Amulet_printOneStrip(void);

void AmuletOneStrip_menu(void) {
	short ch_num = current.main_chamber;
	char acMsg[100];
	char acMsg2[100];
	float act1, sum;
	//char kun;
	static short nmeas;
	static short nuc_index;
	static NUCDATA nucdata;
	static bool printer_pressed;
	static char message179[26];
	static char message180[26];
	static char message181[26];
	static char message182[26];
	static char message184[52];
	static char message186[52];


	//unsigned long int wait;
	//bool foundPET;
	//short index;

	switch(m_iPhase) {
		case PHASE_ONESTRIP_PRE_INIT:

			SetAmuletByte(92,current.language);

			if(current.num_chambers>1){
				if(chamber_type(0) == R_CHAMB && chamber_type(1) == R_CHAMB) SetAmuletByte(21, 0xFF);
			}

			if(current.num_chambers>0){
				sprintf(acMsg, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(acMsg, ", R");
						break;
					case P_CHAMB:
						strcat(acMsg, ", PET");
						break;
					case B_CHAMB:
						strcat(acMsg, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(acMsg, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(acMsg, ", HR");
						break;
					case K_CHAMB:
						strcat(acMsg, ", 1K");
						break;
				}
				send_to_amulet_string(9, acMsg);
				SetAmuletByte(20, 0xFF);

				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				nuc_index = NuclideData_getIndexFromName("Tc99m");
				set_nuclide_data(nuc_index, ch_num);
				NuclideData_getNuclide(nuc_index, &nucdata);
				//SetAmuletString(178, "QC - One Strip Test");
				send_amulet_message(L_QC_ONE_STRIP_TEST,178);    // "QC - One Strip Test"
				SetAmuletByte(177, 0xFF);

				if(m_ucClear == 31){
					m_ucClear = 0;
					printer_pressed = FALSE;
					nmeas = 1;
					m_iPhase = PHASE_ONESTRIP_POST_INIT;
				}else{
					if(printer_pressed){
						printer_pressed = FALSE;
						send_to_amulet_string(179, message179);
						delayloop(2);
						send_to_amulet_string(180, message180);
						delayloop(2);
						send_to_amulet_string(181, message181);
						delayloop(2);
						send_to_amulet_string(182, message182);
						delayloop(2);
						send_to_amulet_string(184, message184);
						delayloop(2);
						send_to_amulet_string(186, message186);
						delayloop(2);
						SetAmuletByte(100, 0xFF);
						SetAmuletByte(182, 0xFF);
						SetAmuletByte(183, 0xFF);
						SetAmuletByte(184, 0xFF);
						SetAmuletByte(185, 0xFF);

						//if(current.printer != NONE_PRINTER)
						if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
							SetAmuletByte(192, 0xFF);
						m_iPhase = PHASE_ONESTRIP_FINISH;
					}else{
						nmeas = 1;
						m_iPhase = PHASE_ONESTRIP_POST_INIT;
					}
				}
			}
			break;

		case PHASE_ONESTRIP_POST_INIT:
			// Wait for reappear table
			break;

		case PHASE_ONESTRIP_ENABLE_ACCEPT:
			if(current.num_chambers>0){
				switch(nmeas){
					case 1:
						//strcpy(acMsg, "1) Top of strip");
						get_amulet_message(L_TOP_OF_STRIP,acMsg);    // "1) Top of strip"
						strcpy(message179, acMsg);
						break;

					case 2:
						//strcpy(acMsg, "2) Bottom of strip");
						get_amulet_message(L_BOTTOM_OF_STRIP,acMsg);    // "2) Bottom of strip"
						strcpy(message181, acMsg);
						break;
				}


				send_to_amulet_string(2 * (nmeas - 1)+ 179, acMsg);
				SetAmuletWord(252, 250);
				//SetAmuletWord(253, 51 * (nmeas - 1)+ 41);
				SetAmuletWord(253, 51 * (nmeas - 1)+ 70);
				SetAmuletWord(254, 600);
				//SetAmuletWord(255, 51 * (nmeas - 1)+ 41);
				SetAmuletWord(255, 51 * (nmeas - 1)+ 70);

				SetAmuletByte(180, 0xFF);
				send_amulet_message(L_ACCEPT_BUTTON,100);    // "Accept"
				SetAmuletByte(178, 0xFF);
				m_iPhase = PHASE_ONESTRIP_READ;
			}
			break;

		case PHASE_ONESTRIP_READ:
			if(current.num_chambers>0){
				if (measurement[ch_num].display_flag_2) {
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						// Changed:
						//act = measurement[ch_num].act;
						//kun = measurement[ch_num].kun;
						//act1 = act / unitfact[kun - 1];
						act1 = measurement[ch_num].act0;
						qcmeas[nmeas - 1].meas = act1;
						// Locked: CI
						//format_activity(qcmeas[nmeas - 1].meas, current.system, qcmeas[nmeas - 1].actstr);
						//format_activity(qcmeas[nmeas - 1].meas, CI, qcmeas[nmeas - 1].actstr);
						format_activity_system(qcmeas[nmeas - 1].meas, qcmeas[nmeas - 1].actstr);
						strcpy(acMsg, qcmeas[nmeas - 1].actstr);
						trim(acMsg);
						strcpy(acMsg2, chamber[ch_num].nucdata.name);
						trim(acMsg2);
						strcat(acMsg2, " ");
						strcat(acMsg2, acMsg);
						send_to_amulet_string(177, acMsg2);
					}else{
						//SetAmuletString(177, "OVER RANGE");
						send_amulet_message(L_OVERRANGE,177);    // "OVER RANGE"
						qcmeas[nmeas - 1].meas = 100.0;
					}
				}
			}
			break;

		case PHASE_ONESTRIP_ACCEPT:
			if(current.num_chambers>0){
				if(qcmeas[nmeas - 1].meas == 100.0){
					PopPageStack();
					//Amulet_DisplayError("One Strip", "OVER RANGE\nERROR", TRUE);
					get_amulet_message(L_ONE_STRIP,acMsg);    // "One Strip"
					get_amulet_message(L_OVERRANGE_ERROR,acMsg2);    // "OVER RANGE\nERROR"
					Amulet_DisplayError(acMsg,acMsg2, TRUE);
					return;
				}else{
					send_to_amulet_string(2 * (nmeas - 1) + 180, qcmeas[nmeas - 1].actstr);
					switch(nmeas){
						case 1:
							strcpy(message180, qcmeas[nmeas - 1].actstr);
							break;
						case 2:
							strcpy(message182, qcmeas[nmeas - 1].actstr);
							break;
					}
					SetAmuletByte((nmeas - 1) + 182, 0xFF);
					nmeas++;
					if(nmeas < 3){
						m_iPhase = PHASE_ONESTRIP_ENABLE_ACCEPT;
					}else{
						m_iPhase = PHASE_ONESTRIP_REPORT;
					}
				}
			}
			break;

		case PHASE_ONESTRIP_REPORT:
			if(current.num_chambers>0){
				sum = qcmeas[0].meas + qcmeas[1].meas;
				qcmeas[0].result = 100.0 * qcmeas[0].meas / sum;
				qcmeas[1].result = 100.0 * qcmeas[1].meas / sum;

				//sprintf(acMsg, "    Top/(Top+Bottom) = %5.1f", qcmeas[0].result);
				get_amulet_message(L_TOP_TOP_BOTTOM_EQUALS,acMsg2);    // "    Top/(Top+Bottom) = %5.1f"
				sprintf(acMsg,acMsg2, qcmeas[0].result);
				strcat(acMsg, "%");
				strcpy(message184, acMsg);
				send_to_amulet_string(184, acMsg);
				SetAmuletByte(184, 0xFF);

				//sprintf(acMsg, "Bottom/(Top+Bottom) = %5.1f", qcmeas[1].result);
				get_amulet_message(L_BOTTOM_TOP_BOTTOM_EQUALS,acMsg2);    // "Bottom/(Top+Bottom) = %5.1f"
				sprintf(acMsg,acMsg2, qcmeas[1].result);
				strcat(acMsg, "%");
				strcpy(message186, acMsg);
				send_to_amulet_string(186, acMsg);
				SetAmuletByte(185, 0xFF);

				//if(current.printer != NONE_PRINTER)
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(192, 0xFF);

				m_iPhase = PHASE_ONESTRIP_FINISH;
			}
			break;

		case PHASE_ONESTRIP_FINISH:
			printer_pressed = FALSE;
			break;

		case PHASE_ONESTRIP_PRINT:
			m_iPhase = PHASE_ONESTRIP_FINISH;
			beep_amulet();
			printer_pressed = TRUE;
			Amulet_printOneStrip();
			SetAmuletByte(193, 0xFF);
			break;

		default:
			break;
	}
}
