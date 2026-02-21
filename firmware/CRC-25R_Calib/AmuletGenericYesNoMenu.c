#define PHASE_GENERICYESNO_PRE_INIT	0
#define PHASE_GENERICYESNO_WAIT		1
#define PHASE_GENERICYESNO_YES		2
#define PHASE_GENERICYESNO_NO		3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "database.h"
#include "wipes.h"
#include "chambfac.h"
#include "nuc.h"
#include "message.h"
#include "daily.h"
#include "i2c.h"

#define CONFIG_YESNO_RESUME_AUTOLIN	1
#define CONFIG_YESNO_HOME_PAUSE_AUTOLIN	2
#define CONFIG_YESNO_BACK_PAUSE_AUTOLIN	3
#define CONFIG_YESNO_ABORT_AUTOLIN	4
#define CONFIG_YESNO_SAVE_AUTOLIN	5
#define CONFIG_YESNO_SAVE_ACCURACY	6

extern CURRENT current;
extern CHAMBERVALS chamb_vals[];
extern int m_iPhase;
extern unsigned char m_ucClear;
extern AUTOLINEARITYTEST AmuletAutoLinearityTest_test;
extern int AmuletAutoLinearityTest_mode;
extern short num_daily;
extern ACCDATA acc_data[];
extern MEASUREMENT measurement[];
extern REMOTE remote[9];

long long int AmuletGenericYesNo_LatchedDailyTestID;
bool AmuletGenericYesNo_AccuracyTestPending;

uchar AmuletGenericYesNo_config;
char AmuletGenericYesNo_comment[26];
static int AmuletGenericYesNo_resumeChamber;
static int AmuletGenericYesNo_resumeNuclideID;

void trim(char *acByte);
void trim_and_shrink(char *acByte);
void SetAmuletHomeHTML(void);
void SetAmuletBackHTML(void);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
void AmuletAutoLinearityTest_clearTest(void);
void AmuletAutoLinearityTest_eraseAutoLinearityTest(void);
void AmuletAutoLinearityTest_matchChamberAndNuclide(int *resumeChamber, int *resumeNuclideID);
bool AmuletAutoLinearityTest_isFinished(void);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void DB_CreateChamberAccuracyTest(CHAMBERACCURACYTEST *chamberaccuracytest, long long int ChamberDailyTestID, bool bookEnd);
void DB_CreateChamberAccuracyMeasurement(CHAMBERACCURACYMEASUREMENT *chamberaccuracymeasurement, bool bookEnd);
void DB_CreateChamberAutoConstancy(CHAMBERAUTOCONSTANCY *chamberautoconstancy, bool bookEnd);

