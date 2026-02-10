#define PHASE_MEASURE_PRE_INIT	0
#define PHASE_MEASURE_PAUSE		1
#define PHASE_MEASURE_WAIT		2
#define PHASE_MEASURE_ACCEPT	3

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "crc.h"
#include "message.h"
#include "daily.h"
#include "Amulet.h"
#include "i2c.h"
#include "nuc.h"

extern int m_iPhase;
extern CURRENT  current;
extern unsigned char m_ucSetMeasure;
extern unsigned char m_ucAddInventory_Nuclide;
extern short m_iWithdrawInventory_NucIndex;
extern short m_iKitInventory_NucIndex;
extern volatile char m_acTitle[52];
extern short AmuletSetupLinearityLineatorMenu_nuclideIndex;
extern short AmuletSetupLinearityCalicheckMenu_nuclideIndex;
extern short AmuletDoseTableMenu_nuclideID;
extern ACCDATA acc_data[];
extern CHAMBER chamber[];
extern CHAMBERVALS chamb_vals[];
extern MEASUREMENT measurement[];
extern float m_fAddInventory_Activity;
extern time_t m_dtmAddInventory_Date;
extern unsigned char m_ucAddInventory_MeasuredNuclide;
extern float m_fWithdrawInventory_MeasuredActivity;
extern time_t m_dtmWithdrawInventory_MeasuredDate;
extern unsigned char m_ucWithdrawInventory_MeasuredNuclide;
extern float m_fKitInventory_MeasuredActivity;
extern time_t m_dtmKitInventory_MeasuredDate;
extern unsigned char m_ucKitInventory_MeasuredNuclide;
extern ACTIVITY_MEASUREMENT AmuletSetupLinearityLineatorMenu_measurement[8];
extern ACTIVITY_MEASUREMENT AmuletSetupLinearityCalicheckMenu_measurement[12];
extern float AmuletDoseTableMenu_activity;
extern time_t AmuletDoseTableMenu_timestamp;
extern time_t clock_time;

void trim_and_shrink(char *acByte);
void trim(char *acByte);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void SetAmuletBackHTML(void);

