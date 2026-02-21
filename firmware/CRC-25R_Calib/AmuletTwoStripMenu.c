#define PHASE_TWOSTRIP_PRE_INIT 0
#define PHASE_TWOSTRIP_POST_INIT 1
#define PHASE_TWOSTRIP_ENABLE_ACCEPT 2
#define PHASE_TWOSTRIP_READ 3
#define PHASE_TWOSTRIP_ACCEPT 4
#define PHASE_TWOSTRIP_REPORT 5
#define PHASE_TWOSTRIP_FINISH 6
#define PHASE_TWOSTRIP_PRINT  7

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "nuc.h"
#include "printer.h"

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
void Amulet_printTwoStrip(void);

void AmuletTwoStrip_menu(void) {
	short ch_num = current.main_chamber;
	char acMsg[100];
	char acMsg2[100];
	float act1, sum;
	//char kun;
	static short nmeas;
	static short nuc_index;
	static NUCDATA nucdata;
	static bool printer_pressed;
	static char message169[25];
	static char message172[25];
	static char message175[25];
	static char message178[25];
	static char message170[51];
	static char message173[51];
	static char message176[51];
	static char message179[51];
	static char message182[51];
	static char message185[51];
	static char message188[51];
	//unsigned long int wait;
	//bool foundPET;
	//short index;

	switch(m_iPhase) {
		case PHASE_TWOSTRIP_PRE_INIT:

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
				//SetAmuletString(168, "QC - Two Strip Test");
				send_amulet_message(L_QC_TWO_STRIP_TEST,168);    // "QC - Two Strip Test"
				SetAmuletByte(167, 0xFF);

				if(m_ucClear == 32){
					m_ucClear = 0;
					printer_pressed = FALSE;
					nmeas = 1;
					m_iPhase = PHASE_TWOSTRIP_POST_INIT;
				}else{
					if(printer_pressed){
						printer_pressed = FALSE;
						send_to_amulet_string(169, message169);
						delayloop(2);
						send_to_amulet_string(172, message172);
						delayloop(2);
						send_to_amulet_string(175, message175);
						delayloop(2);
						send_to_amulet_string(178, message178);
						delayloop(2);
						send_to_amulet_string(170, message170);
						delayloop(2);
						send_to_amulet_string(173, message173);
						delayloop(2);
						send_to_amulet_string(176, message176);
						delayloop(2);
						send_to_amulet_string(179, message179);
						delayloop(2);
						send_to_amulet_string(182, message182);
						delayloop(2);
						send_to_amulet_string(185, message185);
						delayloop(2);
						send_to_amulet_string(188, message188);
						SetAmuletByte(100, 0xFF);
						SetAmuletByte(172, 0xFF);
						SetAmuletByte(173, 0xFF);
						SetAmuletByte(174, 0xFF);
						SetAmuletByte(175, 0xFF);
						SetAmuletByte(176, 0xFF);
						SetAmuletByte(177, 0xFF);
						SetAmuletByte(178, 0xFF);
						//if(current.printer != NONE_PRINTER)
						if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
							SetAmuletByte(182, 0xFF);
						m_iPhase = PHASE_TWOSTRIP_FINISH;
					}else{
						nmeas = 1;
						m_iPhase = PHASE_TWOSTRIP_POST_INIT;
					}
				}
			}
			break;

		case PHASE_TWOSTRIP_POST_INIT:
			// Wait for reappear table
			break;

		case PHASE_TWOSTRIP_ENABLE_ACCEPT:
			if(current.num_chambers>0){
				switch(nmeas){
					case 1:
						//strcpy(acMsg, "1) Top of strip A");
						get_amulet_message(L_TOP_OF_STRIP_A,acMsg);    // "1) Top of strip A"
						strcpy(message169, acMsg);
						break;

					case 2:
						//strcpy(acMsg, "2) Bottom of strip A");
						get_amulet_message(L_BOTTOM_OF_STRIP_A,acMsg);    // "2) Bottom of strip A"
						strcpy(message172, acMsg);
						break;

					case 3:
						//strcpy(acMsg, "3) Top of strip B",acMsg);
						get_amulet_message(L_TOP_OF_STRIP_B,acMsg);    // "3) Top of strip B"
						strcpy(message175, acMsg);
						break;

					case 4:
						//strcpy(acMsg, "4) Bottom of Strip B");
						get_amulet_message(L_BOTTOM_OF_STRIP_B,acMsg);    // "4) Bottom of strip B"
						strcpy(message178, acMsg);
						break;
				}

				send_to_amulet_string(3 * (nmeas - 1)+ 169, acMsg);
				SetAmuletWord(252, 250);
				//SetAmuletWord(253, 51 * (nmeas - 1) + 41);
				SetAmuletWord(253, 51 * (nmeas - 1) + 70);
				SetAmuletWord(254, 600);
				//SetAmuletWord(255, 51 * (nmeas - 1) + 41);
				SetAmuletWord(255, 51 * (nmeas - 1) + 70);

				send_amulet_message(L_ACCEPT_BUTTON,100);    // "Accept"

				SetAmuletByte(170, 0xFF);
				SetAmuletByte(168, 0xFF);
				m_iPhase = PHASE_TWOSTRIP_READ;
			}
			break;

		case PHASE_TWOSTRIP_READ:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						//Changed:
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
						send_to_amulet_string(167, acMsg2);
					}else{
						//SetAmuletString(167, "OVER RANGE");
						send_amulet_message(L_OVERRANGE,167);    // "OVER RANGE"
						qcmeas[nmeas - 1].meas = 100.0;
					}
				}
			}
			break;

		case PHASE_TWOSTRIP_ACCEPT:
			if(current.num_chambers>0){
				if(qcmeas[nmeas - 1].meas == 100.0){
					PopPageStack();
					//Amulet_DisplayError("Two Strip", "OVER RANGE\nERROR", TRUE);
					get_amulet_message(L_TWO_STRIP,acMsg);    // "Two Strip"
					get_amulet_message(L_OVERRANGE_ERROR,acMsg2);    // "OVER RANGE\nERROR"
					Amulet_DisplayError(acMsg,acMsg2, TRUE);
					return;
				}else{
					send_to_amulet_string(3 * (nmeas - 1) + 170, qcmeas[nmeas - 1].actstr);
					switch(nmeas){
						case 1:
							strcpy(message170, qcmeas[nmeas-1].actstr);
							break;

						case 2:
							strcpy(message173, qcmeas[nmeas-1].actstr);
							break;

						case 3:
							strcpy(message176, qcmeas[nmeas-1].actstr);
							break;

						case 4:
							strcpy(message179, qcmeas[nmeas-1].actstr);
							break;
					}
					SetAmuletByte((nmeas - 1) + 172, 0xFF);
					nmeas++;
					if(nmeas <= 4){
						m_iPhase = PHASE_TWOSTRIP_ENABLE_ACCEPT;
					}else{
						m_iPhase = PHASE_TWOSTRIP_REPORT;
					}
				}
			}
			break;

		case PHASE_TWOSTRIP_REPORT:
			if(current.num_chambers>0){
				sum = qcmeas[0].meas + qcmeas[1].meas;
				qcmeas[0].result = 100. * qcmeas[0].meas / sum;
				sum = qcmeas[2].meas + qcmeas[3].meas;
				qcmeas[1].result = 100. * qcmeas[3].meas / sum;
				qcmeas[2].result = 100. - (qcmeas[0].result + qcmeas[1].result);

				//sprintf(acMsg, "Free Tc99m : %5.1f", qcmeas[0].result);
				get_amulet_message(L_FREE,acMsg2);    // "Free Tc99m : %5.1f"
				sprintf(acMsg,acMsg2, qcmeas[0].result);
				strcat(acMsg, "%");
				strcpy(message182, acMsg);
				send_to_amulet_string(182, acMsg);
				SetAmuletByte(176, 0xFF);

				//sprintf(acMsg, "Reduced/Hydrolized: %5.1f", qcmeas[1].result);
				get_amulet_message(L_REDUCED_HYDROLIZED,acMsg2);    // "Reduced/Hydrolized: %5.1f"
				sprintf(acMsg,acMsg2, qcmeas[1].result);
				strcat(acMsg, "%");
				strcpy(message185, acMsg);
				send_to_amulet_string(185, acMsg);
				SetAmuletByte(177, 0xFF);

				//sprintf(acMsg, "Purity : %5.1f", qcmeas[2].result);
				get_amulet_message(L_PURITY,acMsg2);    // "Purity: %5.1f"
				sprintf(acMsg, acMsg2, qcmeas[2].result);
				strcat(acMsg, "%");
				strcpy(message188, acMsg);
				send_to_amulet_string(188, acMsg);
				SetAmuletByte(178, 0xFF);

				//if(current.printer != NONE_PRINTER)
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(182, 0xFF);
				m_iPhase = PHASE_TWOSTRIP_FINISH;
			}
			break;


		case PHASE_TWOSTRIP_FINISH:
			printer_pressed = FALSE;
			break;

		case PHASE_TWOSTRIP_PRINT:
			m_iPhase = PHASE_TWOSTRIP_FINISH;
			beep_amulet();
			printer_pressed = TRUE;
			Amulet_printTwoStrip();
			SetAmuletByte(183, 0xFF);
			break;

		default:
			break;
	}
}
