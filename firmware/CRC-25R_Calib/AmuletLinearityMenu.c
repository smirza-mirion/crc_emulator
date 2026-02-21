/**
 * \file
 * \details This file handles calls from the Amulet Linearity Test Screen
 */
#define PHASE_LINEARITY_PRE_INIT 0

#define PHASE_LINEARITY_STANDARD_INIT 1
#define PHASE_LINEARITY_STANDARD_WAIT_FULL_SCREEN 2
#define PHASE_LINEARITY_STANDARD_AFTER_FULL_SCREEN 3
#define PHASE_LINEARITY_STANDARD_WAIT_FOR_MEASURE 4
#define PHASE_LINEARITY_STANDARD_MEASURE_ACCEPTED 5
#define PHASE_LINEARITY_STANDARD_WAIT_FOR_ACCEPT 6
#define PHASE_LINEARITY_STANDARD_ACCEPTED 7

#define PHASE_LINEARITY_LINEATOR_INIT 85
#define PHASE_LINEARITY_LINEATOR_WAIT_FULL_SCREEN 86
#define PHASE_LINEARITY_LINEATOR_AFTER_FULL_SCREEN 87
#define PHASE_LINEARITY_LINEATOR_WAIT_FOR_ACCEPT 88
#define PHASE_LINEARITY_LINEATOR_ACCEPTED 89
#define PHASE_LINEARITY_LINEATOR_DELAY_AFTER_ACCEPT 90
#define PHASE_LINEARITY_LINEATOR_DELAY_BEFORE_FINISH 91

#define PHASE_LINEARITY_CALICHECK_INIT 170
#define PHASE_LINEARITY_CALICHECK_WAIT_FULL_SCREEN 171
#define PHASE_LINEARITY_CALICHECK_AFTER_FULL_SCREEN 172
#define PHASE_LINEARITY_CALICHECK_WAIT_FOR_ACCEPT 173
#define PHASE_LINEARITY_CALICHECK_ACCEPTED 174
#define PHASE_LINEARITY_CALICHECK_DELAY_AFTER_ACCEPT 175
#define PHASE_LINEARITY_CALICHECK_DELAY_BEFORE_FINISH 176

#define PHASE_LINEARITY_STOP_UPDATING 252
#define PHASE_LINEARITY_PRINT 253
#define PHASE_LINEARITY_CLEAR_MEASUREMENTS 254
#define PHASE_LINEARITY_FINISH 255

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "printer.h"
#include "chambfac.h"
#include "i2c.h"
#include "nuc.h"
#include "linearity.h"

typedef struct linmeas LINMEAS;
struct linmeas{
    float meas_act;
    float factor;
    float pc_ratio;
    char act_str[10];
    time_t meas_time;
};
typedef struct calmeas CALMEAS;
struct calmeas{
    float meas_act;
    float result;
    float var;
    char res_str[10];
    time_t meas_time;
};

extern CURRENT  current;
extern int m_iPhase;
extern volatile short linearity_type;
extern unsigned char m_ucClear;
extern STANDLIN standlin[][12];
extern MEASUREMENT measurement[];
extern CHAMBER chamber[];

