/**
 * \file
 * \details This file handles calls from the Amulet Calicheck Linearity Setup Screen
 */
#define PHASE_SETUP_LINEARITY_CALICHECK_PRE_INIT	0
#define PHASE_SETUP_LINEARITY_CALICHECK_WAIT		1
#define PHASE_SETUP_LINEARITY_CALICHECK_SAVE		2
#define PHASE_SETUP_LINEARITY_CALICHECK_PRINT		3

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
char AmuletSetupLinearityCalicheckMenu_testTotal;
ACTIVITY_MEASUREMENT AmuletSetupLinearityCalicheckMenu_measurement[12];
short AmuletSetupLinearityCalicheckMenu_nuclideIndex;
char AmuletSetupLinearityCalicheckMenu_calicheckSerial[11];

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
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void Amulet_DisplayNotification(char *title, char *notificationstring, bool showOK);
void SetAmuletBackHTML(void);
void Amulet_printCalib(short linearity_type, CALMEAS *calmeas, LINDEF *lin_def, bool finished);
void send_accept_cancel(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen SetupLinearityCalicheck.htm. SetupLinearityCalicheck.htm setups the calicheck linearity test.
 * \param Amulet_Byte_ID Description
 * \param 100 Refresh Display
 * \param 101 Display Tube 1
 * \param 102 Display Tube 2
 * \param 103 Display Tube 3
 * \param 104 Display Tube 4
 * \param 105 Display Tube 5
 * \param 106 Display Tube 6
 * \param 107 Display Tube 7
 * \param 108 Display Tube 8
 * \param 109 Display Tube 9
 * \param 110 Display Tube 10
 * \param 111 Display Tube 11
 * \param 112 Display Tube 12
 * \param 113 Refresh Factors
 * \param 114 Show Delete Button
 * \param Amulet_String_ID Description
 * \param 90 Accept
 * \param 91 Cancel
 * \param 100 Num of Measurements
 * \param 101 Tube 1 Activity
 * \param 102 Tube 2 Activity
 * \param 103 Tube 3 Activity
 * \param 104 Tube 4 Activity
 * \param 105 Tube 5 Activity
 * \param 106 Tube 6 Activity
 * \param 107 Tube 7 Activity
 * \param 108 Tube 8 Activity
 * \param 109 Tube 9 Activity
 * \param 110 Tube 10 Activity
 * \param 111 Tube 11 Activity
 * \param 112 Tube 12 Activity
 * \param 113_114 Factor 1
 * \param 115_116 Factor 2
 * \param 117_118 Factor 3
 * \param 119_120 Factor 4
 * \param 121_122 Factor 5
 * \param 123_124 Factor 6
 * \param 125_126 Factor 7
 * \param 127_128 Factor 8
 * \param 129_130 Factor 9
 * \param 131_132 Factor 10
 * \param 133_134 Factor 11
 * \param 135_136 Factor 12
 * \param 137_138 Chamber Serial Number
 * \param 139 Calicheck Serial Number
 * \param 140_141 Title
 * \param 142_158 Tube Names
 * \param 159_160 Please enter # of Measurements
 * \param 161_162 Please enter Calicheck Serial number
 * \param 163 Print button
 * \param 164 Delete Current Settings
 * \param 165 Calicheck Serial
 * \returns None
 */
void AmuletSetupLinearityCalicheck_menu(void){
	short index;
	char message[52];
	float correctedactivity;
	short ch_num = current.main_chamber;
	static bool finished;
	static char prev_chamb_num[7];
	static char cur_chamb_num[7];
	static float prev_factors[12];
	static float cur_factors[12];
	CALMEAS calmeasure[12];
	bool show_print;
	char *ptr;
	char fmt_str[30];
	char err_message[104];

	switch(m_iPhase){
		case PHASE_SETUP_LINEARITY_CALICHECK_PRE_INIT:
			show_print = FALSE;

			//tube names
			send_amulet_message(L_CAL_1,142);    // "1)Black:"
			send_amulet_message(L_CAL_2,143);    // "2)Black + Red:"
			send_amulet_message(L_CAL_3,144);    // "3)Black + Orange:"
			send_amulet_message(L_CAL_4,145);    // "4)Black + Yellow:"
			send_amulet_message(L_CAL_5,146);    // "5)Black + Green:"
			send_amulet_message(L_CAL_6,147);    // "6)Black + Blue:"
			send_amulet_message(L_CAL_7,148);    // "7)Black + Purple:"
			send_amulet_message(L_CAL_8,149);    // "8)Black + Purple"
			send_amulet_message(L_CAL_82,150);    // "+ Red:"
			send_amulet_message(L_CAL_8,149);    // "8)Black + Purple"
			send_amulet_message(L_CAL_82,150);    // "+ Red:"
			send_amulet_message(L_CAL_9,151);    // "9)Black + Purple"
			send_amulet_message(L_CAL_92,152);    // "+ Orange:"
			send_amulet_message(L_CAL_10,153);    // "10)Black + Purple"
			send_amulet_message(L_CAL_102,154);    // "+ Yellow:"
			send_amulet_message(L_CAL_11,155);    // "11)Black + Purple"
			send_amulet_message(L_CAL_11_2,156);    // "+ Green:"
			send_amulet_message(L_CAL_12,157);    // "12)Black + Purple"
			send_amulet_message(L_CAL_12_2,158);    // "+ Blue:"

			
			if (m_ucClear == 14){
				EE_READ(lindef, (uchar *) &linearityMirror);
				AmuletSetupLinearityCalicheckMenu_testTotal = linearityMirror.num_Cali[ch_num];

				for(index=0; index<12; index++){
					AmuletSetupLinearityCalicheckMenu_measurement[index].activity = -999;
					AmuletSetupLinearityCalicheckMenu_measurement[index].timestamp = (time_t) 0;
					AmuletSetupLinearityCalicheckMenu_measurement[index].nuclideid = -1;
				}
				AmuletSetupLinearityCalicheckMenu_nuclideIndex = NuclideData_getIndexFromName("Tc99m");

				if(AmuletSetupLinearityCalicheckMenu_testTotal < 0){
					AmuletSetupLinearityCalicheckMenu_calicheckSerial[0] = 0;
					prev_chamb_num[0] = 0;
					for(index=0; index<12; index++) prev_factors[index] = -999;
				}else{
					strcpy(AmuletSetupLinearityCalicheckMenu_calicheckSerial, &linearityMirror.serial_num_Cali[ch_num][0]);
					strcpy(prev_chamb_num, &linearityMirror.chamb_num_Cali[ch_num][0]);
					for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
						prev_factors[index] = linearityMirror.factors_Cali[ch_num][index];
					}
					if(AmuletSetupLinearityCalicheckMenu_testTotal < 12){
						for(index=AmuletSetupLinearityCalicheckMenu_testTotal; index<12; index++){
							prev_factors[index] = -999;
						}
					}
				}

				for(index=0; index<6; index++) cur_chamb_num[index] = chamb_vals[ch_num].sn[index];
				cur_chamb_num[6] = 0;

				m_ucClear = 0;

				if(prev_chamb_num[0] != 0){
					if(strcmp(cur_chamb_num, prev_chamb_num) != 0){
						//Amulet_DisplayNotification("Setup CaliCheck", "The current factors are for a different chamber\nand can not be used for this chamber", TRUE);
						get_amulet_message(L_SETUP_CALICHECK,message);    // "Setup Calicheck"
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

			if(linearityMirror.num_Cali[ch_num] >= 0){
				show_print = TRUE;
				SetAmuletByte(114, 0xFF);
			}

			if(prev_chamb_num[0]==0){
				//sprintf(message, "Chamber Serial: %s", cur_chamb_num);
				get_amulet_message(L_CHAMBER_SERIAL,fmt_str);    // "Chamber Serial: %s"
				sprintf(message, fmt_str, cur_chamb_num);
				send_to_amulet_string(137, message);
			}else{
				if(strcmp(cur_chamb_num, prev_chamb_num)==0){
					//sprintf(message, "Chamber Serial: %s", cur_chamb_num);
					get_amulet_message(L_CHAMBER_SERIAL,fmt_str);    // "Chamber Serial: %s"
					sprintf(message, fmt_str, cur_chamb_num);
				}else{
					//sprintf(message, "Ch Serial)Old:%s New:%s", prev_chamb_num, cur_chamb_num);
					get_amulet_message(L_CH_SERIAL_OLD_NEW,fmt_str);    // "Ch Serial)Old:%s New:%s		"
					sprintf(message,fmt_str, prev_chamb_num, cur_chamb_num);
				}
				send_to_amulet_string(137, message);
			}
			delayloop(5);

			if(AmuletSetupLinearityCalicheckMenu_calicheckSerial[0] != 0){
				send_to_amulet_string(139, AmuletSetupLinearityCalicheckMenu_calicheckSerial);
			}
			delayloop(5);

			finished = FALSE;
			if(AmuletSetupLinearityCalicheckMenu_testTotal>=0){
				sprintf(message, "%d", AmuletSetupLinearityCalicheckMenu_testTotal);
				send_to_amulet_string(100, message);
				delayloop(5);

				finished = TRUE;
				for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
					if(AmuletSetupLinearityCalicheckMenu_measurement[index].timestamp == (time_t)0) finished = FALSE;
					else{
						format_activity_system(AmuletSetupLinearityCalicheckMenu_measurement[index].activity, message);
						trim_and_shrink(message);
						send_to_amulet_string(101+index, message);
						delayloop(5);
					}
					SetAmuletByte(101+index, 0xFF);
					delayloop(20);
				}
			}

			send_amulet_message(L_SETUP_LINEARITY_CALICHECK,140);    // "Setup Linearity Calicheck"

			send_amulet_message(L_ENTER_NUM_MEASUREMENTS,159);    // "Please Enter # of Measurements:"
			send_amulet_message(L_PLEASE_ENTER_CALICHECK_SN,161);    // "Please Enter Calicheck Serial #"
			send_amulet_message(L_PRINT,163);    // "Print"
			send_amulet_message(L_DELETE_CURRENT_SETTINGS,164);    // "Delete Current Settings"
			send_amulet_message(L_CALICHECK_SERIAL,165);    // "Calicheck Serial:"
			

			send_accept_cancel();

			
			delayloop(20);
			SetAmuletByte(100, 0xFF);
			delayloop(20);

			if(finished){
				if(AmuletSetupLinearityCalicheckMenu_calicheckSerial[0] != 0) show_print = TRUE;

				for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
					correctedactivity = nucdecay(AmuletSetupLinearityCalicheckMenu_measurement[index].activity,
												 AmuletSetupLinearityCalicheckMenu_measurement[index].timestamp,
												 AmuletSetupLinearityCalicheckMenu_measurement[0].timestamp,
												 NuclideData_getHalflife(AmuletSetupLinearityCalicheckMenu_measurement[index].nuclideid),
												 NuclideData_getHalflifeUnit(AmuletSetupLinearityCalicheckMenu_measurement[index].nuclideid));

					cur_factors[index] = AmuletSetupLinearityCalicheckMenu_measurement[0].activity / correctedactivity;

					if(prev_factors[index] != -999){
						//sprintf(message, "Old: %.2f, New: %.2f", prev_factors[index], cur_factors[index]);
						get_amulet_message(L_FACTOR_OLD_NEW,fmt_str);    // "Old: %.2f, New: %.2f"
						sprintf(message, fmt_str, prev_factors[index], cur_factors[index]);
					}else{
						//sprintf(message, "New Factor: %.2f", cur_factors[index]);
						get_amulet_message(L_NEW_FACTOR,fmt_str);    // "New Factor: %.2f"
						sprintf(message, fmt_str, cur_factors[index]);
					}
					send_to_amulet_string(2*index+113, message);
					delayloop(5);
				}
			}else{
				if(AmuletSetupLinearityCalicheckMenu_testTotal >= 0){
					for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
						if(prev_factors[index] != -999){
							//sprintf(message, "Current Factor: %.2f", prev_factors[index]);
							get_amulet_message(L_CURRENT_FACTOR,fmt_str);    // "Current Factor: %.2f"
							sprintf(message, fmt_str, prev_factors[index]);
							send_to_amulet_string(2*index+113, message);
							//SetAmuletString(2*index+113, message);
							delayloop(5);
						}
					}
				}
			}

			//if(show_print && (current.printer != NONE_PRINTER)) SetAmuletByte(115, 0xFF);
			if(show_print && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(115, 0xFF);
			delayloop(20);
			SetAmuletByte(113, 0xFF);

			m_iPhase = PHASE_SETUP_LINEARITY_CALICHECK_WAIT;
			break;

		case PHASE_SETUP_LINEARITY_CALICHECK_WAIT:
			break;

		case PHASE_SETUP_LINEARITY_CALICHECK_SAVE:
			if(AmuletSetupLinearityCalicheckMenu_testTotal>0){
				if(finished){
					if(AmuletSetupLinearityCalicheckMenu_calicheckSerial[0] != 0){
						linearityMirror.num_Cali[ch_num] = AmuletSetupLinearityCalicheckMenu_testTotal;
						EE_WRITE(lindef.num_Cali[ch_num], (uchar *)&linearityMirror.num_Cali[ch_num]);
						EE_WRITE(lindef.nuc_index_Cali[ch_num], (uchar *)&AmuletSetupLinearityCalicheckMenu_nuclideIndex);

						for(index=0; index<7; index++){
							EE_WRITE(lindef.chamb_num_Cali[ch_num][index], (uchar *)&cur_chamb_num[index]);
						}

						for(index=0; index<11; index++){
							EE_WRITE(lindef.serial_num_Cali[ch_num][index], (uchar *)&AmuletSetupLinearityCalicheckMenu_calicheckSerial[index]);
						}

						for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
							EE_WRITE(lindef.factors_Cali[ch_num][index], (uchar *)&cur_factors[index]);
						}

						cur_factors[0] = -999;
						if(AmuletSetupLinearityCalicheckMenu_testTotal < 12){
							for(index=AmuletSetupLinearityCalicheckMenu_testTotal; index<12; index++){
								EE_WRITE(lindef.factors_Cali[ch_num][index], (uchar *)&cur_factors[0]);
							}
						}

						SetAmuletBackHTML();
						return;
					}else{
						//Amulet_DisplayError("Setup Calicheck Error", "Please fill in Calicheck Serial Number",TRUE);
						get_amulet_message(L_SETUP_CALICHECK_ERROR,message);    // "Setup Calicheck Error"
						get_amulet_message(L_FILL_IN_CALICHECK_SERIAL_NUMBER,err_message);    // "Please fill in Calicheck Serial Number"
						Amulet_DisplayError(message,err_message,TRUE);
						return;
					}
				}else{
					//Amulet_DisplayError("Setup Calicheck Error", "Please fill in all measurements",TRUE);
					get_amulet_message(L_SETUP_CALICHECK_ERROR,message);    // "Setup Calicheck Error"
					get_amulet_message(L_FILL_IN_ALL_MEASUREMENTS,err_message);    // "Please fill in all measurements"
					Amulet_DisplayError(message,err_message,TRUE);
					return;
				}
			}else{
				//Amulet_DisplayError("Setup Calicheck Error", "Please fill in Num of Measurements",TRUE);
				get_amulet_message(L_SETUP_CALICHECK_ERROR,message);    // "Setup Calicheck Error"
				get_amulet_message(L_FILL_IN_NUM_MEASUREMENTS,err_message);    // "Please fill in the Num of Measurements"
				Amulet_DisplayError(message,err_message,TRUE);
				return;
			}
			//break;

		case PHASE_SETUP_LINEARITY_CALICHECK_PRINT:
			beep_amulet();
			EE_READ(lindef, (uchar *) &linearityMirror);

			for(index=0;index<12;index++) calmeasure[index].act_str[0] = 0;

			if(finished){
				linearityMirror.num_Cali[ch_num] = AmuletSetupLinearityCalicheckMenu_testTotal;
				linearityMirror.nuc_index_Cali[ch_num] = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
				for(index=0;index<7;index++){
					linearityMirror.chamb_num_Cali[ch_num][index] = cur_chamb_num[index];
				}
				for(index=0;index<11;index++){
					linearityMirror.serial_num_Cali[ch_num][index] = AmuletSetupLinearityCalicheckMenu_calicheckSerial[index];
				}

				for(index=0;index<AmuletSetupLinearityCalicheckMenu_testTotal;index++){
					linearityMirror.factors_Cali[ch_num][index] = cur_factors[index];
					format_activity_system(AmuletSetupLinearityCalicheckMenu_measurement[index].activity, message);
					trim_and_shrink(message);
					ptr = message;
					while((*ptr)!=0){
						pr_mu(ptr, current.printer);
						ptr++;
					}
					strcpy(&(calmeasure[index].act_str[0]), message);
				}
			}

			Amulet_printCalib(CALICHECK, calmeasure, &linearityMirror, finished);

			SetAmuletByte(116, 0xFF);
			m_iPhase = PHASE_SETUP_LINEARITY_CALICHECK_WAIT;
			break;
	}
}

