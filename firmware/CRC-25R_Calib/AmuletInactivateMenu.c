#define PHASE_INACTIVATE_PRE_INIT	0
#define PHASE_INACTIVATE_WAIT		1
#define PHASE_INACTIVATE_YES		2
#define PHASE_INACTIVATE_NO			3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "database.h"
#include "message.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern WELLWIPE AmuletWellWipeReportMenu_wellWipe;
extern WELLSYSTEMTEST AmuletWellSystemTestAnalysisMenu_systemTest;
extern WELLMDATEST AmuletWellMDAAnalysisMenu_mda;
extern WELLCHITEST AmuletWellChiAnalysisMenu_chi;
extern WELLSCHILLINGTEST AmuletWellSchillingReport_test;
extern WELLPLASMATEST AmuletWellPlasmaReportMenu_test;
extern WELLRBCTEST AmuletWellRBCReportMenu_test;
extern PROBEBIOASSAYTEST AmuletWellBioAssayAnalysis_test;
extern PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeTest;
extern WELLRBCSURVIVALTEST AmuletWellRBCSurvivalTest;
extern AUTOLINEARITYTEST AmuletAutoLinearityTest_retrieved;
extern CHAMBERDAILYTEST			AmuletChamberSearchMenu_DailyTest;
extern CHAMBERZERO					AmuletChamberSearchMenu_Zero;
extern CHAMBERBACKGROUND			AmuletChamberSearchMenu_Background;
extern CHAMBERVOLTAGE				AmuletChamberSearchMenu_Voltage;
extern CHAMBERACCURACYTEST			AmuletChamberSearchMenu_AccuracyTest;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
unsigned char PopPageStack(void);
void SetAmuletBackHTML(void);
void AmuletWellWipeSearch_inactivate(long long WellWipeID);
void AmuletWellSystemTestSearch_inactivate(long long int WellSystemTestID);
void AmuletWellMDASearch_inactivate(long long int WellMDATestID);
void AmuletWellChiSearch_inactivate(long long int WellChiSquareTestID);
void AmuletWellSchillingSearch_inactivate(long long int WellSchillingTestID);
void AmuletWellPlasmaSearch_inactivate(long long int WellPlasmaTestID);
void AmuletWellRBCSearch_inactivate(long long int WellRBCTestID);
void AmuletWellBioAssaySearch_inactivate(long long int ProbeBioAssayTestID);
void AmuletAutoLinearitySearch_inactivate(long long int AutoLinearityTestID);
void AmuletChamberSearch_inactivate(int mode, long long int TestID);

void send_to_amulet_string(uchar ucIndex, char message0[]);

uchar AmuletInactivate_config;
char AmuletInactivate_comment[26];

