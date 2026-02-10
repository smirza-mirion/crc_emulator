/**
 * \file
 * \details This file handles calls from the Amulet Geometry Test Screen
 */
#define PHASE_GEOMETRY_BEFORE_INIT 0
#define PHASE_GEOMETRY_NO_CAL_NUM 1
#define PHASE_GEOMETRY_WAIT_FOR_CONTAINER 2
#define PHASE_GEOMETRY_SYRINGE_SELECTED 3
#define PHASE_GEOMETRY_VIAL_SELECTED 4
#define PHASE_GEOMETRY_BEFORE_INIT_VOL 5
#define PHASE_GEOMETRY_WAITING_FOR_INIT_VOL 6
#define PHASE_GEOMETRY_INIT_MEASUREMENT 7
#define PHASE_GEOMETRY_WAIT_MEASUREMENT 8
#define PHASE_GEOMETRY_SAVE_INITIAL_MEASUREMENT 9
#define PHASE_GEOMETRY_WAIT_FOR_ADD_VOL 10
#define PHASE_GEOMETRY_ADD_MEASUREMENT 11
#define PHASE_GEOMETRY_SAVE_ADD_MEASUREMENT 12
#define PHASE_GEOMETRY_ADDITIONAL_TESTS_QUESTION 13
#define PHASE_GEOMETRY_ADDITIONAL_TESTS_YES 14
#define PHASE_GEOMETRY_REPORT_INIT 15
#define PHASE_GEOMETRY_FINISHED 16

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "message.h"
#include "amulet.h"
#include "chambfac.h"
#include "nuc.h"
#include "i2c.h"

typedef struct geomeas GEOMEAS;
struct geomeas{
	float meas_act;
	float totvol;
	char act_str[10];
	float var;
};
extern GEOMEAS geomeas[10];
extern CURRENT  current;
extern int m_iPhase;
extern char Geometry_acContainer[8];
extern int Geometry_nmeas;
extern char m_cKeypress[2];
extern char m_cKeystring[100];
extern CHAMBER chamber[];
extern volatile bool DisplayActivity_SendActivity;
extern MEASUREMENT measurement[];

