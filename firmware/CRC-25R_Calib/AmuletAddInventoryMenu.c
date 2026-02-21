#define PHASE_ADD_INVENTORY_PRE_INIT 0
#define PHASE_ADD_INVENTORY_WAIT 1
#define PHASE_ADD_INVENTORY_SUBMIT 2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "nuc.h"

extern CURRENT  current;
extern int m_iPhase;
extern unsigned char m_ucClear;
extern unsigned char m_ucAddInventory_ID;
extern char m_acAddInventory_Lot[17];
extern float m_fAddInventory_Volume;
extern unsigned char m_ucAddInventory_Nuclide;
extern float m_fAddInventory_Activity;
extern time_t m_dtmAddInventory_Date;
extern unsigned char m_ucAddInventory_MeasuredNuclide;
extern short m_iAddInventory_Study;
extern float m_fAddInventory_MoPerTc;
extern INVENTORY inventory[MAX_INVENTORY];

void send_to_amulet_string(uchar ucIndex, char message0[]);
void trim(char *acByte);
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void GetExtendedTimeInfoLanguage(time_t *dtmDateTime, char *acMsg);
void get_study_type(short istudy, char *study);
void SetAmuletBackHTML(void);
void write_Inventory(void);

void AmuletAddInventory_menu(void) {
	char acMsg[50];
	NUCDATA nuc;
//	char halflife[50];
	float fTemp;
	short i;
	char str[20];


	switch(m_iPhase) {
		case PHASE_ADD_INVENTORY_PRE_INIT:
			if (m_ucClear == 1) {
				m_ucAddInventory_Nuclide = 0xFF;
				m_ucAddInventory_ID = 0;
				m_acAddInventory_Lot[0] = 0;
				m_fAddInventory_Volume = -999;
				m_fAddInventory_Activity = -999;
				m_ucAddInventory_MeasuredNuclide = 0xFF;
				m_dtmAddInventory_Date = 0;
				m_iAddInventory_Study = -1;
				m_ucClear = 0;
				m_fAddInventory_MoPerTc = 0;
			}

			SetAmuletByte(92,current.language);

			if(current.num_chambers>1)  SetAmuletByte(21, 0xFF);

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
			}

			if (m_ucAddInventory_Nuclide == 0xFF) {
				send_to_amulet_string(99, "");
				SetAmuletByte(200, 0xFF);
			}
			else {
				if(m_ucAddInventory_MeasuredNuclide != 0xFF) {
					if(m_ucAddInventory_Nuclide != m_ucAddInventory_MeasuredNuclide){
						m_fAddInventory_Activity = -999;
						m_dtmAddInventory_Date = 0;
						m_ucAddInventory_MeasuredNuclide = 0xFF;
					}
				}

				GetExtendedNuclideInfo(m_ucAddInventory_Nuclide, &nuc, acMsg);
				send_to_amulet_string(99, acMsg);
				send_amulet_message(L_MEASURE,214);    // "Measure"

				SetAmuletByte(208, 0xFF);
			}

			if (m_ucAddInventory_ID == 0) {
				send_to_amulet_string(97, "");
			}
			else {
				sprintf(acMsg, "%d", m_ucAddInventory_ID);
				send_to_amulet_string(97, acMsg);
			}

			//SetAmuletString(96, m_acAddInventory_Lot);
			send_to_amulet_string(96, m_acAddInventory_Lot);

			if (m_fAddInventory_Volume == -999) {
				send_to_amulet_string(95, "");
				SetAmuletByte(201, 0xFF);
			}
			else {
				sprintf(acMsg, "%.3f ml", m_fAddInventory_Volume);
				send_to_amulet_string(95, acMsg);
			}

			if (m_fAddInventory_Activity == -999) {
				send_to_amulet_string(94, "");
				SetAmuletByte(202, 0xFF);
			}
			else {
				if (m_fAddInventory_Activity < 1.e-8) {
					if(current.system == CI){
						fTemp = m_fAddInventory_Activity;
						fTemp *= 1.e+6;
						sprintf(acMsg,"%.3f uCi", fTemp);
					}else{
						format_activity_system(m_fAddInventory_Activity, acMsg);
					}
				}
				else {
					// Locked: CI
					//format_activity(m_fAddInventory_Activity, current.system, acMsg);
					format_activity_system(m_fAddInventory_Activity, acMsg);
					trim(acMsg);
				}
				send_to_amulet_string(94, acMsg);
			}

			if (m_dtmAddInventory_Date == 0) {
				send_to_amulet_string(93, "");
				SetAmuletByte(203, 0xFF);
			}
			else {
				GetExtendedTimeInfoLanguage(&m_dtmAddInventory_Date, acMsg);
				send_to_amulet_string(93, acMsg);
			}

			send_amulet_message(L_ADD_INVENTORY_ITEM,200);    // "Add Inventory Item"
			send_amulet_message(L_NUCLIDE_COLON,202);    // "Nuclide:"
			send_amulet_message(L_LOT_COLON,203);    // "Lot:"
			send_amulet_message(L_VOLUME,204);    // "Volume:"
			send_amulet_message(L_ACTIVITY_COLON,205);    // "Activity:"
			send_amulet_message(L_DATE_COLON,206);    // "Date:"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE,227);    // "Please Select Nuclide"
			send_amulet_message(L_PLEASE_ENTER_ID,208);    // "Please Enter ID:"
			send_amulet_message(L_PLEASE_ENTER_LOT,210);    // "Please Enter Lot:"
			send_amulet_message(L_PLEASE_ENTER_MEASUREMENT_TIME,212);    // "Please Enter Measurment Time:"
			send_amulet_message(L_STUDY_LABEL,215);    // "STUDY:"
			send_amulet_message(L_SUBMIT,216);    // "Submit"
			send_amulet_message(L_PLEASE_ACCEPT_ACTIVITY,217);    // "Please Accept Activity:"
			send_amulet_message(L_PLEASE_ENTER_MOTC_RATIO,219);    // "Please Enter Mo/Tc Ratio:"
			send_amulet_message(L_PLEASE_ENTER_ACTIVITY,221);    // "Please Enter Activity:"
			send_amulet_message(L_PLEASE_ENTER_VOLUME,223);    // "Please Enter Volume(ml):"

			SetAmuletByte(205, 0xFF);

			if(m_ucAddInventory_Nuclide == NuclideData_getIndexFromName("Tc99m")) {
				if(m_iAddInventory_Study == -1) {
					acMsg[0] = 0;
				}
				else {
					//acMsg[5] = 0;
					//strncpy(acMsg, study_type[m_iAddInventory_Study], 5);
					get_study_type(m_iAddInventory_Study,str);
					strcpy(acMsg, str);

				}
				//SetAmuletString(91, acMsg);
				send_to_amulet_string(91, acMsg);
				SetAmuletByte(206, 0xFF);

				if(m_fAddInventory_MoPerTc == 0) {
					acMsg[0] = 0;
				}
				else {
					sprintf(acMsg, "%.3f", m_fAddInventory_MoPerTc);
				}

				send_to_amulet_string(90, acMsg);
				SetAmuletByte(207, 0xFF);
			}
			else {
				m_iAddInventory_Study = -1;
				m_fAddInventory_MoPerTc = 0;
			}

			if((m_ucAddInventory_Nuclide!=0xFF) && (m_fAddInventory_Volume!=-999) && (m_fAddInventory_Activity!=-999) && (m_dtmAddInventory_Date!=0)){
				SetAmuletByte(209, 0xFF);
			}

			m_iPhase = PHASE_ADD_INVENTORY_WAIT;
			break;

		case PHASE_ADD_INVENTORY_WAIT:
			break;

		case PHASE_ADD_INVENTORY_SUBMIT:
			for(i=0; i<MAX_INVENTORY; i++){
				if(inventory[i].nucnum == -1) {
					inventory[i].act = m_fAddInventory_Activity;
					inventory[i].vol = m_fAddInventory_Volume;
					inventory[i].mopertc = m_fAddInventory_MoPerTc;
					inventory[i].nucnum = m_ucAddInventory_Nuclide;
					inventory[i].type = m_iAddInventory_Study;
					inventory[i].id = m_ucAddInventory_ID;
					// Locked: CI
					//EE_READ(syst, &current.system);
					//inventory[i].syst = current.system;
					// Removed:
					//inventory[i].syst = CI;
					strcpy(inventory[i].lot, m_acAddInventory_Lot);
					inventory[i].date = m_dtmAddInventory_Date;
					break;
				}
			}
			// Save SD
			write_Inventory();
			SetAmuletBackHTML();
			break;
	}
}