void AmuletGenericYesNo_saveInDatabase(bool autoconstancy){
	CHAMBERACCURACYTEST *chamberaccuracytest;
	CHAMBERACCURACYMEASUREMENT *chamberaccuracymeasurement;
	CHAMBERAUTOCONSTANCY *chamberautoconstancy;
	short ch_num = current.main_chamber;
	short constch[12];
	bool measuredConstancy, measuredAccuracy;
	int i, index;
	char message[100];

	chamberaccuracytest = (CHAMBERACCURACYTEST *) malloc(sizeof(CHAMBERACCURACYTEST));
	chamberaccuracytest->ChamberAccuracyTestID = 0;
	chamberaccuracytest->ChamberSerialNumber[6] = 0;
	strncpy(chamberaccuracytest->ChamberSerialNumber, chamb_vals[ch_num].sn, 6);
	chamberaccuracytest->ChamberType = chamb_vals[ch_num].chamb_type;
	chamberaccuracytest->TwoStageChamber = chamber_one_gain_relay(ch_num);
	chamberaccuracytest->ConstancyNuclide[0] = 0;
	chamberaccuracytest->ConstancyNuclideSerialNumber[0] = 0;
	chamberaccuracytest->ConstancyVoltage = -999.0;

	measuredConstancy = FALSE;
	if(autoconstancy){
		for(i=0; i<num_daily; i++){
			if(acc_data[i].const_source){
				strcpy(chamberaccuracytest->ConstancyNuclide, acc_data[i].nucname);
				strcpy(chamberaccuracytest->ConstancyNuclideSerialNumber, acc_data[i].sn);
				trim(chamberaccuracytest->ConstancyNuclide);
				if((acc_data[i].test_res[ch_num].ms_kun>=0) && (acc_data[i].test_res[ch_num].nc != -1)){
					chamberaccuracytest->ConstancyVoltage = measurement[8].meas;
					measuredConstancy = TRUE;
				}
				break;
			}
		}
	}

	chamberaccuracytest->CreatedOn = 0;
	chamberaccuracytest->InactiveReason[0] = 0;
	chamberaccuracytest->Inactive = FALSE;

	DB_CreateChamberAccuracyTest(chamberaccuracytest, AmuletGenericYesNo_LatchedDailyTestID, TRUE);

	if(chamberaccuracytest->ChamberAccuracyTestID > 0){
		measuredAccuracy = FALSE;
		for(i=0; i<num_daily; i++){
			if(acc_data[i].test_res[ch_num].ms_kun >= 0){
				measuredAccuracy = TRUE;
				break;
			}
		}

		if(measuredAccuracy){
			chamberaccuracymeasurement = (CHAMBERACCURACYMEASUREMENT *) malloc(sizeof(CHAMBERACCURACYMEASUREMENT));

			DB_StartBookEnd();

			for(i=0; i<num_daily; i++){
				if(acc_data[i].test_res[ch_num].ms_kun >= 0){
					chamberaccuracymeasurement->ChamberAccuracyMeasurementID = 0;
					chamberaccuracymeasurement->ChamberAccuracyTestID = chamberaccuracytest->ChamberAccuracyTestID;
					strcpy(chamberaccuracymeasurement->SourceNuclide, acc_data[i].nucname);
					chamberaccuracymeasurement->SourceNuclideIndex = acc_data[i].nuc_index;
					trim(chamberaccuracymeasurement->SourceNuclide);
					strcpy(chamberaccuracymeasurement->SourceSerialNumber, acc_data[i].sn);
					trim(chamberaccuracymeasurement->SourceSerialNumber);
					chamberaccuracymeasurement->SourceCalActivity = acc_data[i].cal_activity;
					chamberaccuracymeasurement->SourceCalDate = acc_data[i].cal_date;
					chamberaccuracymeasurement->MeasuredOn = acc_data[i].test_res[ch_num].measuredon;
					strcpy(chamberaccuracymeasurement->CalculatedActivityTextEnglish, acc_data[i].test_res[ch_num].pract);
					trim(chamberaccuracymeasurement->CalculatedActivityTextEnglish);
					replace(chamberaccuracymeasurement->CalculatedActivityTextEnglish, '$', 'u');
					strcpy(chamberaccuracymeasurement->CalculatedActivityTextFrench, chamberaccuracymeasurement->CalculatedActivityTextEnglish);
					chamberaccuracymeasurement->CalculatedActivity = acc_data[i].test_res[ch_num].pred_act;
					chamberaccuracymeasurement->Response = acc_data[i].test_res[ch_num].resp0;
					chamberaccuracymeasurement->HalfLife = NuclideData_getHalflife(chamberaccuracymeasurement->SourceNuclideIndex);
					chamberaccuracymeasurement->HalfLifeUnit = NuclideData_getHalflifeUnit(chamberaccuracymeasurement->SourceNuclideIndex);

					if(!acc_data[i].test_res[ch_num].over_flag){
						strcpy(chamberaccuracymeasurement->MeasuredActivityTextEnglish, acc_data[i].test_res[ch_num].msact);
						trim(chamberaccuracymeasurement->MeasuredActivityTextEnglish);
						replace(chamberaccuracymeasurement->MeasuredActivityTextEnglish, '$', 'u');
						strcpy(chamberaccuracymeasurement->MeasuredActivityTextFrench, chamberaccuracymeasurement->MeasuredActivityTextEnglish);
						chamberaccuracymeasurement->MeasuredActivity = acc_data[i].test_res[ch_num].ms_act;
						if(acc_data[i].test_res[ch_num].nc == -1){
							chamberaccuracymeasurement->DeviationStatus = 1;
							get_amulet_message_with_language(L_ERROR2, chamberaccuracymeasurement->DeviationTextEnglish, ENGLISH);    // "Error"
							get_amulet_message_with_language(L_ERROR2, chamberaccuracymeasurement->DeviationTextFrench, FRENCH);    // "Error"
						}else{
							chamberaccuracymeasurement->DeviationStatus = 0;
							strcpy(chamberaccuracymeasurement->DeviationTextEnglish, acc_data[i].test_res[ch_num].var);
							trim(chamberaccuracymeasurement->DeviationTextEnglish);
							strcat(chamberaccuracymeasurement->DeviationTextEnglish, " %");
							strcpy(chamberaccuracymeasurement->DeviationTextFrench, chamberaccuracymeasurement->DeviationTextEnglish);
						}
						chamberaccuracymeasurement->Deviation = acc_data[i].test_res[ch_num].diff;
					}else{
						get_amulet_message_with_language(L_OVERRANGE2, chamberaccuracymeasurement->MeasuredActivityTextEnglish, ENGLISH);    // "Over Range"
						get_amulet_message_with_language(L_OVERRANGE2, chamberaccuracymeasurement->MeasuredActivityTextFrench, FRENCH);    // "Over Range"
						chamberaccuracymeasurement->MeasuredActivity = 100.0;
						chamberaccuracymeasurement->DeviationStatus = 1;
						get_amulet_message_with_language(L_ERROR2, chamberaccuracymeasurement->DeviationTextEnglish, ENGLISH);    // "Error"
						get_amulet_message_with_language(L_ERROR2, chamberaccuracymeasurement->DeviationTextFrench, FRENCH);    // "Error"
						chamberaccuracymeasurement->Deviation = 999;
					}

					DB_CreateChamberAccuracyMeasurement(chamberaccuracymeasurement, FALSE);
				}
			}

			DB_EndBookEnd();

			free(chamberaccuracymeasurement);
		}

		if(measuredConstancy){
			remote[8].exists = FALSE;
			EE_READ(constancych, (uchar *) &constch);

			measuredConstancy = FALSE;
			for(index=0; index<12; index++){
				if(constch[index]>=0){
					measuredConstancy = TRUE;
					break;
				}
			}

			if(measuredConstancy){
				chamberautoconstancy = (CHAMBERAUTOCONSTANCY *) malloc(sizeof(CHAMBERAUTOCONSTANCY));

				DB_StartBookEnd();

				for(index=0; index<12; index++){
					if(constch[index]>=0){
						if(NuclideData_getEffectiveResponse(constch[index], chamberaccuracytest->ChamberType)!=0.0){
							set_nuclide_data(constch[index], 8);
							display_activity(8);
							chamberautoconstancy->ChamberAutoConstancyID = 0;
							chamberautoconstancy->ChamberAccuracyTestID = chamberaccuracytest->ChamberAccuracyTestID;
							NuclideData_getName(constch[index], message);
							strcpy(chamberautoconstancy->ConstancyNuclide, message);
							trim_and_shrink(chamberautoconstancy->ConstancyNuclide);
							chamberautoconstancy->ConstancyNuclideIndex = constch[index];
							chamberautoconstancy->Response = measurement[8].resp0;
							chamberautoconstancy->HalfLife = NuclideData_getHalflife(chamberautoconstancy->ConstancyNuclideIndex);
							chamberautoconstancy->HalfLifeUnit = NuclideData_getHalflifeUnit(chamberautoconstancy->ConstancyNuclideIndex);
							strcpy(chamberautoconstancy->ConstancyActivityTextEnglish, measurement[8].actstr);
							trim(chamberautoconstancy->ConstancyActivityTextEnglish);
							replace(chamberautoconstancy->ConstancyActivityTextEnglish, '$', 'u');
							strcpy(chamberautoconstancy->ConstancyActivityTextFrench, chamberautoconstancy->ConstancyActivityTextEnglish);
							chamberautoconstancy->ConstancyActivity = measurement[8].act0;
							DB_CreateChamberAutoConstancy(chamberautoconstancy, FALSE);
						}
					}
				}

				DB_EndBookEnd();

				free(chamberautoconstancy);
			}
		}
	}

	free(chamberaccuracytest);
}