void send_to_amulet_string(uchar ucIndex, char message0[]);
bool new_cal_exist(short nuc_index);
unsigned char IsNumber(char *ptr);
void trim(char *acByte);
void trim_and_shrink(char *acByte);
void GetExtendedNuclideString(unsigned char ucNuclideID, char *acMsg);
unsigned char PopPageStack(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

/**
 * \details Handles the Amulet Screen Geometry.htm. Geometry.htm runs the geometry test.
 * \param Amulet_Byte_ID Description
 * \param 20 Show Chamber Label (TOGGLE)
 * \param 80 Language (STATE) English = 0, Spanish = 1
 * \param 189 0xFF = Displays "No Cal Number" error
 * \param 190 0xFF = Displays the Container Label, radio button Syringe and radio button Vial
 * \param 191 0xFF = Displays Volume Range Error
 * \param 192 0xFF = Hides Volume Range Error
 * \param 193 0xFF = Hides Keypad and Display Initial Volume Result
 * \param 194 0xFF = Show Added Volume Keypad, enable Initial Activity Result
 * \param 195 0xFF = Hides Keypad and Display Added Volume Result.
 * \param 196 0xFF = Show 'Another Volume', 'Finished' buttons and enable Added Activity Result.
 * \param 197 0xFF = Get Additional Measurement
 * \param 198 0xFF = Show 'Finished' button and enable Add Activity Result
 * \param Amulet_Word_ID Description
 * \param 224 Error Y offset
 * \param 225 Error X offset
 * \param 226 Keypad Y offset
 * \param 227 Keypad X offset
 * \param 228 Button Enter Y offset
 * \param 229 Button Enter X offset
 * \param 230 Button Backspace Y offset
 * \param 231 Button Backspace X offset
 * \param 232 Button Dot Y offset
 * \param 233 Button Dot X offset
 * \param 234 Button 0 Y offset
 * \param 235 Button 0 X offset
 * \param 236 Button 3 Y offset
 * \param 237 Button 3 X offset
 * \param 238 Button 2 Y offset
 * \param 239 Button 2 X offset
 * \param 240 Button 1 Y offset
 * \param 241 Button 1 X offset
 * \param 242 Button 6 Y offset
 * \param 243 Button 6 X offset
 * \param 244 Button 5 Y offset
 * \param 245 Button 5 X offset
 * \param 246 Button 4 Y offset
 * \param 247 Button 4 X offset
 * \param 248 Button 9 Y offset
 * \param 249 Button 9 X offset
 * \param 250 Button 8 Y offset
 * \param 251 Button 8 X offset
 * \param 252 Button 7 Y offset
 * \param 253 Button 7 X offset
 * \param 254 Keypad display Y offset
 * \param 255 Keypad display X offset
 * \param Amulet_String_ID Description
 * \param 5 Initial Volume
 * \param 9 Chamber Info
 * \param 70_71 Title
 * \param 72 1) Container
 * \param 73_74 2) Initial Volume
 * \param 75 Initial Measurement
 * \param 76_77 4) Added Volume
 * \param 78_79 4) Volume
 * \param 80 Measurement
 * \param 81_82 CAL # NOT ENTERED FOR NUCLIDE
 * \param 83 BackSP
 * \param 84 Accept
 * \param 85_86 More Measurements
 * \param 87 Finished
 * \param 88 Syringe
 * \param 89 Vial
 * \param 189 Label of the selected container (Syringe or Vial)
 * \param 190 Running activity for measurement
 * \param 191 Result of initial measurement
 * \param 192_193_194 Keypad Error
 * \param 195_196_197 Added Volume String
 * \returns None
 */
void AmuletGeometry_menu(void) {
	short ch_num = current.main_chamber;
	short ch_type;
	char acMsg[100];
	char acMsg2[100];
	int i;
	bool nucok;
	bool ok;
	float fTestVol;
	float fTotalVol;
	//char kun;
	float act1;
	float corr_act;
	//float ref_act;
	char titlestring[51];
	char formatstring[100];

	static time_t time0;
    static time_t time1;
    static char nucname[8];
    static short nuc_index;
    static NUCDATA nucdata;
//    static float response;
//    static short original_nuc_index;
    static float fCurrentVol;
    static float fResponse;
    char nucshrink[25];
    char message[51];

	if(current.num_chambers>0) ch_type = chamber_type(ch_num);

	switch(m_iPhase) {
		case PHASE_GEOMETRY_BEFORE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_GEOMETRY, 70);    // "Geometry"
			send_amulet_message(L_CONTAINER, 72);    // "1) Container"
			send_amulet_message(L_INITIAL_VOLUME, 73);    // "2) Initial Volume"
			send_amulet_message(L_INITIAL_MEASUREMENT, 75);    // "Initial Measurement"
			send_amulet_message(L_ADDED_VOLUME, 76);    // "4) Added Volume"
			send_amulet_message(L_VOLUME2, 78);    // "4) Volume"
			send_amulet_message(L_MEASUREMENT, 80);    // "Measurement"
			send_amulet_message(L_CAL_NOT_ENTERED_FOR_NUCLIDE, 81);    // "CAL # NOT ENTERED FOR Tc99m"
			send_amulet_message(L_BACKSPACE_SHORT, 83);    // "BackSP"
			send_amulet_message(L_ACCEPT_BUTTON, 84);    // "Accept"
			send_amulet_message(L_MORE_MEASUREMENTS, 85);    // "More Measurements"
			send_amulet_message(L_FINISHED, 87);    // "Finished"
			send_amulet_message(L_SYRINGE, 88);    // "Syringe"
			send_amulet_message(L_VIAL, 89);    // "Vial"
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

				for(i=0; i<10; i++){
					geomeas[i].totvol = -1;
					geomeas[i].var = -1;
				}

				strcpy(nucname,"Tc99m");
				nuc_index = NuclideData_getIndexFromName(nucname);
				nucok = TRUE;

				NuclideData_getNuclide(nuc_index,&nucdata);
				fResponse = NuclideData_getEffectiveResponse(nuc_index, ch_type);

				//ok = new_cal_exist(nuc_index);
				//if(!ok && fResponse == 0.0){
				if(fResponse == 0.0){
					nucok = FALSE;
				}

				if(nucok){
					SetAmuletByte(190, 0xFF);
					m_iPhase = PHASE_GEOMETRY_WAIT_FOR_CONTAINER;
				}else{
					SetAmuletByte(189, 0xFF);
					m_iPhase = PHASE_GEOMETRY_NO_CAL_NUM;
				}
			}
			break;

		case PHASE_GEOMETRY_NO_CAL_NUM:
			break;

		case PHASE_GEOMETRY_WAIT_FOR_CONTAINER:
			// Wait for Container Selection
			break;

		case PHASE_GEOMETRY_SYRINGE_SELECTED:
			if(current.num_chambers>0){
				strcpy(Geometry_acContainer, "Syringe");
				m_iPhase = PHASE_GEOMETRY_BEFORE_INIT_VOL;
			}
			break;

		case PHASE_GEOMETRY_VIAL_SELECTED:
			if(current.num_chambers>0){
				strcpy(Geometry_acContainer, "Vial");
				m_iPhase = PHASE_GEOMETRY_BEFORE_INIT_VOL;
			}
			break;

		case PHASE_GEOMETRY_BEFORE_INIT_VOL:
			if(current.num_chambers>0){
				Geometry_nmeas = 0;
				m_cKeypress[0] = 0;
				m_cKeypress[1] = 0;
				m_cKeystring[0] = 0;
				//SetAmuletString(192, "Valid range is 98.9ml to 0.1ml");
				send_amulet_message(L_VALID_RANGE_FOR_GEOMETRY, 192);    // "Valid range is 98.9ml to 0.1ml"
				m_iPhase = PHASE_GEOMETRY_WAITING_FOR_INIT_VOL;
			}
			break;

		case PHASE_GEOMETRY_WAITING_FOR_INIT_VOL:
			if(current.num_chambers>0){
				//Process Keypress
				if(m_cKeypress[0] != 0){
					switch(m_cKeypress[0]){
						case '9':
						case '8':
						case '7':
						case '6':
						case '5':
						case '4':
						case '3':
						case '2':
						case '1':
						case '0':
							if((m_cKeypress[0]!='0') || (strlen(m_cKeystring)!=0)){
								if(strlen(m_cKeystring) < 10){
									if(strchr(m_cKeystring, '.') == NULL){
										strcat(m_cKeystring, m_cKeypress);
									}else{
										if((strlen(m_cKeystring) - ((long)(strchr(m_cKeystring, '.') - m_cKeystring)) - 1) < 3){
											strcat(m_cKeystring, m_cKeypress);
										}
									}
								}
							}
							break;

						case '.':
							if(strchr(m_cKeystring,'.') == NULL){
								if(strlen(m_cKeystring) < 10){
									strcat(m_cKeystring, m_cKeypress);
								}
							}
							break;

						case 8:
							if (strlen(m_cKeystring)!= 0){
								m_cKeystring[strlen(m_cKeystring) - 1] = 0;
							}
							break;

						case 13:
							if(IsNumber(m_cKeystring)){
								fTestVol = atof(m_cKeystring);
								if((fTestVol < 0.1) || (fTestVol > 98.9)){
									if((strcmp(m_cKeystring, "98.9") == 0) || (strcmp(m_cKeystring, "98.90") == 0) || (strcmp(m_cKeystring, "98.900") == 0)){
										geomeas[Geometry_nmeas].totvol = fTestVol;
										SetAmuletByte(193, 0xFF);
										m_iPhase = PHASE_GEOMETRY_INIT_MEASUREMENT;
									}
								}else{
									geomeas[Geometry_nmeas].totvol = fTestVol;
									SetAmuletByte(193, 0xFF);
									m_iPhase = PHASE_GEOMETRY_INIT_MEASUREMENT;
								}
							}
							break;

						default:
							break;
					}
					if(m_cKeypress[0] != 13){
						send_to_amulet_string(5, m_cKeystring);
						if(IsNumber(m_cKeystring)){
							fTestVol = atof(m_cKeystring);
							if((fTestVol < 0.1) || (fTestVol > 98.9)){
								if((strcmp(m_cKeystring, "98.9") == 0) || (strcmp(m_cKeystring, "98.90") == 0) || (strcmp(m_cKeystring, "98.900") == 0)){
									SetAmuletByte(192, 0xFF);
								}else{
									SetAmuletByte(191, 0xFF);
								}
							}else{
								SetAmuletByte(192, 0xFF);
							}
						}else{
							SetAmuletByte(192, 0xFF);
						}
					}
					m_cKeypress[0] = 0;
				}
			}
			break;

		case PHASE_GEOMETRY_INIT_MEASUREMENT:
			if(current.num_chambers>0){
				chamber[ch_num].nuc_index = nuc_index;
				set_nuclide_data(nuc_index, ch_num);
				strcpy(nucshrink, chamber[ch_num].nucdata.name);
				trim_and_shrink(nucshrink);
				if(DisplayActivity_SendActivity){
					if(nucshrink[0]!=0){
						GetExtendedNuclideString(chamber[ch_num].nuc_index, message);
						send_to_amulet_string(0, message);
					}
					else send_to_amulet_string(0, "");
					//else send_to_amulet_string(0, "*****");
				}
				m_iPhase = PHASE_GEOMETRY_WAIT_MEASUREMENT;
			}
			break;

		case PHASE_GEOMETRY_WAIT_MEASUREMENT:
			// Wait for accept
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						read_clock(&time0);
						// Changed:
						//kun = measurement[ch_num].kun;
						//act1 = measurement[ch_num].act / unitfact[kun - 1];
						act1 = measurement[ch_num].act0;
						geomeas[Geometry_nmeas].meas_act = act1;
						// Locked: CI
						//format_activity(act1, current.system, geomeas[Geometry_nmeas].act_str);
						//format_activity(act1, CI, geomeas[Geometry_nmeas].act_str);
						format_activity_system(act1, geomeas[Geometry_nmeas].act_str);
						strcpy(acMsg, geomeas[Geometry_nmeas].act_str);
						trim(acMsg);
						strcpy(acMsg2, chamber[ch_num].nucdata.name);
						strcat(acMsg2, " ");
						strcat(acMsg2, acMsg);
						send_to_amulet_string(190, acMsg2);
					}else{
						//SetAmuletString(190, "OVER RANGE");
						send_amulet_message(L_OVERRANGE, 190);    // "OVER RANGE"
						geomeas[Geometry_nmeas].meas_act = 100.0;
					}
				}
			}
			break;

		case PHASE_GEOMETRY_SAVE_INITIAL_MEASUREMENT:
			if(current.num_chambers>0){
				if(geomeas[Geometry_nmeas].meas_act == 100.0){
					PopPageStack();
					get_amulet_message(L_GEOMETRY, titlestring);    // "Geometry"
					get_amulet_message(L_OVERRANGE_ERROR, message);    // "OVER RANGE\nERROR"
					Amulet_DisplayError(titlestring, message, TRUE);
					return;
				}else{
					strcpy(acMsg, geomeas[Geometry_nmeas].act_str);
					trim(acMsg);
					strcpy(acMsg2, chamber[ch_num].nucdata.name);
					strcat(acMsg2, " ");
					strcat(acMsg2, acMsg);
					send_to_amulet_string(191, acMsg2);
					SetAmuletByte(194,0xFF);
					Geometry_nmeas++;
					m_cKeypress[0] = 0;
					m_cKeypress[1] = 0;
					m_cKeystring[0] = 0;
					fCurrentVol = geomeas[Geometry_nmeas - 1].totvol;
					get_amulet_message(L_VALID_RANGE_IS_ML_TO_ML, formatstring);    // "Valid range is %.3fml to 0.1ml"
					sprintf(acMsg, formatstring, 99.0 - fCurrentVol);
					send_to_amulet_string(192, acMsg);
					send_to_amulet_string(190, "");
					m_iPhase = PHASE_GEOMETRY_WAIT_FOR_ADD_VOL;
				}
			}
			break;

		case PHASE_GEOMETRY_WAIT_FOR_ADD_VOL:
			//Process Keypress
			if(current.num_chambers>0){
				if(m_cKeypress[0] != 0){
					switch(m_cKeypress[0]){
						case '9':
						case '8':
						case '7':
						case '6':
						case '5':
						case '4':
						case '3':
						case '2':
						case '1':
						case '0':
							if((m_cKeypress[0]!='0') || (strlen(m_cKeystring)!=0)){
								if(strlen(m_cKeystring) < 10){
									if(strchr(m_cKeystring, '.') == NULL){
										strcat(m_cKeystring, m_cKeypress);
									}else{
										if((strlen(m_cKeystring) - ((long)(strchr(m_cKeystring, '.') - m_cKeystring)) - 1) < 3){
											strcat(m_cKeystring, m_cKeypress);
										}
									}
								}
							}
							break;

						case '.':
							if(strchr(m_cKeystring,'.') == NULL){
								if(strlen(m_cKeystring) < 10){
									strcat(m_cKeystring, m_cKeypress);
								}
							}
							break;

						case 8:
							if(strlen(m_cKeystring)!= 0){
								m_cKeystring[strlen(m_cKeystring) - 1] = 0;
							}
							break;

						case 13:
							if(IsNumber(m_cKeystring)){
								fTestVol = atof(m_cKeystring);
								fTotalVol = fTestVol + fCurrentVol;
								if((fTestVol >= 0.1) && (fTotalVol <= 99.0)){
									geomeas[Geometry_nmeas].totvol = fTotalVol;
									get_amulet_message(L_ADDED_VOL_TOTAL_VOL, formatstring);    // "Added Vol: %.3fml\nTotal Vol: %.3fml"
									sprintf(acMsg, formatstring, fTestVol, fTotalVol);
									send_to_amulet_string(195, acMsg);
									SetAmuletByte(195, 0xFF);
									m_iPhase = PHASE_GEOMETRY_ADD_MEASUREMENT;
								}
							}
							break;

						default:
							break;
					}
					if(m_cKeypress[0] != 13){
						send_to_amulet_string(5, m_cKeystring);
						if(IsNumber(m_cKeystring)){
							fTestVol = atof(m_cKeystring);
							fTotalVol = fTestVol + fCurrentVol;
							if((fTestVol < 0.1) || (fTotalVol > 99.0)){
								SetAmuletByte(191, 0xFF);
							}else{
								SetAmuletByte(192, 0xFF);
							}
						}else{
							SetAmuletByte(192, 0xFF);
						}
					}
					m_cKeypress[0] = 0;
				}
			}
			break;

		case PHASE_GEOMETRY_ADD_MEASUREMENT:
			if(current.num_chambers>0){
				if(measurement[ch_num].display_flag_2){
					activity_to_screen();
					measurement[ch_num].display_flag_2 = FALSE;
					if(!measurement[ch_num].over_flag){
						// Changed:
						//kun = measurement[ch_num].kun;
						//act1 = measurement[ch_num].act / unitfact[kun - 1];
						act1 = measurement[ch_num].act0;
						read_clock(&time1);
						corr_act = nucdecay(act1, time1, time0, nucdata.halflife, nucdata.hlunit);
						geomeas[Geometry_nmeas].meas_act = corr_act;
						// Locked: CI
						//format_activity(act1, current.system, geomeas[Geometry_nmeas].act_str);
						//format_activity(act1, CI, geomeas[Geometry_nmeas].act_str);
						format_activity_system(act1, geomeas[Geometry_nmeas].act_str);
						strcpy(acMsg, geomeas[Geometry_nmeas].act_str);
						trim(acMsg);
						strcpy(acMsg2, chamber[ch_num].nucdata.name);
						strcat(acMsg2, " ");
						strcat(acMsg2, acMsg);
						send_to_amulet_string(190, acMsg2);
					}else{
						//SetAmuletString(190, "OVER RANGE");
						send_amulet_message(L_OVERRANGE, 190);    // "OVER RANGE"
						geomeas[Geometry_nmeas].meas_act = 100.0;
					}
				}
			}
			break;


		case PHASE_GEOMETRY_SAVE_ADD_MEASUREMENT:
			if(current.num_chambers>0){
				if(geomeas[Geometry_nmeas].meas_act == 100.0){
					PopPageStack();
					get_amulet_message(L_GEOMETRY, titlestring);    // "Geometry"
					get_amulet_message(L_OVERRANGE_ERROR, message);    // "OVER RANGE\nERROR"
					Amulet_DisplayError(titlestring, message, TRUE);
					return;
				}else{
					strcpy(acMsg, geomeas[Geometry_nmeas].act_str);
					trim(acMsg);
					strcpy(acMsg2, chamber[ch_num].nucdata.name);
					strcat(acMsg2, " ");
					strcat(acMsg2, acMsg);
					send_to_amulet_string(191, acMsg2);
					Geometry_nmeas++;

					if((Geometry_nmeas >= 10) || (geomeas[Geometry_nmeas - 1].totvol >= 99.0)) SetAmuletByte(198, 0xFF);
					else SetAmuletByte(196, 0xFF);

					m_iPhase = PHASE_GEOMETRY_ADDITIONAL_TESTS_QUESTION;
				}
			}
			break;

		case PHASE_GEOMETRY_ADDITIONAL_TESTS_QUESTION:
			break;

		case PHASE_GEOMETRY_ADDITIONAL_TESTS_YES:
			if(current.num_chambers>0){
				m_cKeypress[0] = 0;
				m_cKeypress[1] = 0;
				m_cKeystring[0] = 0;
				fCurrentVol = geomeas[Geometry_nmeas - 1].totvol;
				get_amulet_message(L_VALID_RANGE_IS_ML_TO_ML, formatstring);    // "Valid range is %.3fml to 0.1ml"
				sprintf(acMsg, formatstring, 99.0 - fCurrentVol);
				send_to_amulet_string(192, acMsg);
				send_to_amulet_string(190, "");
				SetAmuletByte(197, 0xFF);
				m_iPhase = PHASE_GEOMETRY_WAIT_FOR_ADD_VOL;
			}
			break;

