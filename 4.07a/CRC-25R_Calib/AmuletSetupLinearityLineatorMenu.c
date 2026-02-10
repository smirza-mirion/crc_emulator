/**
 * \file
 * \details This file handles calls from the Amulet Lineator Linearity Setup Screen
 */
#define PHASE_SETUP_LINEARITY_LINEATOR_PRE_INIT	0
#define PHASE_SETUP_LINEARITY_LINEATOR_WAIT		1
#define PHASE_SETUP_LINEARITY_LINEATOR_SAVE		2
#define PHASE_SETUP_LINEARITY_LINEATOR_PRINT	3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "linearity.h"
#include "i2c.h"
#include "nuc.h"
#include "amulet.h"
#include "printer.h"
#include "chambfac.h"
#include "message.h"

static LINDEF linearityMirror;
ACTIVITY_MEASUREMENT AmuletSetupLinearityLineatorMenu_measurement[8];
short AmuletSetupLinearityLineatorMenu_nuclideIndex;
char AmuletSetupLinearityLineatorMenu_lineatorSerial[11];

extern CURRENT current;
extern int m_iPhase;
extern unsigned char m_ucClear;
extern CHAMBERVALS chamb_vals[];

typedef struct calmeas CALMEAS;
struct calmeas
{
    float meas_act;
    char act_str[10];
};