/*			if(AmuletSetupLinearityCalicheckMenu_testTotal != 0){
				if((AmuletSetupLinearityCalicheckMenu_testTotal < 5) || (AmuletSetupLinearityCalicheckMenu_testTotal > 12)){
					Amulet_DisplayError("Out of Range Error", "Num of Measurements must be between 5 and 12");
					AmuletSetupLinearityCalicheckMenu_testTotal = 0;
				}else{
					sprintf(message, "%d", AmuletSetupLinearityCalicheckMenu_testTotal);
					SetAmuletString(100, message);

					finished = -1;
					for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
						if(AmuletSetupLinearityCalicheckMenu_measurement[index].activity == -999) finished = 0;
						else{
							// Locked: CI
							//format_activity(AmuletSetupLinearityCalicheckMenu_measurement[index].activity, current.system, message);
							//format_activity(AmuletSetupLinearityCalicheckMenu_measurement[index].activity, CI, message);
							format_activity_system(AmuletSetupLinearityCalicheckMenu_measurement[index].activity, message);
							trim_and_shrink(message);
							SetAmuletString(101+index, message);
						}
						SetAmuletByte(101+index, 0xFF);
					}
					SetAmuletByte(100, 0xFF);

					if(finished){
						for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
							correctedactivity = nucdecay(AmuletSetupLinearityCalicheckMenu_measurement[index].activity,
														 AmuletSetupLinearityCalicheckMenu_measurement[index].timestamp,
														 AmuletSetupLinearityCalicheckMenu_measurement[0].timestamp,
														 NuclideData_getHalflife(AmuletSetupLinearityCalicheckMenu_measurement[index].nuclideid),
														 NuclideData_getHalflifeUnit(AmuletSetupLinearityCalicheckMenu_measurement[index].nuclideid));
							linearityMirror.lin_cal_factors[index] = AmuletSetupLinearityCalicheckMenu_measurement[0].activity / correctedactivity;
						}
						linearityMirror.type = CALICHECK;
						linearityMirror.num = AmuletSetupLinearityCalicheckMenu_testTotal;
						linearityMirror.nuc_index = AmuletSetupLinearityCalicheckMenu_measurement[0].nuclideid;
						if(AmuletNotificationMsgMenu_displayed){
							for(index=0; index<AmuletSetupLinearityCalicheckMenu_testTotal; index++){
								sprintf(message, "Factor: %.3f", linearityMirror.lin_cal_factors[index]);
								SetAmuletString(113+index, message);
							}
							SetAmuletByte(99, 0xFF);
							AmuletNotificationMsgMenu_displayed = 0;
						}else{
							EE_WRITE(lindef, (uchar *) &linearityMirror);
							Amulet_DisplayNotification("Settings Saved", "Calicheck settings have been saved.", TRUE);
						}
					}
				}
			} */