void AmuletInactivate_menu(void){
	switch(m_iPhase){
		case PHASE_INACTIVATE_PRE_INIT:
			send_amulet_message(L_COMMENT, 105);    // "Comment:"
			send_amulet_message(L_PLEASE_ENTER_COMMENT, 106);    // "Please enter Comment:"
			send_amulet_message(L_BTN_YES, 108);    // "YES"
			send_amulet_message(L_BTN_NO, 109);    // "NO"
			if(m_ucClear == 63){
				if(AmuletInactivate_config == 9){
					strcpy(AmuletInactivate_comment, AmuletWellThyroidUptakeTest.InactiveReason);
				}else if(AmuletInactivate_config == 10){
					strcpy(AmuletInactivate_comment, AmuletWellRBCSurvivalTest.InactiveReason);
				}else{
					AmuletInactivate_comment[0] = 0;
				}
				m_ucClear = 0;
			}

			if(AmuletInactivate_config == 9){
				send_amulet_message(L_COMPLETE_THYROID_UPTAKE_TEST_ENGLISH, 101);    // "Complete Thyroid Uptake Test"
				send_amulet_message(L_MARK_TEST_COMPLETED_ENGLISH, 103);    // "Mark Test Completed?"
			}else if(AmuletInactivate_config == 10){
				send_amulet_message(L_COMPLETE_RBC_SURVIVAL_TEST_ENGLISH, 101);    // "Complete RBC Survival Test"
				send_amulet_message(L_MARK_TEST_COMPLETED_ENGLISH, 103);    // "Mark Test Completed?"
			}else{
				send_amulet_message(L_INACTIVATE_RECORD, 101);    // "Inactivate Record"
				send_amulet_message(L_INACTIVATE_RECORD2, 103);    // "Inactivate record?"
			}
			send_to_amulet_string(100, AmuletInactivate_comment);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_INACTIVATE_WAIT;
			break;

		case PHASE_INACTIVATE_WAIT:
			break;

		case PHASE_INACTIVATE_YES:
			beep_amulet();
			switch(AmuletInactivate_config){
				case 1:
					strcpy(AmuletWellWipeReportMenu_wellWipe.InactiveReason, AmuletInactivate_comment);
					DB_InactivateWipe(&AmuletWellWipeReportMenu_wellWipe);
					AmuletWellWipeSearch_inactivate(AmuletWellWipeReportMenu_wellWipe.WellWipeID);
					break;

				case 2:
					strcpy(AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason, AmuletInactivate_comment);
					DB_InactivateSystemTest(&AmuletWellSystemTestAnalysisMenu_systemTest);
					AmuletWellSystemTestSearch_inactivate(AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID);
					break;

				case 3:
					strcpy(AmuletWellMDAAnalysisMenu_mda.InactiveReason, AmuletInactivate_comment);
					DB_InactivateMDATest(&AmuletWellMDAAnalysisMenu_mda);
					AmuletWellMDASearch_inactivate(AmuletWellMDAAnalysisMenu_mda.WellMDATestID);
					break;

				case 4:
					strcpy(AmuletWellChiAnalysisMenu_chi.InactiveReason, AmuletInactivate_comment);
					DB_InactivateChiTest(&AmuletWellChiAnalysisMenu_chi);
					AmuletWellChiSearch_inactivate(AmuletWellChiAnalysisMenu_chi.WellChiSquareTestID);
					break;

				case 5:
					strcpy(AmuletWellSchillingReport_test.InactiveReason, AmuletInactivate_comment);
					DB_InactivateSchillingTest(&AmuletWellSchillingReport_test);
					AmuletWellSchillingSearch_inactivate(AmuletWellSchillingReport_test.WellSchillingTestID);
					break;

				case 6:
					strcpy(AmuletWellPlasmaReportMenu_test.InactiveReason, AmuletInactivate_comment);
					DB_InactivatePlasmaTest(&AmuletWellPlasmaReportMenu_test);
					AmuletWellPlasmaSearch_inactivate(AmuletWellPlasmaReportMenu_test.WellPlasmaTestID);
					break;

				case 7:
					strcpy(AmuletWellRBCReportMenu_test.InactiveReason, AmuletInactivate_comment);
					DB_InactivateRBCTest(&AmuletWellRBCReportMenu_test);
					AmuletWellRBCSearch_inactivate(AmuletWellRBCReportMenu_test.WellRBCTestID);
					break;

				case 8:
					strcpy(AmuletWellBioAssayAnalysis_test.InactiveReason, AmuletInactivate_comment);
					DB_InactivateBioAssayTest(&AmuletWellBioAssayAnalysis_test);
					AmuletWellBioAssaySearch_inactivate(AmuletWellBioAssayAnalysis_test.ProbeBioAssayTestID);
					break;

				case 9:
					strcpy(AmuletWellThyroidUptakeTest.InactiveReason, AmuletInactivate_comment);
					DB_InactivateProbeThyroidUptakeTest(&AmuletWellThyroidUptakeTest);
					break;

				case 10:
					strcpy(AmuletWellRBCSurvivalTest.InactiveReason, AmuletInactivate_comment);
					DB_InactivateWellRBCSurvivalTest(&AmuletWellRBCSurvivalTest);
					break;

				case 11:
					strcpy(AmuletAutoLinearityTest_retrieved.InactiveReason, AmuletInactivate_comment);
					DB_InactivateAutoLinearityTest(&AmuletAutoLinearityTest_retrieved);
					AmuletAutoLinearitySearch_inactivate(AmuletAutoLinearityTest_retrieved.AutoLinearityTestID);
					break;

				case 12:
					strcpy(AmuletChamberSearchMenu_DailyTest.InactiveReason, AmuletInactivate_comment);
					DB_InactivateDailyTest(&AmuletChamberSearchMenu_DailyTest);

					if(AmuletChamberSearchMenu_Zero.ChamberZeroID > 0){
						strcpy(AmuletChamberSearchMenu_Zero.InactiveReason, AmuletInactivate_comment);
						DB_InactivateZeroMeasurement(&AmuletChamberSearchMenu_Zero);
					}

					if(AmuletChamberSearchMenu_Background.ChamberBackgroundID > 0){
						strcpy(AmuletChamberSearchMenu_Background.InactiveReason, AmuletInactivate_comment);
						DB_InactivateBackgroundMeasurement(&AmuletChamberSearchMenu_Background);
					}

					if(AmuletChamberSearchMenu_Voltage.ChamberVoltageID > 0){
						strcpy(AmuletChamberSearchMenu_Voltage.InactiveReason, AmuletInactivate_comment);
						DB_InactivateChamberVoltage(&AmuletChamberSearchMenu_Voltage);
					}

					if(AmuletChamberSearchMenu_AccuracyTest.ChamberAccuracyTestID > 0){
						strcpy(AmuletChamberSearchMenu_AccuracyTest.InactiveReason, AmuletInactivate_comment);
						DB_InactivateAccuracyTest(&AmuletChamberSearchMenu_AccuracyTest);
					}

					AmuletChamberSearch_inactivate(0, AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID);
					break;

				case 13:
					strcpy(AmuletChamberSearchMenu_Zero.InactiveReason, AmuletInactivate_comment);
					DB_InactivateZeroMeasurement(&AmuletChamberSearchMenu_Zero);
					AmuletChamberSearch_inactivate(1, AmuletChamberSearchMenu_Zero.ChamberZeroID);
					break;

				case 14:
					strcpy(AmuletChamberSearchMenu_Background.InactiveReason, AmuletInactivate_comment);
					DB_InactivateBackgroundMeasurement(&AmuletChamberSearchMenu_Background);
					AmuletChamberSearch_inactivate(2, AmuletChamberSearchMenu_Background.ChamberBackgroundID);
					break;

				case 15:
					strcpy(AmuletChamberSearchMenu_Voltage.InactiveReason, AmuletInactivate_comment);
					DB_InactivateChamberVoltage(&AmuletChamberSearchMenu_Voltage);
					AmuletChamberSearch_inactivate(3, AmuletChamberSearchMenu_Voltage.ChamberVoltageID);
					break;

				case 16:
					strcpy(AmuletChamberSearchMenu_AccuracyTest.InactiveReason, AmuletInactivate_comment);
					DB_InactivateAccuracyTest(&AmuletChamberSearchMenu_AccuracyTest);
					AmuletChamberSearch_inactivate(4, AmuletChamberSearchMenu_AccuracyTest.ChamberAccuracyTestID);
					break;
			}
			PopPageStack();
			SetAmuletBackHTML();
			return;

		case PHASE_INACTIVATE_NO:
			beep_amulet();
			SetAmuletBackHTML();
			return;
	}
}