void AmuletMeasure_menu(void) {
	static short iNucIndex;
	short ch_num = current.main_chamber;
	char acMsg[100];
	char acMsg2[100];
	char formatstring[100];
	char nucName[8];
	static bool flgOK;
	static unsigned long int delaystamp;
	static time_t time0;
	static float act0;
	float diff, pract;
	static bool over_flow;
	static short message_color; // 0=unassigned, 1=white, 2=red
	static MEASUREMENT last_measurement;
	//static char kun;

	switch(m_iPhase) {
		case PHASE_MEASURE_PRE_INIT:
			if(current.num_chambers>0){
				send_amulet_message(L_ACCEPT_BUTTON, 108);    // "Accept"
				send_amulet_message(L_CANCEL_BUTTON, 109);    // "Cancel"

				switch(m_ucSetMeasure) {
					case 1:
						iNucIndex = m_ucAddInventory_Nuclide;
						break;

					case 2:
						iNucIndex = m_iWithdrawInventory_NucIndex;
						break;

					case 3:
						iNucIndex = m_iKitInventory_NucIndex;
						break;

					case 4:
						get_amulet_message(L_TUBE_1_LINEATOR, (char *) m_acTitle);    // "Tube #1: (1)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 5:
						get_amulet_message(L_TUBE_2_LINEATOR, (char *) m_acTitle);    // "Tube #2: (1 + 2)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 6:
						get_amulet_message(L_TUBE_3_LINEATOR, (char *) m_acTitle);    // "Tube #3: (1 + 3)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 7:
						get_amulet_message(L_TUBE_4_LINEATOR, (char *) m_acTitle);    // "Tube #4: (1 + 2 + 3)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 8:
						get_amulet_message(L_TUBE_5_LINEATOR, (char *) m_acTitle);    // "Tube #5: (1 + 4)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 9:
						get_amulet_message(L_TUBE_6_LINEATOR, (char *) m_acTitle);    // "Tube #6: (1 + 2 + 4)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 10:
						get_amulet_message(L_TUBE_7_LINEATOR, (char *) m_acTitle);    // "Tube #7: (1 + 3 + 4)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 11:
						get_amulet_message(L_TUBE_8_LINEATOR, (char *) m_acTitle);    // "Tube #8: (1 + 2 + 3 + 4)"
						iNucIndex = AmuletSetupLinearityLineatorMenu_nuclideIndex;
						break;

					case 12:
						get_amulet_message(L_TUBE_1_CALICHECK, (char *) m_acTitle);    // "Tube 1: Black"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 13:
						get_amulet_message(L_TUBE_2_CALICHECK, (char *) m_acTitle);    // "Tube 2: Black + Red"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 14:
						get_amulet_message(L_TUBE_3_CALICHECK, (char *) m_acTitle);    // "Tube 3: Black + Orange"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 15:
						get_amulet_message(L_TUBE_4_CALICHECK, (char *) m_acTitle);    // "Tube 4: Black + Yellow"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 16:
						get_amulet_message(L_TUBE_5_CALICHECK, (char *) m_acTitle);    // "Tube 5: Black + Green"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 17:
						get_amulet_message(L_TUBE_6_CALICHECK, (char *) m_acTitle);    // "Tube 6: Black + Blue"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 18:
						get_amulet_message(L_TUBE_7_CALICHECK, (char *) m_acTitle);    // "Tube 7: Black + Purple"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 19:
						get_amulet_message(L_TUBE_8_CALICHECK, (char *) m_acTitle);    // "Tube 8: Black + Purple + Red"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 20:
						get_amulet_message(L_TUBE_9_CALICHECK, (char *) m_acTitle);    // "Tube 9: Black + Purple + Orange"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 21:
						get_amulet_message(L_TUBE_10_CALICHECK, (char *) m_acTitle);    // "Tube 10: Black + Purple + Yellow"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 22:
						get_amulet_message(L_TUBE_11_CALICHECK, (char *) m_acTitle);    // "Tube 11: Black + Purple + Green"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 23:
						get_amulet_message(L_TUBE_12_CALICHECK, (char *) m_acTitle);    // "Tube 12: Black + Purple + Blue"
						iNucIndex = AmuletSetupLinearityCalicheckMenu_nuclideIndex;
						break;

					case 24:
						iNucIndex = AmuletDoseTableMenu_nuclideID;
						break;

					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
					case 30:
						get_amulet_message(L_MEASURE_TEST_SOURCE, (char *) m_acTitle);    // "Measure test source, "
						strcpy(acMsg, acc_data[m_ucSetMeasure - 25].nucname);
						trim_and_shrink(acMsg);
						strcat((char *)m_acTitle, acMsg);
						get_amulet_message(L_MEASURE_TEST_SOURCE_SN, formatstring);    // ", S/N: "
						strcat((char *)m_acTitle, formatstring);
						strcpy(acMsg, acc_data[m_ucSetMeasure - 25].sn);
						trim(acMsg);
						strcat((char *)m_acTitle, acMsg);
						iNucIndex = acc_data[m_ucSetMeasure - 25].nuc_index;
						break;

					default:
						iNucIndex = 0;
						break;
				}
				// Removed: System is read at the beginning
				//EE_READ(syst, &current.system);
				flgOK = set_nuclide_data(iNucIndex, ch_num);
				disp_nuclide();

				if((m_ucSetMeasure>=25) && (m_ucSetMeasure<=30)){
					if(acc_data[m_ucSetMeasure-25].const_source){
						memcpy(&(chamb_vals[8]), &(chamb_vals[ch_num]), sizeof(CHAMBERVALS));
						memcpy(&(chamber[8]), &(chamber[ch_num]), sizeof(CHAMBER));
					}
				}

				send_to_amulet_string(100, (char *) m_acTitle);
				delaystamp = g_csec_tstamp + 65;
				message_color = 0;
				m_iPhase = PHASE_MEASURE_PAUSE;
			}else{
				flgOK = FALSE;
			}
			break;

		case PHASE_MEASURE_PAUSE:
			if(g_csec_tstamp > delaystamp){
				if(flgOK) SetAmuletByte(100, 0xFF);
				else SetAmuletByte(106, 0xFF);
				m_iPhase = PHASE_MEASURE_WAIT;
			}
			break;

		case PHASE_MEASURE_WAIT:
			if(current.num_chambers>0){
				if(flgOK){
					if(measurement[ch_num].display_flag_2){
						if(!measurement[ch_num].over_flag){
							if((m_ucSetMeasure>=25) && (m_ucSetMeasure<=30)){
								if(acc_data[m_ucSetMeasure-25].const_source){
									//memcpy(&(measurement[8]), &(measurement[ch_num]), sizeof(MEASUREMENT));
									memcpy(&last_measurement, &(measurement[ch_num]), sizeof(MEASUREMENT));
								}
							}
						}
						activity_to_screen();
						measurement[ch_num].display_flag_2 = FALSE;

						read_clock(&time0);
						//kun = measurement[ch_num].kun;
						// Changed:
						//act0 = measurement[ch_num].act / unitfact[kun - 1];

						if(!measurement[ch_num].over_flag){
							act0 = measurement[ch_num].act0;

							// Locked: CI
							//format_activity(act0, current.system, acMsg);
							//format_activity(act0, CI, acMsg);
							format_activity_system(act0, acMsg);
							trim_and_shrink(acMsg);
							over_flow = FALSE;
						}else{
							act0 = 100.0;
							get_amulet_message(L_OVERRANGE, acMsg);    // "OVER RANGE"
							over_flow = TRUE;
						}
						send_to_amulet_string(103, acMsg);

						strcpy(acMsg2, chamber[ch_num].nucdata.name);
						trim_and_shrink(acMsg2);
						send_to_amulet_string(102, acMsg2);

						SetAmuletByte(101, 0xFF);

						if((m_ucSetMeasure>=25) && (m_ucSetMeasure<=30)){
							pract = acc_data[m_ucSetMeasure-25].test_res[ch_num].pred_act;
							format_activity_system(pract, acMsg);
							diff = 100.0 * (act0 - pract)/pract;
							if(fabs(diff) > 20.0){
								get_amulet_message(L_CALC_DEV_ERROR, formatstring);    // "Calc: %s, Dev: ERROR"
								sprintf(acMsg2, formatstring, acMsg);
								send_to_amulet_string(106, acMsg2);
								if(message_color == 2) SetAmuletByte(103, 0xFF);
								else SetAmuletByte(105, 0xFF);
								message_color = 2;
							}else{
								get_amulet_message(L_CALC_DEV, formatstring);    // "Calc: %s, Dev: %.1f%%"
								sprintf(acMsg2, formatstring, acMsg, diff);
								send_to_amulet_string(104, acMsg2);
								if(message_color == 1) SetAmuletByte(102, 0xFF);
								else SetAmuletByte(104, 0xFF);
								message_color = 1;
							}
						}
					}
				}else{
					if(measurement[ch_num].display_flag_2){
						measurement[ch_num].display_flag_2 = FALSE;
						NuclideData_getName(iNucIndex, nucName);
						strcpy(acMsg, "No Cal# for ");
						strcat(acMsg, nucName);
						send_to_amulet_string(106, acMsg);
						if(message_color == 2) SetAmuletByte(103, 0xFF);
						else SetAmuletByte(105, 0xFF);
						message_color = 2;
					}
				}
			}
			break;

		case PHASE_MEASURE_ACCEPT:
			if(current.num_chambers>0){
				if(over_flow){
					SetAmuletBackHTML();
					m_ucSetMeasure = 0;
				}else{
					switch(m_ucSetMeasure){
						case 1:
							m_fAddInventory_Activity = act0;
							m_dtmAddInventory_Date = time0;
							m_ucAddInventory_MeasuredNuclide = iNucIndex;
							SetAmuletBackHTML();
							m_ucSetMeasure = 0;
							break;
						case 2:
							m_fWithdrawInventory_MeasuredActivity = act0;
							m_dtmWithdrawInventory_MeasuredDate = time0;
							m_ucWithdrawInventory_MeasuredNuclide = iNucIndex;
							SetAmuletBackHTML();
							m_ucSetMeasure = 0;
							break;

						case 3:
							m_fKitInventory_MeasuredActivity = act0;
							m_dtmKitInventory_MeasuredDate = time0;
							m_ucKitInventory_MeasuredNuclide = iNucIndex;
							SetAmuletBackHTML();
							m_ucSetMeasure = 0;
							break;

						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
						case 9:
						case 10:
						case 11:
							AmuletSetupLinearityLineatorMenu_measurement[m_ucSetMeasure - 4].activity = act0;
							AmuletSetupLinearityLineatorMenu_measurement[m_ucSetMeasure - 4].timestamp = time0;
							AmuletSetupLinearityLineatorMenu_measurement[m_ucSetMeasure - 4].nuclideid = iNucIndex;
							SetAmuletBackHTML();
							m_ucSetMeasure = 0;
							break;

						case 12:
						case 13:
						case 14:
						case 15:
						case 16:
						case 17:
						case 18:
						case 19:
						case 20:
						case 21:
						case 22:
						case 23:
							AmuletSetupLinearityCalicheckMenu_measurement[m_ucSetMeasure - 12].activity = act0;
							AmuletSetupLinearityCalicheckMenu_measurement[m_ucSetMeasure - 12].timestamp = time0;
							AmuletSetupLinearityCalicheckMenu_measurement[m_ucSetMeasure - 12].nuclideid = iNucIndex;
							SetAmuletBackHTML();
							m_ucSetMeasure = 0;
							break;

						case 24:
							AmuletDoseTableMenu_nuclideID = iNucIndex;
							AmuletDoseTableMenu_activity = act0;
							//AmuletDoseTableMenu_activityUnit = kun - 1;
							AmuletDoseTableMenu_timestamp = time0;
							SetAmuletBackHTML();
							m_ucSetMeasure = 0;
							break;

						case 25:
						case 26:
						case 27:
						case 28:
						case 29:
						case 30:
							if(acc_data[m_ucSetMeasure-25].const_source){
								memcpy(&(measurement[8]), &last_measurement, sizeof(MEASUREMENT));
							}
							acc_data[m_ucSetMeasure-25].test_res[ch_num].ms_act = act0;
							acc_data[m_ucSetMeasure-25].test_res[ch_num].ms_kun = 0;
							acc_data[m_ucSetMeasure-25].test_res[ch_num].measuredon = clock_time;
							acc_data[m_ucSetMeasure-25].test_res[ch_num].resp0 = measurement[ch_num].resp0;
							acc_data[m_ucSetMeasure-25].test_res[ch_num].over_flag = FALSE;
							SetAmuletBackHTML();
							m_ucSetMeasure = 0;
							break;

						default:
							m_iPhase = PHASE_MEASURE_WAIT;
							break;
					}
				}
			}
			break;
	}
}
