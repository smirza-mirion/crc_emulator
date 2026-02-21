/**
 * \file
 * \details This file handles calls from the Amulet Accuracy Test Screen.
 */
#define PHASE_ACCURACY_PRE_INIT 0
#define PHASE_ACCURACY_WAIT 	1
#define PHASE_ACCURACY_PRINT	2
#define PHASE_ACCURACY_SAVE		3

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "daily.h"
#include "printer.h"

extern CURRENT  current;
extern int m_iPhase;
extern unsigned char m_ucClear;
extern short num_daily;
extern bool AmuletAccuracy_daily;
extern MEASUREMENT measurement[];
extern bool AmuletGenericYesNo_AccuracyTestPending;
extern ACCDATA acc_data[];
extern uchar AmuletGenericYesNo_config;
extern char AmuletGenericYesNo_comment[26];
extern bool AmuletDailyMenu_okdata;

void send_to_amulet_string(uchar ucIndex, char message0[]);
short get_num_sources(bool daily);
void trim(char *acByte);
void trim_and_shrink(char *acByte);
short prdailysec(bool okdata,short acc_ret);
short Amulet_printAccuracyAndConstancy(bool printconstancy);
void PushPageStack(unsigned char ucPage);

/**
 * \details Handles the Amulet Screen Accuracy.htm. Accuracy.htm runs the accuracy test.
 * \param Amulet_Byte_ID Description
 * \param 80 Language		(STATE) English = 0, French = 1
 * \param 91  btnSourceMeasureText1 Show
 * \param 92  btnSourceMeasureText2 Show
 * \param 93  btnSourceMeasureText3 Show
 * \param 94  btnSourceMeasureText4 Show
 * \param 95  btnSourceMeasureText5 Show
 * \param 96  btnSourceMeasureText6 Show
 * \param 97  btnAutoConstancy Show
 * \param 100 Show Title
 * \param 101 Show Source1
 * \param 102 Show Result1
 * \param 103 Show Source2
 * \param 104 Show Result2
 * \param 105 Show Source3
 * \param 106 Show Result3
 * \param 107 Show Source4
 * \param 108 Show Result4
 * \param 109 Show Source5
 * \param 110 Show Result5
 * \param 111 Show Source6
 * \param 112 Show Result6
 * \param 113 Toggle Red Result1
 * \param 114 Toggle Red Result2
 * \param 115 Toggle Red Result3
 * \param 116 Toggle Red Result4
 * \param 117 Toggle Red Result5
 * \param 118 Toggle Red Result6
 * \param 119 Show Print button
 * \param 120 Show No Source Message
 * \param 121 Show Autoconstancy button
 * \param 122 Show Print and Hidden Buttons
 * \param Amulet_String_ID Description
 * \param 80 Title (sfTitle)
 * \param 81 S/N: (sfSourcesnlabel1,sfSourcesnlabel2,sfSourcesnlabel3,sfSourcesnlabel4,sfSourcesnlabel5,sfSourcesnlabel6)
 * \param 82 Measured: (sfSourcemeasurelabel1,sfSourcemeasurelabel2,sfSourcemeasurelabel3,sfSourcemeasurelabel4,sfSourcemeasurelabel5,sfSourcemeasurelabel6)
 * \param 83 Calculated: (sfSourcecalclabel1,sfSourcecalclabel2,sfSourcecalclabel3,sfSourcecalclabel4,sfSourcecalclabel5,sfSourcecalclabel6)
 * \param 84 AutoConstancy (btnAutoConstancy)
 * \param 85_86  Please Enter Password
 * \param 100 Nuclide1
 * \param 101 SN1
 * \param 102 Measured1
 * \param 103 Calc1
 * \param 104 Devtop1
 * \param 105 Devbottom1
 * \param 106 Nuclide2
 * \param 107 SN2
 * \param 108 Measured2
 * \param 109 Calc2
 * \param 110 Devtop2
 * \param 111 Devbottom2
 * \param 112 Nuclide3
 * \param 113 SN3
 * \param 114 Measured3
 * \param 115 Calc3
 * \param 116 Devtop3
 * \param 117 Devbottom3
 * \param 118 Nuclide4
 * \param 119 SN4
 * \param 120 Measured4
 * \param 121 Calc4
 * \param 122 Devtop4
 * \param 123 Devbottom4
 * \param 124 Nuclide5
 * \param 125 SN5
 * \param 126 Measured5
 * \param 127 Calc5
 * \param 128 Devtop5
 * \param 129 Devbottom5
 * \param 130 Nuclide6
 * \param 131 SN6
 * \param 132 Measured6
 * \param 133 Calc6
 * \param 134 Devtop6
 * \param 135 Devbottom6
 * \returns None
 */