void AmuletGenericYesNo_menu(void){
	char message[100];
	char formatstring[100];

	switch(m_iPhase){
		case PHASE_GENERICYESNO_PRE_INIT:
			send_amulet_message(L_COMMENT, 104);    // "Comment:"
			send_amulet_message(L_YES, 105);    // "Yes"
			send_amulet_message(L_NO, 106);    // "No"
			send_amulet_message(L_PLEASE_ENTER_COMMENT, 107);    // "Please enter Comment:"

			switch(AmuletGenericYesNo_config){
				case CONFIG_YESNO_RESUME_AUTOLIN:
					send_amulet_message(L_RESUME_AUTOLINEARITY_TEST, 100);    // "Resume AutoLinearity Test"

					// Test Chamber S/N
					AmuletAutoLinearityTest_matchChamberAndNuclide(&AmuletGenericYesNo_resumeChamber, &AmuletGenericYesNo_resumeNuclideID);

					if(AmuletGenericYesNo_resumeChamber == -1){
						send_amulet_message(L_ERROR, 73);    // "ERROR"
						SetAmuletByte(82, 0xFF);

						get_amulet_message(L_UNABLE_TO_FIND_CHAMBER_WITH_SN, formatstring);    // "Unable to find chamber with S/N: %s"
						sprintf(message, formatstring, AmuletAutoLinearityTest_test.SerialNumber);
						send_to_amulet_string(79, message);
						SetAmuletByte(84, 0xFF);

						send_amulet_message(L_ABORT_CURRENT_AUTOLINEARITY_TEST, 94);    // "Abort current AutoLinearity Test?"
						SetAmuletByte(89, 0xFF);
					}else if(AmuletGenericYesNo_resumeNuclideID == -1){
						send_amulet_message(L_ERROR, 73);    // "ERROR"
						SetAmuletByte(82, 0xFF);

						get_amulet_message(L_ABORT_UNABLE_TO_FIND_USABLE_NUCLIDE, formatstring);    // "Unable to find usable Nuclide: %s"
						sprintf(message, formatstring, AmuletAutoLinearityTest_test.NuclideName);
						send_to_amulet_string(79, message);
						SetAmuletByte(84, 0xFF);

						send_amulet_message(L_ABORT_AUTOLINEARITY_TEST2, 94);    // "Abort AutoLinearity Test?"
						SetAmuletByte(89, 0xFF);
					}else{
						if(AmuletAutoLinearityTest_isFinished()){
							send_amulet_message(L_COMPLETED_AUTOLINEARITY_NOT_SAVED, 76);    // "Completed AutoLinearity Test has not been saved."
							SetAmuletByte(83, 0xFF);

							send_amulet_message(L_PROCEED_TO_AUTOLINEARITY, 94);    // "Proceed to AutoLinearity Test?"
							SetAmuletByte(89, 0xFF);
						}else{
							get_amulet_message(L_PLEASE_PLACE_NUCLIDE, formatstring);    // "Please place Nuclide: %s into Chamber: %d"
							sprintf(message, formatstring, AmuletAutoLinearityTest_test.NuclideName, AmuletGenericYesNo_resumeChamber + 1);
							send_to_amulet_string(76, message);
							SetAmuletByte(83, 0xFF);

							send_amulet_message(L_RESUME_AUTOLINEARITY, 94);    // "Resume AutoLinearity Test?"
							SetAmuletByte(89, 0xFF);
						}
					}
					break;

				case CONFIG_YESNO_HOME_PAUSE_AUTOLIN:
					send_amulet_message(L_PAUSE_AUTOLINEARITY_TEST, 100);    // "Pause AutoLinearity Test"

					send_amulet_message(L_AUTOLINEARITY_HAS_BEEN_PAUSED, 76);    // "AutoLinearity has been paused."
					SetAmuletByte(83, 0xFF);

					if(AmuletAutoLinearityTest_isFinished()){
						send_amulet_message(L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED, 82);    // "Completed AutoLinearity Test has not been saved."
						SetAmuletByte(85, 0xFF);
					}else{
						send_amulet_message(L_TO_AVOID_MISSING_MEASUREMENTS, 82);    // "To avoid missing measurements, "
						SetAmuletByte(85, 0xFF);

						send_amulet_message(L_PLEASE_RETURN_TO_AUTOLINEARITY, 85);    // "Please return to AutoLinearity as soon as possible."
						SetAmuletByte(86, 0xFF);
					}

					send_amulet_message(L_PROCEED_TO_HOME_SCREEN, 94);    // "Proceed to Home Screen?"
					SetAmuletByte(89, 0xFF);
					break;

				case CONFIG_YESNO_BACK_PAUSE_AUTOLIN:
					send_amulet_message(L_PAUSE_AUTOLINEARITY_TEST, 100);    // "Pause AutoLinearity Test"

					send_amulet_message(L_AUTOLINEARITY_HAS_BEEN_PAUSED, 76);    // "AutoLinearity has been paused."
					SetAmuletByte(83, 0xFF);

					if(AmuletAutoLinearityTest_isFinished()){
						send_amulet_message(L_COMPLETED_AUTOLINEARITY_HAS_NOT_BEEN_SAVED, 82);    // "Completed AutoLinearity Test has not been saved."
						SetAmuletByte(85, 0xFF);
					}else{
						send_amulet_message(L_TO_AVOID_MISSING_MEASUREMENTS, 82);    // "To avoid missing measurements, "
						SetAmuletByte(85, 0xFF);

						send_amulet_message(L_PLEASE_RETURN_TO_AUTOLINEARITY, 85);    // "Please return to AutoLinearity as soon as possible."
						SetAmuletByte(86, 0xFF);
					}

					send_amulet_message(L_PROCEED_TO_AUTOLINEARITY_MENU, 94);    // "Proceed to AutoLinearity Menu?"
					SetAmuletByte(89, 0xFF);
					break;

				case CONFIG_YESNO_ABORT_AUTOLIN:
					send_amulet_message(L_ABORT_AUTOLINEARITY_TEST, 100);    // "Abort AutoLinearity Test"

					if(AmuletAutoLinearityTest_isFinished()){
						send_amulet_message(L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED, 82);    // "Current AutoLinearity Test has finished."
						SetAmuletByte(85, 0xFF);
					}else{
						send_amulet_message(L_AUTOLINEARITY_HAS_BEEN_PAUSED, 82);    // "AutoLinearity has been paused."
						SetAmuletByte(85, 0xFF);
					}
					send_amulet_message(L_ERASE_CURRENT_TEST, 94);    // "Erase Current Test?"
					SetAmuletByte(89, 0xFF);
					break;

				case CONFIG_YESNO_SAVE_AUTOLIN:
					send_amulet_message(L_SAVE_AUTOLINEARITY_TEST, 100);    // "Save AutoLinearity Test"

					send_to_amulet_string(103, AmuletGenericYesNo_comment);
					SetAmuletByte(91, 0xFF);

					if(AmuletAutoLinearityTest_isFinished()){
						send_amulet_message(L_CURRENT_AUTOLINEARITY_TEST_HAS_FINISHED, 82);    // "Current AutoLinearity Test has finished."
						SetAmuletByte(85, 0xFF);

						send_amulet_message(L_SAVE_CURRENT_TEST, 94);    // "Save Current Test?"
						SetAmuletByte(89, 0xFF);
					}else{
						send_amulet_message(L_AUTOLINEARITY_HAS_BEEN_PAUSED, 79);    // "AutoLinearity has been paused."
						SetAmuletByte(84, 0xFF);

						send_amulet_message(L_CURRENT_AUTOLINEARITY_IS_PARTIALLY_COMPLETE, 82);    // "Current AutoLinearity Test is partially complete."
						SetAmuletByte(85, 0xFF);

						send_amulet_message(L_END_AND_SAVE_TEST, 94);    // "End and Save Current Test?"
						SetAmuletByte(89, 0xFF);
					}
					break;

				case CONFIG_YESNO_SAVE_ACCURACY:
					send_amulet_message(L_SAVE_ACCURACY_TEST, 100);    // "Save Accuracy / AutoConstancy"
					send_amulet_message(L_END_AND_SAVE_TEST, 82);    // "End and Save Current Test?"
					SetAmuletByte(85, 0xFF);

					break;
			}
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_GENERICYESNO_WAIT;
			break;

		case PHASE_GENERICYESNO_WAIT:
			break;

		case PHASE_GENERICYESNO_YES:
			beep_amulet();
			switch(AmuletGenericYesNo_config){
				case CONFIG_YESNO_RESUME_AUTOLIN:
					if((AmuletGenericYesNo_resumeChamber == -1) || (AmuletGenericYesNo_resumeNuclideID == -1)){
						AmuletAutoLinearityTest_eraseAutoLinearityTest();
						AmuletAutoLinearityTest_clearTest();
					}

					AmuletAutoLinearityTest_mode = 0;
					m_ucClear = 105;
					PopPageStack();
					SetAmuletHTML(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
					PushPageStack(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
					return;

				case CONFIG_YESNO_HOME_PAUSE_AUTOLIN:
					PopPageStack();
					SetAmuletHomeHTML();
					return;

				case CONFIG_YESNO_BACK_PAUSE_AUTOLIN:
					PopPageStack();
					SetAmuletBackHTML();
					return;

				case CONFIG_YESNO_ABORT_AUTOLIN:
					AmuletAutoLinearityTest_eraseAutoLinearityTest();
					AmuletAutoLinearityTest_clearTest();
					AmuletAutoLinearityTest_mode = 0;
					m_ucClear = 105;
					SetAmuletBackHTML();
					return;

				case CONFIG_YESNO_SAVE_AUTOLIN:
					strcpy(AmuletAutoLinearityTest_test.Comment, AmuletGenericYesNo_comment);
					DB_CreateAutoLinearityTest(&AmuletAutoLinearityTest_test, TRUE);
					AmuletAutoLinearityTest_eraseAutoLinearityTest();
					AmuletAutoLinearityTest_clearTest();
					AmuletAutoLinearityTest_mode = 0;
					m_ucClear = 105;
					SetAmuletBackHTML();
					break;

				case CONFIG_YESNO_SAVE_ACCURACY:
					AmuletGenericYesNo_saveInDatabase(TRUE);
					PopPageStack();
					SetAmuletHomeHTML();
					return;
			}
			break;

		case PHASE_GENERICYESNO_NO:
			beep_amulet();
			switch(AmuletGenericYesNo_config){
				case CONFIG_YESNO_RESUME_AUTOLIN:
				case CONFIG_YESNO_HOME_PAUSE_AUTOLIN:
				case CONFIG_YESNO_BACK_PAUSE_AUTOLIN:
				case CONFIG_YESNO_ABORT_AUTOLIN:
				case CONFIG_YESNO_SAVE_AUTOLIN:
				case CONFIG_YESNO_SAVE_ACCURACY:
					SetAmuletBackHTML();
					return;
			}
			break;
	}
}