void trim_and_shrink(char *acByte);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void SetAmuletBackHTML(void);
void Amulet_printCalib(short linearity_type, CALMEAS *calmeas, LINDEF *lin_def, bool finished);
void send_accept_cancel(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen SetupLinearityLineator.htm. SetupLinearityLineator.htm setups the lineator linearity test.
 * \param Amulet_Byte_ID Description
 * \param 100 Refresh Display (0xFF)
 * \param 101 Display Factors (0xFF)
 * \param Amulet_String_ID Description
 * \param 101 Tube 1 Activity
 * \param 102 Tube 2 Activity
 * \param 103 Tube 3 Activity
 * \param 104 Tube 4 Activity
 * \param 105 Tube 5 Activity
 * \param 106 Tube 6 Activity
 * \param 107 Tube 7 Activity
 * \param 108 Tube 8 Activity
 * \param 109_110 Factor 1
 * \param 111_112 Factor 2
 * \param 113_114 Factor 3
 * \param 115_116 Factor 4
 * \param 117_118 Factor 5
 * \param 119_120 Factor 6
 * \param 121_122 Factor 7
 * \param 123_124 Factor 8
 * \param 125_126 Chamber Serial Number
 * \param 127 Lineator Serial Number
 * \param 129_130 Title
 * \param 131 Tube # 1
 * \param 132 Tube # 2
 * \param 133 Tube # 3
 * \param 134 Tube # 4
 * \param 135 Tube # 5
 * \param 136 Tube # 6
 * \param 137 Tube # 7
 * \param 138 Tube # 8
 * \param 139 Print
 * \param 140 Delete Current Settings
 * \param 141_142 Please enter Lineator Serial #
 * \param 143 Lineator Serial
 * \returns None
 */
void AmuletSetupLinearityLineator_menu(void){
	short index;
	char message[52];
	static bool finished;
	float correctedactivity;
	short ch_num = current.main_chamber;
	static char prev_chamb_num[7];
	static char cur_chamb_num[7];
	static float prev_factors[8];
	static float cur_factors[8];
	CALMEAS calmeasure[12];
	bool show_print;
	char *ptr;
	char fmt_str[30];
	char err_message[104];


	switch(m_iPhase){
		case PHASE_SETUP_LINEARITY_LINEATOR_PRE_INIT:
			show_print = FALSE;
			if(m_ucClear == 13){
				EE_READ(lindef, (uchar *) &linearityMirror);
				for(index=0; index<8; index++){
					AmuletSetupLinearityLineatorMenu_measurement[index].activity = -999;
					AmuletSetupLinearityLineatorMenu_measurement[index].timestamp = 0;
					AmuletSetupLinearityLineatorMenu_measurement[index].nuclideid = -1;
				}
				AmuletSetupLinearityLineatorMenu_nuclideIndex = NuclideData_getIndexFromName("Tc99m");

				if(linearityMirror.num_Lin[ch_num] < 0){
					AmuletSetupLinearityLineatorMenu_lineatorSerial[0] = 0;
					prev_chamb_num[0] = 0;
					for(index=0; index<8; index++) prev_factors[index] = -999;
				}else{
					strcpy(AmuletSetupLinearityLineatorMenu_lineatorSerial, &linearityMirror.serial_num_Lin[ch_num][0]);
					strcpy(prev_chamb_num, &linearityMirror.chamb_num_Lin[ch_num][0]);
					for(index=0; index<8; index++) prev_factors[index] = linearityMirror.factors_Lin[ch_num][index];
				}

				for(index=0;index<6;index++) cur_chamb_num[index] = chamb_vals[ch_num].sn[index];
				cur_chamb_num[6] = 0;

				m_ucClear = 0;
				if(prev_chamb_num[0]!=0){
					if(strcmp(cur_chamb_num, prev_chamb_num)!=0){
						//Amulet_DisplayNotification("Setup Lineator", "The current factors are for a different chamber\nand can not be used for this chamber", TRUE);
						get_amulet_message(L_SETUP_LINEATOR,message);    // "Setup Lineator"
						get_amulet_message(L_DIFFERENT_CHAMBERS,err_message);    // "The current factors are for a different chamber\nand can not be used for this chamber"
						Amulet_DisplayNotification(message,err_message, TRUE);
						return;
					}
				}
			}

			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);

			if(current.num_chambers>0){
				sprintf(message, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(message, ", R");
						break;
					case P_CHAMB:
						strcat(message, ", PET");
						break;
					case B_CHAMB:
						strcat(message, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(message, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(message, ", HR");
						break;
					case K_CHAMB:
						strcat(message, ", 1K");
						break;
				}
				send_to_amulet_string(9, message);
				SetAmuletByte(20, 0xFF);
			}

			if(linearityMirror.num_Lin[ch_num] >= 0) {
				show_print = TRUE;
				SetAmuletByte(102, 0xFF);
			}

			if(prev_chamb_num[0]==0){
				//sprintf(message, "Chamber Serial: %s", cur_chamb_num);
				get_amulet_message(L_CHAMBER_SERIAL,fmt_str);    // "Chamber Serial: %s"
				sprintf(message, fmt_str, cur_chamb_num);
				send_to_amulet_string(125, message);
			}else{
				if(strcmp(cur_chamb_num, prev_chamb_num)==0){
					//sprintf(message, "Chamber Serial: %s", cur_chamb_num);
					get_amulet_message(L_CHAMBER_SERIAL,fmt_str);    // "Chamber Serial: %s"
					sprintf(message, fmt_str, cur_chamb_num);
				}else{
					//sprintf(message, "(Chamber Serial) Old: %s, New: %s", prev_chamb_num, cur_chamb_num);
					get_amulet_message(L_CHAMBER_SERIAL_OLD_NEW,fmt_str);    // "(Chamber Serial) Old:%s, New:%s"
					sprintf(message, fmt_str, prev_chamb_num, cur_chamb_num);
				}
				send_to_amulet_string(125, message);
			}

			if(AmuletSetupLinearityLineatorMenu_lineatorSerial[0]!=0){
				//SetAmuletString(127, AmuletSetupLinearityLineatorMenu_lineatorSerial);
				send_to_amulet_string(127, AmuletSetupLinearityLineatorMenu_lineatorSerial);
			}

			send_amulet_message(L_SETUP_LINEARITY_LINEATOR,129);    // "Setup Linearity Lineator"

			send_amulet_message(L_LINEATOR_SERIAL,143);    // "Lineator Serial:"

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #1 (1):");
			send_to_amulet_string(131,message);

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #2 (1 + 2):");
			send_to_amulet_string(132,message);

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #3 (1 + 3):");
			send_to_amulet_string(133,message);

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #4 (1 + 2,3):");
			send_to_amulet_string(134,message);

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #5 (1 + 4):");
			send_to_amulet_string(135,message);

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #6 (1 + 2,4):");
			send_to_amulet_string(136,message);

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #7 (1 + 3,4):");
			send_to_amulet_string(137,message);

			get_amulet_message(L_TUBE,message);    // "Tube"
			strcat(message," #8 (1 + 2,3,4):");
			send_to_amulet_string(138,message);

			send_amulet_message(L_PRINT,139);    // "Print"
			send_amulet_message(L_DELETE_CURRENT_SETTINGS,140);    // "Delete Current Settings"

			get_amulet_message(L_PLEASE_ENTER_LINEATOR_SN,message);    // "Please Enter Lineator Serial #"
			strcat(message,":");
			send_to_amulet_string(141,message);

			send_accept_cancel();

			finished = TRUE;
			for(index=0; index<8; index++){
				if(AmuletSetupLinearityLineatorMenu_measurement[index].timestamp == (time_t)0) finished = FALSE;
				else{
					format_activity_system(AmuletSetupLinearityLineatorMenu_measurement[index].activity, message);
					trim_and_shrink(message);
					send_to_amulet_string(101+index, message);
					delayloop(5);
				}
			}
			SetAmuletByte(100, 0xFF);
			delayloop(20);

			if(finished){
				if((AmuletSetupLinearityLineatorMenu_lineatorSerial[0] != 0)) show_print = TRUE;

				for(index=0; index<8; index++){
					correctedactivity = nucdecay(AmuletSetupLinearityLineatorMenu_measurement[index].activity,
										         AmuletSetupLinearityLineatorMenu_measurement[index].timestamp,
										         AmuletSetupLinearityLineatorMenu_measurement[0].timestamp,
										         NuclideData_getHalflife(AmuletSetupLinearityLineatorMenu_measurement[index].nuclideid),
										         NuclideData_getHalflifeUnit(AmuletSetupLinearityLineatorMenu_measurement[index].nuclideid));
					cur_factors[index] = AmuletSetupLinearityLineatorMenu_measurement[0].activity / correctedactivity;

					if(prev_factors[index] != -999){
						//sprintf(message, "Old: %.2f, New: %.2f", prev_factors[index], cur_factors[index]);
						get_amulet_message(L_FACTOR_OLD_NEW,fmt_str);    // "Old: %.2f, New: %.2f"
						sprintf(message, fmt_str, prev_factors[index], cur_factors[index]);
					}else{
						//sprintf(message, "New Factor: %.2f", cur_factors[index]);
						get_amulet_message(L_NEW_FACTOR,fmt_str);    // "New Factor: %.2f"
						sprintf(message, fmt_str, cur_factors[index]);
					}
					send_to_amulet_string(2*index+109, message);
					delayloop(5);
				}
			}else{
				for(index=0;index<8;index++){
					if(prev_factors[index] != -999){
						//sprintf(message, "Current Factor: %.2f", prev_factors[index]);
						get_amulet_message(L_CURRENT_FACTOR,fmt_str);    // "Current Factor: %.2f"
						sprintf(message, fmt_str, prev_factors[index]);
						//SetAmuletString(2*index+109, message);
						send_to_amulet_string(2*index+109, message);
						delayloop(5);
					}
				}
			}
			delayloop(20);
			//if(show_print && (current.printer != NONE_PRINTER)) SetAmuletByte(103, 0xFF);
			if(show_print && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(103, 0xFF);
			SetAmuletByte(101, 0xFF);
			delayloop(20);

			m_iPhase = PHASE_SETUP_LINEARITY_LINEATOR_WAIT;
			break;

		case PHASE_SETUP_LINEARITY_LINEATOR_WAIT:
			break;

		case PHASE_SETUP_LINEARITY_LINEATOR_SAVE:
			if(!finished){
				//Amulet_DisplayError("Setup Lineator Error", "Please fill in all measurements",TRUE);
				get_amulet_message(L_SETUP_LINEATOR_ERROR,message);    // "Setup Lineator Error"
				get_amulet_message(L_FILL_IN_ALL_MEASUREMENTS,err_message);    // "Please fill in all measurements"
				Amulet_DisplayError(message,err_message,TRUE);
				return;
			}else{
				if(AmuletSetupLinearityLineatorMenu_lineatorSerial[0] == 0){
					get_amulet_message(L_SETUP_LINEATOR_ERROR,message);    // "Setup Lineator Error"
					get_amulet_message(L_FILL_IN_LINEATOR_SERIAL_NUMBER,err_message);    // "Please fill in Lineator Serial Number"
					Amulet_DisplayError(message,err_message,TRUE);
					//Amulet_DisplayError("Setup Lineator Error", "Please fill in Lineator Serial Number",TRUE);
					return;
				}else{
					linearityMirror.num_Lin[ch_num] = 8;
					EE_WRITE(lindef.num_Lin[ch_num], (uchar *) &linearityMirror.num_Lin[ch_num]);
					EE_WRITE(lindef.nuc_index_Lin[ch_num], (uchar *) &AmuletSetupLinearityLineatorMenu_nuclideIndex);
					for(index=0;index<7;index++){
						EE_WRITE(lindef.chamb_num_Lin[ch_num][index], (uchar *) &cur_chamb_num[index]);
					}

					for(index=0;index<11;index++){
						EE_WRITE(lindef.serial_num_Lin[ch_num][index], (uchar *) &AmuletSetupLinearityLineatorMenu_lineatorSerial[index]);
					}

					for(index=0;index<8;index++){
						EE_WRITE(lindef.factors_Lin[ch_num][index], (uchar *) &cur_factors[index]);
					}
					SetAmuletBackHTML();
					return;
				}
			}
			//break;

		case PHASE_SETUP_LINEARITY_LINEATOR_PRINT:
			beep_amulet();
			EE_READ(lindef, (uchar *)&linearityMirror);
			if(finished){
				linearityMirror.num_Lin[ch_num] = 8;
				linearityMirror.nuc_index_Lin[ch_num] = AmuletSetupLinearityLineatorMenu_nuclideIndex;
				for(index=0;index<7;index++){
					linearityMirror.chamb_num_Lin[ch_num][index] = cur_chamb_num[index];
				}
				for(index=0;index<11;index++){
					linearityMirror.serial_num_Lin[ch_num][index] = AmuletSetupLinearityLineatorMenu_lineatorSerial[index];
				}
				for(index=0;index<8;index++){
					linearityMirror.factors_Lin[ch_num][index] = cur_factors[index];
				}

				for(index=0;index<12;index++) calmeasure[index].act_str[0] = 0;
				for(index=0;index<8;index++){
					format_activity_system(AmuletSetupLinearityLineatorMenu_measurement[index].activity, message);
					trim_and_shrink(message);
					ptr = message;
					while((*ptr)!=0){
						pr_mu(ptr, current.printer);
						ptr++;
					}
					strcpy(&(calmeasure[index].act_str[0]), message);
				}
			}else{
				for(index=0;index<12;index++) calmeasure[index].act_str[0] = 0;
			}

			Amulet_printCalib(LINEATOR, calmeasure, &linearityMirror, finished);

			SetAmuletByte(104,0xFF);
			m_iPhase = PHASE_SETUP_LINEARITY_LINEATOR_WAIT;
			break;
	}
}

/*			finished = -1;
			for(index=0; index<8; index++){
				if(AmuletSetupLinearityLineatorMenu_measurement[index].activity == -999){
					message[0] = 0;
					finished = 0;
				}else{
					// Locked: CI
					//format_activity(AmuletSetupLinearityLineatorMenu_measurement[index].activity, current.system, message);
					//format_activity(AmuletSetupLinearityLineatorMenu_measurement[index].activity, CI, message);
					format_activity_system(AmuletSetupLinearityLineatorMenu_measurement[index].activity, message);
					trim_and_shrink(message);
				}
				SetAmuletString(101+ index, message);
			}
			SetAmuletByte(100, 0xff);

			if(finished){
				for(index=0; index<8; index++){
					correctedactivity = nucdecay(AmuletSetupLinearityLineatorMenu_measurement[index].activity,
										         AmuletSetupLinearityLineatorMenu_measurement[index].timestamp,
										         AmuletSetupLinearityLineatorMenu_measurement[0].timestamp,
										         NuclideData_getHalflife(AmuletSetupLinearityLineatorMenu_measurement[index].nuclideid),
										         NuclideData_getHalflifeUnit(AmuletSetupLinearityLineatorMenu_measurement[index].nuclideid));

					linearityMirror.lin_cal_factors[index] = AmuletSetupLinearityLineatorMenu_measurement[0].activity / correctedactivity;
				}

				linearityMirror.type = LINEATOR;
				linearityMirror.num = 8;
				linearityMirror.nuc_index = AmuletSetupLinearityLineatorMenu_measurement[0].nuclideid;

				if(AmuletNotificationMsgMenu_displayed){
					for(index=0; index<8; index++){
						sprintf(message, "Factor: %.3f", linearityMirror.lin_cal_factors[index]);
						SetAmuletString(109+index, message);
					}
					SetAmuletByte(101, 0xFF);
					AmuletNotificationMsgMenu_displayed = 0;
				}else{
					EE_WRITE(lindef, (uchar *) &linearityMirror);
					Amulet_DisplayNotification("Settings Saved", "Lineator settings have been saved.", TRUE);
				}
			} */