void AmuletAccuracy_menu(void) {
	short ch_num = current.main_chamber;
	static short ciso;
	char acMsg[100];
	char buffer[100];
	short index;
	float diff;
	bool started, autoconstancy;
	short iret;

	switch(m_iPhase) {
		case PHASE_ACCURACY_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_ACCURACY, 80);    // "Accuracy"
			send_amulet_message(L_SN2, 81);    // "S/N:"
			send_amulet_message(L_MEASURED, 82);    // "Measured:"
			send_amulet_message(L_CALCULATED, 83);    // "Calculated:"
			send_amulet_message(L_AUTOCONSTANCY, 84);    // "AutoConstancy"
			send_amulet_message(L_ENTER_PASSWORD, 85);    // "Please Enter Password:"
			SetAmuletByte(100, 0xFF);
			delayloop(20);

			if(current.num_chambers>1) SetAmuletByte(21, 0xFF);
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

			if(m_ucClear==28){
				num_daily = get_num_sources(AmuletAccuracy_daily);
				if(num_daily > 0){
					ciso = get_const_source2();
					get_acc_data(ciso, AmuletAccuracy_daily, ch_num);
					measurement[ch_num].future.dosetime = NO_TIME;
				}
				m_ucClear = 0;
			}

			AmuletGenericYesNo_AccuracyTestPending = FALSE;
			if(num_daily == 0){
				//SetAmuletString(100, "No Source Data");
				send_amulet_message(L_NOSOURCEDATA, 100);    // "No Source Data"
				SetAmuletByte(120, 0xFF);
			}else{
				started = FALSE;
				for(index = 0; index<num_daily; index++){
					strcpy(buffer, acc_data[index].nucname);
					trim_and_shrink(buffer);
					send_to_amulet_string(6*index+100, buffer);
					delayloop(5);//
					strcpy(buffer, acc_data[index].sn);
					trim(buffer);
					send_to_amulet_string(6*index+101, buffer);
					delayloop(5);//

					if(acc_data[index].test_res[ch_num].ms_kun>=0){
						acc_data[index].test_res[ch_num].ms_kun = format_activity_system(acc_data[index].test_res[ch_num].ms_act, acc_data[index].test_res[ch_num].msact);
						acc_data[index].test_res[ch_num].pr_kun = format_activity_system(acc_data[index].test_res[ch_num].pred_act, acc_data[index].test_res[ch_num].pract);
						diff = 100.0 * (acc_data[index].test_res[ch_num].ms_act - acc_data[index].test_res[ch_num].pred_act)/acc_data[index].test_res[ch_num].pred_act;
						acc_data[index].test_res[ch_num].diff = diff;
						if(fabs(diff)>20.0){
							acc_data[index].test_res[ch_num].nc = -1;
							strcpy(acc_data[index].test_res[ch_num].var, "*****");
						}else{
							acc_data[index].test_res[ch_num].nc = sprintf(acc_data[index].test_res[ch_num].var, "%5.1f", diff);
						}

						send_to_amulet_string(6*index+102, acc_data[index].test_res[ch_num].msact);
						delayloop(5); //
						send_to_amulet_string(6*index+103, acc_data[index].test_res[ch_num].pract);
						delayloop(5); //
						//SetAmuletString(6*index+104, "Deviation:");
						send_amulet_message(L_DEVIATION, 6*index+104);    // "Deviation:"

						delayloop(5); //
						if(acc_data[index].test_res[ch_num].nc == -1){
							//SetAmuletString(6*index+105, "Error");
							send_amulet_message(L_ERROR2, 6*index+105);    // "Error"
							delayloop(5); //
							SetAmuletByte(index+113, 0xFF);
							delayloop(10); //
						}else{
							strcpy(buffer, acc_data[index].test_res[ch_num].var);
							strcat(buffer, " %");
							send_to_amulet_string(6*index+105, buffer);
							delayloop(5); //
						}
						//delayloop(2);
						SetAmuletByte(2*index+102, 0xFF);
						delayloop(30);
						started = TRUE;
					}

					//delayloop(2);
					SetAmuletByte(2*index+101, 0xFF);
					delayloop(30);
				}

				if(started){
					if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER)) SetAmuletByte(119, 0xFF);
					//if((ciso >= 0) && (acc_data[ciso].test_res[ch_num].nc != -1)) SetAmuletByte(118, 0xFF);
					autoconstancy = FALSE;
					for(index = 0; index<num_daily; index++){
						if((acc_data[index].const_source) && (acc_data[index].test_res[ch_num].ms_kun>=0) && (acc_data[index].test_res[ch_num].nc != -1)){
							autoconstancy = TRUE;
							break;
						}
					}

					if(autoconstancy) SetAmuletByte(121, 0xFF);

					//SetAmuletByte(120, 0xFF);
					AmuletGenericYesNo_AccuracyTestPending = TRUE;
				}
			}

			m_iPhase = PHASE_ACCURACY_WAIT;
			break;

		case PHASE_ACCURACY_WAIT:
			break;

		case PHASE_ACCURACY_PRINT:
			m_iPhase = PHASE_ACCURACY_WAIT;
			beep_amulet();
			if(AmuletAccuracy_daily){
				iret = prdailysec(AmuletDailyMenu_okdata, 0);
			}else{
				iret = Amulet_printAccuracyAndConstancy(FALSE);
			}
			if(iret==1) dosig(current.printer);

			SetAmuletByte(122, 0xFF);
			break;

		case PHASE_ACCURACY_SAVE:
			beep_amulet();
			AmuletGenericYesNo_config = 6;
			AmuletGenericYesNo_comment[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[GENERICYESNO_HTM]);
			PushPageStack(AmuletHTMLIndex[GENERICYESNO_HTM]);
			return;

		default:
			break;
	}
}