//		case PHASE_GEOMETRY_REPORT_INIT:
//			if(current.num_chambers>0){
//				ref_act = geomeas[0].meas_act;
//				for(i=0; i<Geometry_nmeas; i++){
//					geomeas[i].var = 100 * ((geomeas[i].meas_act - ref_act)/ref_act);
//				}
//				//strcpy(acMsg, Geometry_acContainer);
//				//strcat(acMsg, " Assay");
//				if(!strcmp(Geometry_acContainer, "Syringe")) get_amulet_message(L_SYRINGE_ASSAY, acMsg);    // "Syringe Assay"
//				else if(!strcmp(Geometry_acContainer, "Vial")) get_amulet_message(L_VIAL_ASSAY, acMsg);    // "Vial Assay"
//				send_to_amulet_string(168, acMsg);
//				SetAmuletByte(169, 0xFF);
//				for(i=0; i<Geometry_nmeas; i++){
//					sprintf(acMsg, "%.3fml",geomeas[i].totvol);
//					send_to_amulet_string((3*i)+0+169, acMsg);
//					send_to_amulet_string((3*i)+1+169, geomeas[i].act_str);
//
//					if(i==0){
//						//strcpy(acMsg, "BASE");
//						get_amulet_message(L_BASE, acMsg);    // "BASE"
//					}else{
//						sprintf(acMsg, "%.1f", geomeas[i].var);
//					}
//					send_to_amulet_string((3*i)+2+169, acMsg);
//					SetAmuletByte(i+170, 0xFF);
//				}
//				SetAmuletByte(168, 0xFF);
//				m_iPhase = PHASE_GEOMETRY_FINISHED;
//			}
//			break;

		case PHASE_GEOMETRY_FINISHED:
			break;

		default:
			break;
	}
}