void trim(char *acByte);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void predicted_standard_activity(float *predicted_act, short num_test, float *slope, NUCDATA *nucdata);
void LinearElapsedTime(short index, LINDEF *def, time_t start_time, char *elapsed, char *time_due);
unsigned char PopPageStack(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void predicted_standard_activity(float *predicted_act, short num_test, float *slope, NUCDATA *nucdata);
void Amulet_printLinearityStd(float *predicted_act, short num_test, char *nucname);
void Amulet_printLinearityLin(LINMEAS *linmeas, LINDEF *lindef);
void Amulet_printLinearityCali(CALMEAS *calmeas, LINDEF *lindef);

/**
 * \details Handles the Amulet Screen Linearity.htm. Linearity.htm runs the Standard Linearity, Lineator and Calicheck tests.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Status
 * \param 80 Language (State)
 * \param 100 0xFF = Display Print Button
 * \param 101 0xFF = Stop All Updating
 * \param 102 0xFF = Show Buttons After Printing
 * \param 158 0xFF = Display String Fields from Calicheck
 * \param 159 0xFF = Display String Fields from Lineator
 * \param 160 0xFF = Display String Fields from Standard
 * \param 161 0xFF = Display Clear All Button
 * \param 162 0xFF = Set Line1 Dynamic
 * \param 163 0xFF = Set Line2 Dynamic
 * \param 164 0xFF = Set Line3 Dynamic
 * \param 165 0xFF = Set Line4 Dynamic
 * \param 166 0xFF = Set Line5 Dynamic
 * \param 167 0xFF = Set Line6 Dynamic
 * \param 168 0xFF = Set Line7 Dynamic
 * \param 169 0xFF = Set Line8 Dynamic
 * \param 170 0xFF = Set Line9 Dynamic
 * \param 171 0xFF = Set Line10 Dynamic
 * \param 172 0xFF = Set Line11 Dynamic
 * \param 173 0xFF = Set Line12 Dynamic
 * \param 174 0xFF = Set Line13 Dynamic
 * \param 175 0xFF = Display Std Measure Button
 * \param 176 0xFF = Hide Std Measure Button
 * \param 177 0xFF = Display Std Accept Button
 * \param 178 0xFF = Hide Std Accept Button
 * \param 179 0xFF = Display Lineator Accept Button
 * \param 180 0xFF = Hide Lineator Accept Button
 * \param 181 0xFF = Display Calicheck Accept Button
 * \param 182 0xFF = Display Line1
 * \param 183 0xFF = Display Line2
 * \param 184 0xFF = Display Line3
 * \param 185 0xFF = Display Line4
 * \param 186 0xFF = Display Line5
 * \param 187 0xFF = Display Line6
 * \param 188 0xFF = Display Line7
 * \param 189 0xFF = Display Line8
 * \param 190 0xFF = Display Line9
 * \param 191 0xFF = Display Line10
 * \param 192 0xFF = Display Line11
 * \param 193 0xFF = Display Line12
 * \param 194 0xFF = Display Line13
 * \param Amulet_Word_ID Description
 * \param 252 Measure Button Y offset
 * \param 253 Measure Button X offset
 * \param 254 Accept Button Y offset
 * \param 255 Accept Button X offset
 * \param Amulet_String_ID Description
 * \param 100_101 Please Enter Password:
 * \param 102_103 Clear All Measurements
 * \param 104 Accept
 * \param 105_106 Measure Now
 * \param 209 Title
 * \param 160 Row1, Col1
 * \param 161 Row1, Col2
 * \param 162 Row1, Col3
 * \param 163 Row2, Col1
 * \param 164 Row2, Col2
 * \param 165 Row2, Col3
 * \param 166 Row3, Col1
 * \param 167 Row3, Col2
 * \param 168 Row3, Col3
 * \param 169 Row4, Col1
 * \param 170 Row4, Col2
 * \param 171 Row4, Col3
 * \param 172 Row5, Col1
 * \param 173 Row5, Col2
 * \param 174 Row5, Col3
 * \param 175 Row6, Col1
 * \param 176 Row6, Col2
 * \param 177 Row6, Col3
 * \param 178 Row7, Col1
 * \param 179 Row7, Col2
 * \param 180 Row7, Col3
 * \param 181 Row8, Col1
 * \param 182 Row8, Col2
 * \param 183 Row8, Col3
 * \param 184 Row9, Col1
 * \param 185 Row9, Col2
 * \param 186 Row9, Col3
 * \param 187 Row10, Col1
 * \param 188 Row10, Col2
 * \param 189 Row10, Col3
 * \param 190 Row11, Col1
 * \param 191 Row11, Col2
 * \param 192 Row11, Col3
 * \param 193 Row12, Col1
 * \param 194 Row12, Col2
 * \param 195 Row12, Col3
 * \param 196 Row13, Col1
 * \param 197 Row13, Col2
 * \param 198 Row13, Col3
 * \returns None
 */
void AmuletLinearity_menu(void) {
	short ch_num = current.main_chamber;
	short i;
	char acMsg[100];
	char acMsg2[100];
	char acMsg3[100];
	char titlestring[100], messagestring[100];
	char nucName[8];
	float slope;
	float meas0;
	float var;
	//float act;
    float act1;
    float corr_act;
    float mean;
    //short kun;
    //unsigned long int temp_msec;
	static LINDEF def;
	static short num_meas;
	static short num_test;
	static char nucname[8];
	static short nuc_index;
	static NUCDATA nucdata;
	static float factors[12];
	static time_t time0, time1;
	static LINMEAS linmeas[8];
	static CALMEAS calmeas[12];
	static unsigned long wait;
	float predicted_act[12];
	static char factor[8][25];
	static char ratio[8][25];
	static char caliresult[12][25];
	static char calivar[12][25];
	static bool printer_pressed;
	short ch_type, ch_type_mod;

	ch_type = chamber_type(ch_num);
	switch(m_iPhase) {
		case PHASE_LINEARITY_PRE_INIT:
			//if(current.num_chambers>1) SetAmuletByte(21, 0xFF);

			SetAmuletByte(80, current.language);
			send_amulet_message(L_ENTER_PASSWORD, 100);    // "Please Enter Password:"
			send_amulet_message(L_CLEAR_ALL_MEASUREMENTS, 102);    // "Clear All Measurements"
			send_amulet_message(L_ACCEPT_BUTTON, 104);    // "Accept"
			send_amulet_message(L_MEASURE_NOW, 105);    // "Measure Now"
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

			EE_READ(lindef, (uchar *)&def);

			switch(linearity_type) {
				/*case -1:
					SetAmuletString(159, "Linearity");
					SetAmuletString(160, "Must Define");
					SetAmuletString(163, "Linearity Test");
					SetAmuletByte(160,0xFF);
					m_ucClear = 0;
					m_iPhase = PHASE_LINEARITY_FINISH;
					break;*/

				case STANDARD:
					ch_type_mod = chamber_type_77t_remap(ch_type); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded

					if(def.num_Std[ch_type_mod] == -1){
						//sprintf(acMsg, "No Settings");
						//SetAmuletString(160, acMsg);
						send_amulet_message(L_NO_SETTINGS, 160);    // "No Settings"
						//if(ch_type == R_CHAMB) SetAmuletString(163, "for R Chamber");
						//else SetAmuletString(163, "for PET Chamber");
						if(ch_type == R_CHAMB) send_amulet_message(L_FOR_R_CHAMBER, 163);    // "for R Chamber"
						else if(ch_type == P_CHAMB) send_amulet_message(L_FOR_PET_CHAMBER, 163);    // "for PET Chamber"
						else if(ch_type == B_CHAMB) send_amulet_message(L_FOR_BT_CHAMBER, 163);    // "for BT Chamber"
						else if(ch_type == ONE_DOT_EIGHT_CHAMB) send_amulet_message(L_FOR_18_CHAMBER, 163);    // "for 1.8 Atm Chamber"
						else if(ch_type == C_CHAMB) send_amulet_message(L_FOR_C_CHAMBER, 163);    // "for HR Chamber"
						else if(ch_type == K_CHAMB) send_amulet_message(L_FOR_K_CHAMBER, 163);    // "for 1K Chamber"
						SetAmuletByte(160, 0xFF);
						m_iPhase = PHASE_LINEARITY_FINISH;
					}else{
						ch_type_mod = chamber_type_77t_remap(ch_type); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
						if(NuclideData_getEffectiveResponse(def.nuc_index_Std[ch_type_mod], ch_type) == 0.0){
							//sprintf(acMsg, "Unable to find nuclide");
							//SetAmuletString(160, acMsg);
							send_amulet_message(L_UNABLE_TO_FIND_NUCLIDE2, 160);    // "Unable to find Nuclide"

							NuclideData_getName(def.nuc_index_Std[ch_type_mod], nucName);
							send_to_amulet_string(163, nucName);

							//SetAmuletString(163, "calibration number");
							send_amulet_message(L_CALIBRATION_NUMBER, 166);    // "calibration number"

							//if(ch_type == R_CHAMB) SetAmuletString(166, "for R Chamber");
							//else SetAmuletString(166, "for PET Chamber");

							if(ch_type == R_CHAMB) send_amulet_message(L_FOR_R_CHAMBER2, 169);    // "for R Chamber"
							else if(ch_type == P_CHAMB) send_amulet_message(L_FOR_PET_CHAMBER2, 169);    // "for PET Chamber"
							else if(ch_type == B_CHAMB) send_amulet_message(L_FOR_BT_CHAMBER2, 169);    // "for BT Chamber"
							else if(ch_type == ONE_DOT_EIGHT_CHAMB) send_amulet_message(L_FOR_18_CHAMBER2, 169);    // "for 1.8 Atm Chamber"
							else if(ch_type == C_CHAMB) send_amulet_message(L_FOR_C_CHAMBER2, 169);    // "for HR Chamber"
							else if(ch_type == K_CHAMB) send_amulet_message(L_FOR_K_CHAMBER2, 169);    // "for 1K Chamber"
							SetAmuletByte(160, 0xFF);
							m_iPhase = PHASE_LINEARITY_FINISH;
						}else{
							m_iPhase = PHASE_LINEARITY_STANDARD_INIT;
						}
					}
					//SetAmuletString(159, "Linearity, Std");
					send_amulet_message(L_LINEARITY_STD, 209);    // "Linearity, Std"
					m_ucClear = 0;
					break;

				case LINEATOR:
					if(NuclideData_getEffectiveResponse(def.nuc_index_Lin[ch_num], ch_type) == 0.0){
						send_amulet_message(L_UNABLE_TO_FIND_NUCLIDE2, 160);    // "Unable to find Nuclide"

						NuclideData_getName(def.nuc_index_Lin[ch_num], nucName);
						send_to_amulet_string(163, nucName);

						send_amulet_message(L_CALIBRATION_NUMBER, 166);    // "calibration number"

						if(ch_type == R_CHAMB) send_amulet_message(L_FOR_R_CHAMBER2, 169);    // "for R Chamber"
						else if(ch_type == P_CHAMB) send_amulet_message(L_FOR_PET_CHAMBER2, 169);    // "for PET Chamber"
						else if(ch_type == B_CHAMB) send_amulet_message(L_FOR_BT_CHAMBER2, 169);    // "for BT Chamber"
						else if(ch_type == ONE_DOT_EIGHT_CHAMB) send_amulet_message(L_FOR_18_CHAMBER2, 169);    // "for 1.8 Atm Chamber"
						else if(ch_type == C_CHAMB) send_amulet_message(L_FOR_C_CHAMBER2, 169);    // "for HR Chamber"
						else if(ch_type == K_CHAMB) send_amulet_message(L_FOR_K_CHAMBER2, 169);    // "for 1K Chamber"
						SetAmuletByte(160, 0xFF);
						m_iPhase = PHASE_LINEARITY_FINISH;
					}else{
						sprintf(acMsg, "Lineator #%s", &(def.serial_num_Lin[ch_num][0]));
						//send_to_amulet_string(159, acMsg);
						send_to_amulet_string(209, acMsg);
						m_iPhase = PHASE_LINEARITY_LINEATOR_INIT;
					}
					break;

				case CALICHECK:
					if(NuclideData_getEffectiveResponse(def.nuc_index_Cali[ch_num], ch_type) == 0.0){
						send_amulet_message(L_UNABLE_TO_FIND_NUCLIDE2, 160);    // "Unable to find Nuclide"

						NuclideData_getName(def.nuc_index_Cali[ch_num], nucName);
						send_to_amulet_string(163, nucName);

						send_amulet_message(L_CALIBRATION_NUMBER, 166);    // "calibration number"

						if(ch_type == R_CHAMB) send_amulet_message(L_FOR_R_CHAMBER2, 169);    // "for R Chamber"
						else if(ch_type == P_CHAMB) send_amulet_message(L_FOR_PET_CHAMBER2, 169);    // "for PET Chamber"
						else if(ch_type == B_CHAMB) send_amulet_message(L_FOR_BT_CHAMBER2, 169);    // "for BT Chamber"
						else if(ch_type == ONE_DOT_EIGHT_CHAMB) send_amulet_message(L_FOR_18_CHAMBER2, 169);    // "for 1.8 Atm Chamber"
						else if(ch_type == C_CHAMB) send_amulet_message(L_FOR_C_CHAMBER2, 169);    // "for HR Chamber"
						else if(ch_type == K_CHAMB) send_amulet_message(L_FOR_K_CHAMBER2, 169);    // "for 1K Chamber"
						SetAmuletByte(160, 0xFF);
						m_iPhase = PHASE_LINEARITY_FINISH;
					}else{
						sprintf(acMsg, "Calicheck #%s", &(def.serial_num_Cali[ch_num][0]));
						//send_to_amulet_string(159, acMsg);
						send_to_amulet_string(209, acMsg);
						m_iPhase = PHASE_LINEARITY_CALICHECK_INIT;
					}
					break;

				default:
					m_iPhase = PHASE_LINEARITY_FINISH;
					m_ucClear = 0;
					break;
			}
			break;

		case PHASE_LINEARITY_STANDARD_INIT:
			if(current.num_chambers>0){
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				ch_type_mod = chamber_type_77t_remap(ch_type); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded
				nuc_index = def.nuc_index_Std[ch_type_mod];

				set_nuclide_data(nuc_index, ch_num);
				NuclideData_getNuclide(nuc_index, &nucdata);
				strcpy(nucname, nucdata.name);

				//num_meas = def.num;
				ch_type_mod = chamber_type_77t_remap(ch_type); // C_CHAMB and K_CHAMB will use the R_CHAMB position so that lindef does not need to be expanded

				num_meas = def.num_Std[ch_type_mod];
				num_test = 1;
				for(i=0; i<12; i++){
					if(standlin[ch_num][i].meas_flag != 1) break;
					num_test++;
				}

				if(num_test > 1){
					//SetAmuletByte(161, 0xFF);
					for(i=0;i<12;i++) predicted_act[i] = 0;
					predicted_standard_activity(predicted_act, num_test - 1, &slope, &nucdata);
				}

				send_to_amulet_string(160, "#");
				delayloop(5);
				//SetAmuletString(161, "Measured");
				send_amulet_message(L_MEASURED2, 161);    // "Measured"
				delayloop(5);
				//SetAmuletString(162, "Var %");
				send_amulet_message(L_PERCENT_VAR, 162);    // "% Var"
				delayloop(5);
				SetAmuletByte(182, 0xFF);
				delayloop(20);
				for(i=0; i<num_test-1; i++){
					sprintf(acMsg, "%d)", i+1);
					dateout_language(acMsg2, &standlin[ch_num][i].date, 2);
					strcat(acMsg, acMsg2);
					strcat(acMsg, "; ");
					timeoutsec(acMsg2, &standlin[ch_num][i].date);
					strcat(acMsg, acMsg2);
					send_to_amulet_string(3*i+163, acMsg);
					delayloop(5);
					strcpy(acMsg, standlin[ch_num][i].actstr);
					trim(acMsg);
					send_to_amulet_string(3*i+164, acMsg);
					delayloop(5);
					//Removed: Units Conversion is not needed
					//meas0 = convact(standlin[ch_num][i].meas, standlin[ch_num][i].syst);
					meas0 = standlin[ch_num][i].meas;
					var = 100. * (meas0 - predicted_act[i]) / predicted_act[i];
					sprintf(acMsg, "%6.2f", var);
					trim(acMsg);
					send_to_amulet_string(3*i+165, acMsg);
					delayloop(5);
					SetAmuletByte(i+183, 0xFF);
					delayloop(20);
				}
				delayloop(50);
				SetAmuletByte(160, 0xFF);
				delayloop(50);
				if(num_test > 1){
					SetAmuletByte(161, 0xFF);
				}
				if (num_test <= num_meas) SetAmuletByte(num_test + 162, 0xFF);

				m_iPhase = PHASE_LINEARITY_STANDARD_WAIT_FULL_SCREEN;
			}
			break;

		case PHASE_LINEARITY_STANDARD_WAIT_FULL_SCREEN:
			break;

		case PHASE_LINEARITY_STANDARD_AFTER_FULL_SCREEN:
			if(current.num_chambers>0){
				SetAmuletWord(255, 532);
				SetAmuletWord(254, 39 * (num_test) + 41);
				SetAmuletWord(253, 532);
				SetAmuletWord(252, 39 * (num_test) + 41);

				if(num_test == 1){
					m_iPhase = PHASE_LINEARITY_STANDARD_MEASURE_ACCEPTED;
				}else if (num_test > num_meas){
					num_test--;
					//if(current.printer!=NONE_PRINTER)
					if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
						SetAmuletByte(100, 0xFF);
					m_iPhase = PHASE_LINEARITY_FINISH;
				}else{
					for(i=num_test; i<=num_meas; i++){
						LinearElapsedTime(i - 1, &def, standlin[ch_num][0].date, acMsg, acMsg2);
						sprintf(acMsg3, "%d)", i);
						strcat(acMsg3, acMsg2);
						send_to_amulet_string(3*(i - 1)+163, acMsg3);
						delayloop(5);
						send_to_amulet_string(3*(i - 1)+164, acMsg);
						delayloop(5);
						SetAmuletByte(i + 162, 0xFF);
						delayloop(20);
						SetAmuletByte(i + 182, 0xFF);
						delayloop(20);
					}
					SetAmuletByte(175, 0xFF);
					delayloop(70);
					SetAmuletByte(175, 0xFF);
					m_iPhase = PHASE_LINEARITY_STANDARD_WAIT_FOR_MEASURE;
				}
			}
			break;

		case PHASE_LINEARITY_STANDARD_WAIT_FOR_MEASURE:
			// Wait for user to press Measure button
			break;

		case PHASE_LINEARITY_STANDARD_MEASURE_ACCEPTED:
			if(current.num_chambers>0){
				if(num_test == 1){
					//SetAmuletString(163, "First Measurement:");
					send_amulet_message(L_FIRST_MEASUREMENT, 163);    // "First Measurement:"
				}else{
					for(i=0;i<12;i++) predicted_act[i] = 0;
					read_clock(&standlin[ch_num][num_test-1].date);
					standlin[ch_num][num_test-1].meas = nucdecay(standlin[ch_num][0].meas, standlin[ch_num][0].date, standlin[ch_num][num_test-1].date, nucdata.halflife, nucdata.hlunit);
					predicted_standard_activity(predicted_act, num_test, &slope, &nucdata);
					// Locked: CI
					//format_activity(predicted_act[num_test - 1], current.system, acMsg);
					//format_activity(predicted_act[num_test - 1], CI, acMsg);
					format_activity_system(predicted_act[num_test - 1], acMsg);
					trim(acMsg);
					//strcpy(acMsg2, "Calc: ");
					get_amulet_message(L_CALC, acMsg2);    // "Calc: "
					strcat(acMsg2, acMsg);
					send_to_amulet_string(3*(num_test - 1) + 163, acMsg2);
				}
				SetAmuletByte(177, 0xFF);
				delayloop(70);
				SetAmuletByte(177, 0xFF);
				m_iPhase = PHASE_LINEARITY_STANDARD_WAIT_FOR_ACCEPT;
			}
			break;

		case PHASE_LINEARITY_STANDARD_WAIT_FOR_ACCEPT:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						// Changed:
						//act = measurement[ch_num].act;
						//kun = measurement[ch_num].kun;
						//act1 = act / unitfact[kun - 1];
						act1 = measurement[ch_num].act0;

						standlin[ch_num][num_test - 1].meas = act1;
						// Locked: CI
						//standlin[ch_num][num_test - 1].syst = current.system;
						// Removed:
						//standlin[ch_num][num_test - 1].syst = CI;
						// Locked: CI
						//format_activity(act1, current.system, standlin[ch_num][num_test - 1].actstr);
						//format_activity(act1, CI, standlin[ch_num][num_test - 1].actstr);
						format_activity_system(act1, standlin[ch_num][num_test - 1].actstr);
						read_clock(&standlin[ch_num][num_test - 1].date);
						strcpy(acMsg, standlin[ch_num][num_test - 1].actstr);
						trim(acMsg);
						strcpy(acMsg2, chamber[ch_num].nucdata.name);
						trim(acMsg2);
						strcat(acMsg2, " ");
						strcat(acMsg2, acMsg);
						send_to_amulet_string(3*(num_test - 1) + 164, acMsg2);
					}else{
						//SetAmuletString(3*(num_test - 1) + 164, "OVER RANGE");
						send_amulet_message(L_OVERRANGE, 3*(num_test - 1) + 164);    // "OVER RANGE"
						standlin[ch_num][num_test - 1].meas = 100.0;
					}
				}
			}
			break;

		case PHASE_LINEARITY_STANDARD_ACCEPTED:
			if(current.num_chambers>0){
				if(standlin[ch_num][num_test - 1].meas == 100.0){
					PopPageStack();
					get_amulet_message(L_STD_LINEARITY, titlestring);    // "Std Linearity"
					get_amulet_message(L_OVERRANGE_ERROR, messagestring);    // "OVER RANGE\nERROR"
					//Amulet_DisplayError("Std Linearity", "OVER RANGE\nERROR", TRUE);
					Amulet_DisplayError(titlestring, messagestring, TRUE);
					return;
				}else{
					standlin[ch_num][num_test - 1].meas_flag = 1;
					sprintf(acMsg, "%d)", num_test);
					dateout_language(acMsg2, &standlin[ch_num][num_test-1].date, 2);
					strcat(acMsg, acMsg2);
					strcat(acMsg, "; ");
					timeout(acMsg2, &standlin[ch_num][num_test-1].date);
					strcat(acMsg, acMsg2);
					send_to_amulet_string(3 * (num_test - 1) + 163, acMsg);
					delayloop(5);

					if(num_test == 1){
						strcpy(acMsg, "0.00");
						send_to_amulet_string(3*(num_test-1)+165, acMsg);
						delayloop(5);
						SetAmuletByte(183, 0xFF);
						delayloop(20);
						for(i=num_test+1; i<=num_meas; i++){
							LinearElapsedTime(i - 1, &def, standlin[ch_num][0].date, acMsg, acMsg2);
							sprintf(acMsg3, "%d)", i);
							strcat(acMsg3, acMsg2);
							send_to_amulet_string(3*(i - 1)+163, acMsg3);
							delayloop(5);
							send_to_amulet_string(3*(i - 1)+164, acMsg);
							delayloop(5);
							SetAmuletByte(i + 162, 0xFF);
							delayloop(20);
							SetAmuletByte(i + 182, 0xFF);
							delayloop(20);
						}
						SetAmuletByte(161, 0xFF);
					}else{
						predicted_standard_activity(predicted_act, num_test, &slope, &nucdata);
						// Removed: Units conversion is not necessary
						//meas0 = convact(standlin[ch_num][num_test-1].meas, standlin[ch_num][num_test-1].syst);
						meas0 = standlin[ch_num][num_test-1].meas;
						var = 100. * (meas0 - predicted_act[num_test-1]) / predicted_act[num_test-1];
						sprintf(acMsg, "%6.2f", var);
						trim(acMsg);
						send_to_amulet_string(3*(num_test-1)+165, acMsg);
						delayloop(5);
					}

					if(num_test==num_meas){
						//if(current.printer!=NONE_PRINTER) SetAmuletByte(100, 0xFF);
						m_iPhase = PHASE_LINEARITY_PRE_INIT;
					}else{
						m_iPhase = PHASE_LINEARITY_FINISH;
					}
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_INIT:
			if(current.num_chambers>0){
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				for(i=0; i<8; i++){
					//EE_READ(lindef.lin_cal_factors[i], (uchar *) &factors[i]);
					EE_READ(lindef.factors_Lin[ch_num][i], (uchar *) &factors[i]);
				}

				//num_meas = def.num;
				num_meas = def.num_Lin[ch_num];
				//nuc_index = def.nuc_index;
				nuc_index = def.nuc_index_Lin[ch_num];
				set_nuclide_data(nuc_index, ch_num);
				NuclideData_getNuclide(nuc_index, &nucdata);
				strcpy(nucname, nucdata.name);

				send_to_amulet_string(160, "#");
				delayloop(5);
				//SetAmuletString(161, "Factor");
				send_amulet_message(L_FACTOR, 161);    // "Factor"
				delayloop(5);
				//SetAmuletString(162, "% Ratio");
				send_amulet_message(L_RATIO, 162);    // "% Ratio"
				delayloop(5);
				SetAmuletByte(182, 0xFF);
				delayloop(20);

				//SetAmuletString(163, "1) Tube 1");
				send_amulet_message(L_TUBE_1_LINEATOR2, 163);    // "1) Tube 1"
				delayloop(5);
				SetAmuletByte(183, 0xFF);
				delayloop(20);
				//SetAmuletString(166, "2) Tube 1+2");
				send_amulet_message(L_TUBE_2_LINEATOR2, 166);    // "2) Tube 1+2"
				delayloop(5);
				SetAmuletByte(184, 0xFF);
				delayloop(20);
				//SetAmuletString(169, "3) Tube 1+3");
				send_amulet_message(L_TUBE_3_LINEATOR2, 169);    // "3) Tube 1+3"
				delayloop(5);
				SetAmuletByte(185, 0xFF);
				delayloop(20);
				//SetAmuletString(172, "4) Tube 1+2,3");
				send_amulet_message(L_TUBE_4_LINEATOR2, 172);    // "4) Tube 1+2,3"
				delayloop(5);
				SetAmuletByte(186, 0xFF);
				delayloop(20);
				//SetAmuletString(175, "5) Tube 1+4");
				send_amulet_message(L_TUBE_5_LINEATOR2, 175);    // "5) Tube 1+4"
				delayloop(5);
				SetAmuletByte(187, 0xFF);
				delayloop(20);
				//SetAmuletString(178, "6) Tube 1+2,4");
				send_amulet_message(L_TUBE_6_LINEATOR2, 178);    // "6) Tube 1+2,4"
				delayloop(5);
				SetAmuletByte(188, 0xFF);
				delayloop(20);
				//SetAmuletString(181, "7) Tube 1+3,4");
				send_amulet_message(L_TUBE_7_LINEATOR2, 181);    // "7) Tube 1+3,4"
				delayloop(5);
				SetAmuletByte(189, 0xFF);
				delayloop(20);
				//SetAmuletString(184, "8) Tube 1+2,3,4");
				send_amulet_message(L_TUBE_8_LINEATOR2, 184);    // "8) Tube 1+2,3,4"
				delayloop(5);
				SetAmuletByte(190, 0xFF);
				delayloop(50);
				SetAmuletByte(159, 0xFF);
				delayloop(50);
				num_test = 1;
				if(m_ucClear == 30){
					m_ucClear = 0;
					printer_pressed = FALSE;
					m_iPhase = PHASE_LINEARITY_LINEATOR_WAIT_FULL_SCREEN;
				}else{
					if(printer_pressed){
						printer_pressed = FALSE;
						wait = g_csec_tstamp + 20;
						m_iPhase = PHASE_LINEARITY_LINEATOR_DELAY_BEFORE_FINISH;
					}else{
						m_iPhase = PHASE_LINEARITY_LINEATOR_WAIT_FULL_SCREEN;
					}
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_WAIT_FULL_SCREEN:
			break;

		case PHASE_LINEARITY_LINEATOR_AFTER_FULL_SCREEN:
			if(current.num_chambers>0){
				SetAmuletWord(255, 532);
				SetAmuletWord(254, 39 * (num_test) + 41);
				//SetAmuletByte(179, 0xFF);
				SetAmuletByte(num_test + 162, 0xFF);
				delayloop(50);
				SetAmuletByte(179, 0xFF);
				delayloop(70);
				SetAmuletByte(179, 0xFF);
				m_iPhase = PHASE_LINEARITY_LINEATOR_WAIT_FOR_ACCEPT;
			}
			break;

		case PHASE_LINEARITY_LINEATOR_WAIT_FOR_ACCEPT:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						// Changed:
						//act = measurement[ch_num].act;
						//kun = measurement[ch_num].kun;
						//act1 = act / unitfact[kun - 1];
						act1 = measurement[ch_num].act0;

						if(num_test == 1){
							read_clock(&time0);
							corr_act = act1;
							linmeas[num_test - 1].meas_time = time0;
						}else{
							read_clock(&time1);
							linmeas[num_test - 1].meas_time = time1;
							corr_act = nucdecay(act1, time1, time0, nucdata.halflife, nucdata.hlunit);
						}

						linmeas[num_test - 1].meas_act = corr_act;
						linmeas[num_test - 1].factor = linmeas[0].meas_act / linmeas[num_test - 1].meas_act;
						linmeas[num_test - 1].pc_ratio = 100.0 * linmeas[num_test - 1].factor / factors[num_test - 1];
						// Locked: CI
						//format_activity(linmeas[num_test - 1].meas_act, current.system, linmeas[num_test - 1].act_str);
						//format_activity(linmeas[num_test - 1].meas_act, CI, linmeas[num_test - 1].act_str);
						format_activity_system(linmeas[num_test - 1].meas_act, linmeas[num_test - 1].act_str);
						strcpy(acMsg, linmeas[num_test - 1].act_str);
						trim(acMsg);
						strcpy(acMsg2, chamber[ch_num].nucdata.name);
						trim(acMsg2);
						strcat(acMsg2, " ");
						strcat(acMsg2, acMsg);
						send_to_amulet_string(3*(num_test - 1) + 164, acMsg2);
					}else{
						//SetAmuletString(3*(num_test - 1) + 164, "OVER RANGE");
						send_amulet_message(L_OVERRANGE, 3*(num_test - 1) + 164);    // "OVER RANGE"
						linmeas[num_test - 1].meas_act = 100.0;
					}
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_ACCEPTED:
			if(current.num_chambers>0){
				if(linmeas[num_test - 1].meas_act == 100.0){
					PopPageStack();
					get_amulet_message(L_LINEATOR, titlestring);    // "Lineator"
					get_amulet_message(L_OVERRANGE_ERROR, messagestring);    // "OVER RANGE\nERROR"
					//Amulet_DisplayError("Lineator", "OVER RANGE\nERROR", TRUE);
					Amulet_DisplayError(titlestring, messagestring, TRUE);
					return;
				}else{
					sprintf(acMsg, "%6.2f", linmeas[num_test - 1].factor);
					//sprintf(acMsg, "factor %d", num_test);
					send_to_amulet_string(3*(num_test - 1) + 164, acMsg);
					delayloop(5);
					strcpy(&(factor[num_test-1][0]),acMsg);

					sprintf(acMsg, "%6.2f", linmeas[num_test - 1].pc_ratio);
					//sprintf(acMsg, "ratio %d", num_test);
					send_to_amulet_string(3*(num_test - 1) + 165, acMsg);
					delayloop(5);
					strcpy(&(ratio[num_test-1][0]), acMsg);

					num_test++;
					if(num_test < 9){
						wait = g_csec_tstamp + 5;
						m_iPhase = PHASE_LINEARITY_LINEATOR_DELAY_AFTER_ACCEPT;
					}else{
						//if(current.printer!=NONE_PRINTER)
						if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
							SetAmuletByte(100, 0xFF);
						wait = g_csec_tstamp + 100;
						m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
					}
				}
			}
			break;

		case PHASE_LINEARITY_LINEATOR_DELAY_AFTER_ACCEPT:
			if(g_csec_tstamp > wait) m_iPhase = PHASE_LINEARITY_LINEATOR_AFTER_FULL_SCREEN;
			break;

		case PHASE_LINEARITY_STOP_UPDATING:
			if(g_csec_tstamp > wait){
				SetAmuletByte(101, 0xFF);
				delayloop(50);
				m_iPhase = PHASE_LINEARITY_FINISH;
			}
			break;

		case PHASE_LINEARITY_LINEATOR_DELAY_BEFORE_FINISH:
			if(g_csec_tstamp > wait){
				for(i=0;i<8;i++){
					send_to_amulet_string(3*i+164, &(factor[i][0]));
					delayloop(5);
					send_to_amulet_string(3*i+165, &(ratio[i][0]));
					delayloop(5);
					SetAmuletByte(i+163, 0xFF);
					delayloop(20);
				}
				//if(current.printer!=NONE_PRINTER)
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(100, 0xFF);
				wait = g_csec_tstamp + 100;
				m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
			}
			break;

		case PHASE_LINEARITY_CALICHECK_INIT:
			if(current.num_chambers>0){
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				//for(i=0; i<12; i++) EE_READ(lindef.lin_cal_factors[i],(uchar *)&factors[i]);
				for(i=0; i<12; i++) EE_READ(lindef.factors_Cali[ch_num][i],(uchar *)&factors[i]);

				//num_meas = def.num;
				num_meas = def.num_Cali[ch_num];
				nuc_index = def.nuc_index_Cali[ch_num];
				set_nuclide_data(nuc_index, ch_num);
				NuclideData_getNuclide(nuc_index, &nucdata);
				strcpy(nucname, nucdata.name);

				send_to_amulet_string(160, "#");
				delayloop(5);
				//SetAmuletString(161, "Result");
				send_amulet_message(L_RESULT, 161);    // "Result"
				delayloop(5);
				//SetAmuletString(162, "% Var");
				send_amulet_message(L_PERCENT_VAR, 162);    // "% Var"
				delayloop(5);
				SetAmuletByte(182, 0xFF);
				delayloop(20);
				switch(num_meas){
					case 12:
						//SetAmuletString(196, "12)Blk/Purple/Blue");
						send_amulet_message(L_CAL12, 196);    // "12)Blk/Purple/Blue"
						delayloop(5);
						SetAmuletByte(194, 0xFF);
						delayloop(20);
					case 11:
						//SetAmuletString(193, "11)Blk/Purple/Green");
						send_amulet_message(L_CAL11, 193);    // "11)Blk/Purple/Green"
						delayloop(5);
						SetAmuletByte(193, 0xFF);
						delayloop(20);
					case 10:
						//SetAmuletString(190, "10)Blk/Purpl/Yellow");
						send_amulet_message(L_CAL10, 190);    // "10)Blk/Purpl/Yellow"
						delayloop(5);
						SetAmuletByte(192, 0xFF);
						delayloop(20);
					case 9:
						//SetAmuletString(187, "9)Blk/Purple/Orange");
						send_amulet_message(L_CAL9, 187);    // "9)Blk/Purple/Orange"
						delayloop(5);
						SetAmuletByte(191, 0xFF);
						delayloop(20);
					case 8:
						//SetAmuletString(184, "8)Black/Purple/Red");
						send_amulet_message(L_CAL8, 184);    // "8)Black/Purple/Red"
						delayloop(5);
						SetAmuletByte(190, 0xFF);
						delayloop(20);
					case 7:
						//SetAmuletString(181, "7)Black/Purple");
						send_amulet_message(L_CAL7, 181);    // "7)Black/Purple"
						delayloop(5);
						SetAmuletByte(189, 0xFF);
						delayloop(20);
					case 6:
						//SetAmuletString(178, "6)Black/Blue");
						send_amulet_message(L_CAL6, 178);    // "6)Black/Blue"
						delayloop(5);
						SetAmuletByte(188, 0xFF);
						delayloop(20);
					case 5:
						//SetAmuletString(175, "5)Black/Green");
						send_amulet_message(L_CAL5, 175);    // "5)Black/Green"
						delayloop(5);
						SetAmuletByte(187, 0xFF);
						delayloop(20);
					case 4:
						//SetAmuletString(172, "4)Black/Yellow");
						send_amulet_message(L_CAL4, 172);    // "4)Black/Yellow"
						delayloop(5);
						SetAmuletByte(186, 0xFF);
						delayloop(20);
					case 3:
						//SetAmuletString(169, "3)Black/Orange");
						send_amulet_message(L_CAL3, 169);    // "3)Black/Orange"
						delayloop(5);
						SetAmuletByte(185, 0xFF);
						delayloop(20);
					case 2:
						//SetAmuletString(166, "2)Black/Red");
						send_amulet_message(L_CAL2, 166);    // "2)Black/Red"
						delayloop(5);
						SetAmuletByte(184, 0xFF);
						delayloop(20);
					case 1:
						//SetAmuletString(163, "1)Black");
						send_amulet_message(L_CAL1, 163);    // "1)Black"
						delayloop(5);
						SetAmuletByte(183, 0xFF);
						delayloop(20);
						break;
				}
				delayloop(50);
				SetAmuletByte(158, 0xFF);
				delayloop(50);
				num_test = 1;
				if(m_ucClear == 30){
					m_ucClear = 0;
					printer_pressed = FALSE;
					m_iPhase = PHASE_LINEARITY_CALICHECK_WAIT_FULL_SCREEN;
				}else{
					if(printer_pressed){
						printer_pressed = FALSE;
						wait = g_csec_tstamp + 20;
						m_iPhase = PHASE_LINEARITY_CALICHECK_DELAY_BEFORE_FINISH;
					}else{
						m_iPhase = PHASE_LINEARITY_CALICHECK_WAIT_FULL_SCREEN;
					}
				}
			}
			break;

		case PHASE_LINEARITY_CALICHECK_WAIT_FULL_SCREEN:
			break;

		case PHASE_LINEARITY_CALICHECK_AFTER_FULL_SCREEN:
			if(current.num_chambers>0){
				SetAmuletWord(255, 532);
				SetAmuletWord(254, 39 * (num_test) + 41);
				SetAmuletByte(num_test + 162, 0xFF);
				delayloop(50);
				SetAmuletByte(181, 0xFF);
				delayloop(70);
				SetAmuletByte(181, 0xFF);
				m_iPhase = PHASE_LINEARITY_CALICHECK_WAIT_FOR_ACCEPT;
			}
			break;

		case PHASE_LINEARITY_CALICHECK_WAIT_FOR_ACCEPT:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						// Changed:
						//act = measurement[ch_num].act;
						//kun = measurement[ch_num].kun;
						//act1 = act / unitfact[kun - 1];
						act1 = measurement[ch_num].act0;

						if(num_test == 1){
							read_clock(&time0);
							corr_act = act1;
							calmeas[num_test - 1].meas_time = time0;
						}else{
							read_clock(&time1);
							calmeas[num_test - 1].meas_time = time1;
							corr_act = nucdecay(act1, time1, time0, nucdata.halflife, nucdata.hlunit);
						}

						calmeas[num_test - 1].meas_act = corr_act;
						calmeas[num_test - 1].result = calmeas[num_test - 1].meas_act * factors[num_test - 1];

						// Locked: CI
						//format_activity(calmeas[num_test - 1].result, current.system, calmeas[num_test - 1 ].res_str);
						//format_activity(calmeas[num_test - 1].result, CI, calmeas[num_test - 1 ].res_str);
						format_activity_system(calmeas[num_test - 1].result, calmeas[num_test - 1 ].res_str);

						// Locked: CI
						//format_activity(calmeas[num_test - 1].meas_act, current.system, acMsg);
						//format_activity(calmeas[num_test - 1].meas_act, CI, acMsg);
						format_activity_system(calmeas[num_test - 1].meas_act, acMsg);

						trim(acMsg);
						strcpy(acMsg2, chamber[ch_num].nucdata.name);
						trim(acMsg2);
						strcat(acMsg2, " ");
						strcat(acMsg2, acMsg);
						strcpy(messagestring, "  ");
						strcat(messagestring, acMsg2);
						send_to_amulet_string(3*(num_test - 1) + 164, messagestring);
					}else{
						//SetAmuletString(3*(num_test - 1) + 164, "OVER RANGE");
						send_amulet_message(L_OVERRANGE, 3*(num_test - 1) + 164);    // "OVER RANGE"
						calmeas[num_test - 1].meas_act = 100.0;
					}
				}
			}
			break;

		case PHASE_LINEARITY_CALICHECK_ACCEPTED:
			if(current.num_chambers>0){
				if(calmeas[num_test - 1].meas_act == 100.0){
					PopPageStack();
					get_amulet_message(L_CALICHECK, titlestring);    // "Calicheck"
					get_amulet_message(L_OVERRANGE_ERROR, messagestring);    // "OVER RANGE\nERROR"
					//Amulet_DisplayError("Calicheck", "OVER RANGE\nERROR", TRUE);
					Amulet_DisplayError(titlestring, messagestring, TRUE);
					return;
				}else{
					mean = 0.0;
					for(i=0; i < num_test; i++){
						mean += calmeas[i].result;
					}
					mean /= (float) num_test;
					for(i=0; i < num_test; i++){
						calmeas[i].var = 100.0 * (calmeas[i].result - mean) / mean;
					}

					for(i=0; i < num_test; i++){
						strcpy(acMsg, calmeas[i].res_str);
						trim(acMsg);
						strcpy(&(caliresult[i][0]),acMsg);
						strcpy(messagestring, "  ");
						strcat(messagestring, acMsg);
						send_to_amulet_string(3*i + 164, messagestring);
						delayloop(5);
						sprintf(acMsg, "%5.2f", calmeas[i].var);
						strcpy(&(calivar[i][0]), acMsg);
						send_to_amulet_string(3*i + 165, acMsg);
						delayloop(5);
					}

					num_test++;
					if(num_test < num_meas + 1){
						wait = g_csec_tstamp + 5;
						m_iPhase = PHASE_LINEARITY_CALICHECK_DELAY_AFTER_ACCEPT;
					}else{
						//if(current.printer!=NONE_PRINTER)
						if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
							SetAmuletByte(100, 0xFF);
						wait = g_csec_tstamp + 100;
						m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
					}
				}
			}
			break;

		case PHASE_LINEARITY_CALICHECK_DELAY_AFTER_ACCEPT:
			if(g_csec_tstamp > wait) m_iPhase = PHASE_LINEARITY_CALICHECK_AFTER_FULL_SCREEN;
			break;

		case PHASE_LINEARITY_CALICHECK_DELAY_BEFORE_FINISH:
			if(g_csec_tstamp > wait){
				for(i=0; i<num_meas; i++){
					send_to_amulet_string(3*i + 164, &(caliresult[i][0]));
					delayloop(5);
					send_to_amulet_string(3*i + 165, &(calivar[i][0]));
					delayloop(5);
					SetAmuletByte(i+163, 0xFF);
					delayloop(20);
				}
				//if(current.printer != NONE_PRINTER)
				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
					SetAmuletByte(100, 0xFF);
				wait = g_csec_tstamp + 100;
				m_iPhase = PHASE_LINEARITY_STOP_UPDATING;
			}
			break;

		case PHASE_LINEARITY_CLEAR_MEASUREMENTS:
			if(current.num_chambers>0){
				for(i = 0; i < num_meas; i++)
					standlin[ch_num][i].meas_flag = 0;

				if(num_meas < 12){
					for(i = num_meas; i < 12; i++)
						standlin[ch_num][i].meas_flag = -1;
				}

				for(i=160; i<199; i++){
					send_to_amulet_string(i, "");
					delayloop(5);
				}
				delayloop(20);
				SetAmuletByte(176, 0xFF);
				delayloop(20);
				SetAmuletByte(178, 0xFF);
				delayloop(50);
				m_iPhase = PHASE_LINEARITY_PRE_INIT;
			}
			break;

		case PHASE_LINEARITY_PRINT:
			m_iPhase = PHASE_LINEARITY_FINISH;
			printer_pressed = TRUE;
			beep_amulet();
			switch(linearity_type){
				case STANDARD:
					predicted_standard_activity(predicted_act, num_test, &slope, &nucdata);
					Amulet_printLinearityStd(predicted_act, num_test, nucname);
					break;

				case LINEATOR:
					Amulet_printLinearityLin(linmeas, &def);
					break;

				case CALICHECK:
					Amulet_printLinearityCali(calmeas, &def);
					break;

			}
			SetAmuletByte(102, 0xFF);
			break;

		case PHASE_LINEARITY_FINISH:
			printer_pressed = FALSE;
			break;

		default:
			break;
	}
}
