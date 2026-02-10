/**
 * \file
 * \details This file handles calls from the Amulet AutoLinearity Test Screen
 */
#define PHASE_AUTOLINERITYTEST_PRE_INIT	0
#define PHASE_AUTOLINERITYTEST_WAIT		1
#define PHASE_AUTOLINERITYTEST_START	2
#define PHASE_AUTOLINERITYTEST_ABORT	3
#define PHASE_AUTOLINERITYTEST_HOME		4
#define PHASE_AUTOLINERITYTEST_BACK		5
#define PHASE_AUTOLINERITYTEST_PGDOWN	6
#define PHASE_AUTOLINERITYTEST_PGUP		7
#define PHASE_AUTOLINERITYTEST_SAVE		8
#define PHASE_AUTOLINERITYTEST_EXPORT	9
#define PHASE_AUTOLINERITYTEST_PRINT	10

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "amulet.h"
#include "time.h"
#include "mca.h"
#include "database.h"
#include "nuc.h"
#include "printer.h"
#include "ff.h"
#include "message.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern time_t clock_time;
extern CHAMBER  chamber[];
extern CHAMBERVALS chamb_vals[];
extern MEASUREMENT measurement[];
extern uchar AmuletGenericYesNo_config;
extern char AmuletGenericYesNo_comment[26];
//extern uchar m_ucEnumerated;

AUTOLINEARITYTEST AmuletAutoLinearityTest_test;
AUTOLINEARITYTEST AmuletAutoLinearityTest_retrieved;

int AmuletAutoLinearityTest_mode;	// 0 = running test, 1 = view data
char AmuletAutoLinearityTest_nuclideName[10];
int AmuletAutoLinearityTest_intervalMinutes;
int AmuletAutoLinearityTest_totalMinutes;

static time_t AmuletAutoLinearityTest_displayed_t;
static time_t AmuletAutoLinearityTest_Start_t;
static time_t AmuletAutoLinearityTest_End_t;
static int AmuletAutoLinearityTest_nuclideIndex;

static int AmuletAutoLinearityTest_testCount;
static int AmuletAutoLinearityTest_currentPage;	// Zero based
static int AmuletAutoLinearityTest_lastPage;	// Zero based

static FILINFO AmuletAutoLinearityTest_fileInfo;
static FIL AmuletAutoLinearityTest_fileObject;
//static FRESULT AmuletAutoLinearityTest_Result;
static char AmuletAutoLinearityTest_longFileName[100];
//static char AmuletAutoLinearityTest_AutoLinFileName[100];
//static DIR AmuletAutoLinearityTest_dirObject;

void trim_and_shrink(char *acByte);
void trim(char *acByte);
void SetAmuletHomeHTML(void);
void SetAmuletBackHTML(void);
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void GetExtendedTimeInfoLanguageSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
static void AmuletAutoLinearityTest_displayPage(AUTOLINEARITYTEST *tst);
void AmuletAutoLinearityTest_clearTest(void);
static void AmuletAutoLinearityTest_showElapsedRemaining(void);
void AmuletAutoLinearityTest_saveAutoLinearityTest(AUTOLINEARITYTEST *test);
void AmuletAutoLinearityTest_eraseAutoLinearityTest(void);
void AmuletAutoLinearityTest_matchChamberAndNuclide(int *resumeChamber, int *resumeNuclideID);
int AmuletAutoLinearityTest_getTestCount(AUTOLINEARITYTEST *tst);
int AmuletAutoLinearityTest_getMeasurementCount(AUTOLINEARITYTEST *tst);
void AmuletAutoLinearityTest_calcPredictedAndVariation(AUTOLINEARITYTEST *tst);
static void AmuletAutoLinearityTest_print(void);
void PushPageStack(unsigned char ucPage);
void send_to_amulet_string(uchar ucIndex, char message0[]);

/**
 * \details Handles the Amulet Screen AutoLinearityTest.htm. AutoLinearityTest.htm runs the autolinearity test.
 * \param Amulet_Byte_ID Description
 * \param 80 Language (STATE) English = 0, Spanish = 1
 * \param 81 Show Nuclide Button
 * \param 82 Show Nuclide Text
 * \param 83 Show Interval Button
 * \param 84 Show Interval Text
 * \param 85 Show Total Label and Button
 * \param 86 Show Elapsed, Remaining Text
 * \param 87 Show Start Button
 * \param 88 Show Page Up Button
 * \param 89 Show Page Down Button
 * \param 90 Show Abort Button
 * \param 91 Show Save Button
 * \param 92 Show Inactivate Button
 * \param 93 Show Print Button
 * \param 94 Show Comment
 * \param 95 Show Inactivate
 * \param 100 Show All
 * \param 101 Refresh Elapsed/Remaining
 * \param 102 Refresh Grid, PageUp, PageDown, sfPage
 * \param Amulet_String_ID Description
 * \param 60 Nuclide:
 * \param 61_62 Please Select Nuclide
 * \param 63 Interval:
 * \param 64_65 Enter Interval in minutes:
 * \param 66 Total:
 * \param 67_68 Enter Total Time in hours:
 * \param 69_70 Comment: sfComment
 * \param 71_72 Inactive: sfInactive
 * \param 73 Start
 * \param 74_75 Date Time / Elapsed
 * \param 76 Measured
 * \param 77 Predicted
 * \param 78 % Var
 * \param 79 Abort
 * \param 80 Inactivate
 * \param 81 Save
 * \param 82 Print
 * \param 90 sfPage
 * \param 91 btnNuclideText, sfNuclideText
 * \param 92 btnIntervalText, sfIntervalText
 * \param 93 btnTotalText
 * \param 94_95 sfElapsedRemaining
 * \param 96_97 Title
 * \param 100_101 sfC1R1
 * \param 102 sfC2R1
 * \param 103 sfC3R1
 * \param 104 sfC4R1
 * \param 105_106 sfC1R2
 * \param 107 sfC2R2
 * \param 108 sfC3R2
 * \param 109 sfC4R2
 * \param 110_111 sfC1R3
 * \param 112 sfC2R3
 * \param 113 sfC3R3
 * \param 114 sfC4R3
 * \param 115_116 sfC1R4
 * \param 117 sfC2R4
 * \param 118 sfC3R4
 * \param 119 sfC4R4
 * \param 120_121 sfC1R5
 * \param 122 sfC2R5
 * \param 123 sfC3R5
 * \param 124 sfC4R5
 * \param 125_126 sfC1R6
 * \param 127 sfC2R6
 * \param 128 sfC3R6
 * \param 129 sfC4R6
 * \param 130_131 sfC1R7
 * \param 132 sfC2R7
 * \param 133 sfC3R7
 * \param 134 sfC4R7
 * \param 135_136 sfC1R8
 * \param 137 sfC2R8
 * \param 138 sfC3R8
 * \param 139 sfC4R8
 * \param 140_141 sfC1R9
 * \param 142 sfC2R9
 * \param 143 sfC3R9
 * \param 144 sfC4R9
 * \param 145_146 sfC1R10
 * \param 147 sfC2R10
 * \param 148 sfC3R10
 * \param 149 sfC4R10
 * \returns None
 */
void AmuletAutoLinearityTest_menu(void){
	char message[100], ch1_SN[7], ch2_SN[7], normal[40], normalstring[2], boldstring[2], formatstring[104], titlestring[100];//, field[50];
	char ch_type_string[5];
	int ch_num, index, jndex, total_samples, seconds, elapsed, total_measurements;
	int resumeChamber, resumeNuclideID;
	time_t measurement_t, end_t, sec_stripped;
	struct tm now;
	bool flgOK;
	AUTOLINEARITYTEST *AutoLinearityTestCopy;
	//UINT bytesWritten;

	switch(m_iPhase){
		case PHASE_AUTOLINERITYTEST_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_NUCLIDE_COLON, 60);    // "Nuclide:"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE, 61);    // "Please Select Nuclide"
			send_amulet_message(L_INTERVAL2, 63);    // "Interval:"
			send_amulet_message(L_ENTER_INTERVAL, 64);    // "Enter Interval in minutes:"
			send_amulet_message(L_PR_TOTAL, 66);    // "Total:"
			send_amulet_message(L_ENTER_TOTAL_TIME_IN_HOURS, 67);    // "Enter Total Time in hours:"
			send_amulet_message(L_START, 73);    // "Start"
			send_amulet_message(L_DATE_TIME_ELAPSED, 74);    // "Date Time / Elapsed"
			send_amulet_message(L_MEASURED2, 76);    // "Measured"
			send_amulet_message(L_PREDICTED, 77);    // "Predicted"
			send_amulet_message(L_PERCENT_VAR, 78);    // "% Var"
			send_amulet_message(L_ABORT, 79);    // "Abort"
			send_amulet_message(L_INACTIVATE, 80);    // "Inactivate"
			send_amulet_message(L_SAVE, 81);    // "Save"
			send_amulet_message(L_PRINT, 82);    // "Print"

			if(m_ucClear == 105){
				if(AmuletAutoLinearityTest_mode == 0){			// Running Test
					if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -1){
						AmuletAutoLinearityTest_nuclideName[0] = 0;
						AmuletAutoLinearityTest_intervalMinutes = 0;
						AmuletAutoLinearityTest_totalMinutes = 0;
						AmuletAutoLinearityTest_testCount = 0;
						AmuletAutoLinearityTest_currentPage = 0;
						AmuletAutoLinearityTest_lastPage = 0;
						AmuletAutoLinearityTest_eraseAutoLinearityTest();
					}else{
						strcpy(AmuletAutoLinearityTest_nuclideName, AmuletAutoLinearityTest_test.NuclideName);
						AmuletAutoLinearityTest_intervalMinutes = AmuletAutoLinearityTest_test.IntervalMinutes;
						AmuletAutoLinearityTest_totalMinutes = AmuletAutoLinearityTest_test.TotalMinutes;
						AmuletAutoLinearityTest_testCount = 0;
						AmuletAutoLinearityTest_currentPage = 0;
						AmuletAutoLinearityTest_lastPage = 0;

						AmuletAutoLinearityTest_matchChamberAndNuclide(&resumeChamber, &resumeNuclideID);

						if(resumeChamber == -1){
							get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
							get_amulet_message(L_UNABLE_TO_FIND_CHAMBER, formatstring);    // "Unable to find Chamber"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}else{
							current.main_chamber = resumeChamber;
						}

						if(resumeNuclideID == -1){
							get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
							get_amulet_message(L_UNABLE_TO_FIND_NUCLIDE, formatstring);    // "Unable to find Nuclide"
							Amulet_DisplayError(titlestring, formatstring, TRUE);
							return;
						}else{
							AmuletAutoLinearityTest_nuclideIndex = resumeNuclideID;
							chamber[current.main_chamber].nuc_index = AmuletAutoLinearityTest_nuclideIndex;
							set_nuclide_data(AmuletAutoLinearityTest_nuclideIndex, current.main_chamber);
						}

						AutoLinearityTestCopy = malloc(sizeof(AUTOLINEARITYTEST));
						memcpy(AutoLinearityTestCopy, &AmuletAutoLinearityTest_test, sizeof(AUTOLINEARITYTEST));
						AutoLinearityTestCopy->AutoLinearityTestID = -2;
						AmuletAutoLinearityTest_saveAutoLinearityTest(AutoLinearityTestCopy);
						free(AutoLinearityTestCopy);
					}
				}else if(AmuletAutoLinearityTest_mode == 1){	// Viewing Test
					strcpy(AmuletAutoLinearityTest_nuclideName, AmuletAutoLinearityTest_retrieved.NuclideName);
					AmuletAutoLinearityTest_intervalMinutes = AmuletAutoLinearityTest_retrieved.IntervalMinutes;
					//AmuletAutoLinearityTest_totalMinutes = AmuletAutoLinearityTest_retrieved.TotalMinutes;
					AmuletAutoLinearityTest_testCount = 0;
					AmuletAutoLinearityTest_currentPage = 0;
					AmuletAutoLinearityTest_lastPage = 0;
				}
				m_ucClear = 0;
			}

			if(AmuletAutoLinearityTest_mode == 0){			// Running Test
				ch_num = current.main_chamber;
				if(chamber_type(ch_num) == R_CHAMB) strcpy(ch_type_string, "R");
				else if(chamber_type(ch_num) == P_CHAMB) strcpy(ch_type_string, "PET");
				else if(chamber_type(ch_num) == B_CHAMB) strcpy(ch_type_string, "BT");
				else if(chamber_type(ch_num) == ONE_DOT_EIGHT_CHAMB) strcpy(ch_type_string, "1.8 Atm");
				else if(chamber_type(ch_num) == C_CHAMB) strcpy(ch_type_string, "HR");
				else if(chamber_type(ch_num) == K_CHAMB) strcpy(ch_type_string, "1K");
				else strcpy(ch_type_string, "");
				get_amulet_message(L_AUTOLINEARITY_TEST, formatstring);    // "AutoLinearity Test - Ch:%d, %s"
				sprintf(message, formatstring, current.main_chamber + 1, ch_type_string);
				send_to_amulet_string(96, message);

				send_to_amulet_string(91, AmuletAutoLinearityTest_nuclideName);

				if(AmuletAutoLinearityTest_intervalMinutes == 0){
					message[0] = 0;
				}else{
					sprintf(message, "%d mins", AmuletAutoLinearityTest_intervalMinutes);
				}
				send_to_amulet_string(92, message);

				if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -1){			// Ready
					SetAmuletByte(81, 0xFF);	// Show Nuclide Button
					SetAmuletByte(82, 0x00);	// Hide Nuclide Text

					SetAmuletByte(83, 0xFF);	// Show Interval Button
					SetAmuletByte(84, 0x00);	// Hide Interval Text

					if(AmuletAutoLinearityTest_totalMinutes == 0){
						message[0] = 0;
					}else{
						get_amulet_message(L_HRS, formatstring);    // "%d hrs"
						sprintf(message, formatstring, AmuletAutoLinearityTest_totalMinutes / 60);
					}
					send_to_amulet_string(93, message);
					SetAmuletByte(85, 0xFF);	// Show Total Label and Button
					SetAmuletByte(86, 0x00);	// Hide Elapsed, Remaining Text

					SetAmuletByte(87, 0xFF);	// Show Start Button
					SetAmuletByte(90, 0x00);	// Hide Abort Button
					SetAmuletByte(91, 0x00);	// Hide Save Button

				}else if(AmuletAutoLinearityTest_test.AutoLinearityTestID < -1){	// Running Test
					SetAmuletByte(81, 0x00);	// Hide Nuclide Button
					SetAmuletByte(82, 0xFF);	// Show Nuclide Text

					SetAmuletByte(83, 0x00);	// Hide Interval Button
					SetAmuletByte(84, 0xFF);	// Show Interval Text

					SetAmuletByte(85, 0x00);	// Hide Total Label and Button
					SetAmuletByte(86, 0xFF);	// Show Elapsed, Remaining Text
					AmuletAutoLinearityTest_Start_t = AmuletAutoLinearityTest_test.StartedOn;
					AmuletAutoLinearityTest_End_t = AmuletAutoLinearityTest_test.StartedOn;
					seconds = AmuletAutoLinearityTest_test.TotalMinutes * 60;
					AmuletAutoLinearityTest_End_t = AmuletAutoLinearityTest_End_t + seconds;
					AmuletAutoLinearityTest_showElapsedRemaining();

					SetAmuletByte(87, 0x00);	// Hide Start Button
					SetAmuletByte(90, 0xFF);	// Show Abort Button

					// Get Current Test Count
					AmuletAutoLinearityTest_testCount = AmuletAutoLinearityTest_getTestCount(&AmuletAutoLinearityTest_test);

					// Get Valid Measurements
					total_measurements = AmuletAutoLinearityTest_getMeasurementCount(&AmuletAutoLinearityTest_test);

					if(total_measurements >= 5) SetAmuletByte(91, 0xFF);	// Show Save Button
					else SetAmuletByte(91, 0x00);							// Hide Save Button

					delayloop(30);
					AmuletAutoLinearityTest_displayPage(&AmuletAutoLinearityTest_test);
					delayloop(70);
				}
				SetAmuletByte(92, 0x00);			// Hide Inactivate Button
				SetAmuletByte(93, 0x00);			// Hide Print Button
				SetAmuletByte(94, 0x00); 			// Hide Comment
				SetAmuletByte(95, 0x00);			// Hide Inactivate
			}else if(AmuletAutoLinearityTest_mode == 1){		// Viewing Test
				if(chamber[0].exists){
					for(index=0; index<6; index++){
						ch1_SN[index] = chamb_vals[0].sn[index];
					}
					ch1_SN[6] = 0;
				}else{
					ch1_SN[0] = 0;
				}

				if(chamber[1].exists){
					for(index=0; index<6; index++){
						ch2_SN[index] = chamb_vals[1].sn[index];
					}
					ch2_SN[6] = 0;
				}else{
					ch2_SN[0] = 0;
				}

				if(strcmp(ch1_SN, AmuletAutoLinearityTest_retrieved.SerialNumber) == 0){
					get_amulet_message(L_AUTOLINEARITY_REPORT1, formatstring);    // "AutoLinearity Report, Ch:1 (%s)"
					sprintf(message, formatstring, AmuletAutoLinearityTest_retrieved.SerialNumber);
				}else if(strcmp(ch2_SN, AmuletAutoLinearityTest_retrieved.SerialNumber) == 0){
					get_amulet_message(L_AUTOLINEARITY_REPORT2, formatstring);    // "AutoLinearity Report, Ch:2 (%s)"
					sprintf(message, formatstring, AmuletAutoLinearityTest_retrieved.SerialNumber);
				}else{
					get_amulet_message(L_AUTOLINEARITY_REPORT3, formatstring);    // "AutoLinearity Report, S/N: %s"
					sprintf(message, formatstring, AmuletAutoLinearityTest_retrieved.SerialNumber);
				}
				send_to_amulet_string(96, message);

				strcpy(message, AmuletAutoLinearityTest_nuclideName);
				trim_and_shrink(message);
				send_to_amulet_string(91, message);
				SetAmuletByte(81, 0x00);	// Hide Nuclide Button
				SetAmuletByte(82, 0xFF);	// Show Nuclide Text

				sprintf(message, "%d mins", AmuletAutoLinearityTest_intervalMinutes);
				send_to_amulet_string(92, message);
				SetAmuletByte(83, 0x00);	// Hide Interval Button
				SetAmuletByte(84, 0xFF);	// Show Interval Text

				SetAmuletByte(85, 0x00);	// Hide Total Label and Button
				SetAmuletByte(86, 0x00);	// Hide Elapsed, Remaining Text

				SetAmuletByte(87, 0x00);	// Hide Start Button
				SetAmuletByte(90, 0x00);	// Hide Abort Button
				SetAmuletByte(91, 0x00);	// Hide Save Button

				if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER)) SetAmuletByte(93, 0xFF);	// Show Print Button
				else SetAmuletByte(93, 0x00);																					// Hide Print Button

				boldstring[0] = 5;
				boldstring[1] = 0;
				normalstring[0] = 3;
				normalstring[1] = 0;
				strcpy(message, AmuletAutoLinearityTest_retrieved.Comment);
				trim(message);
				if(message[0] == 0){
					SetAmuletByte(94, 0x00);	// Hide Comment
				}else{
					get_amulet_message(L_COMMENT, formatstring);    // "Comment:"
					sprintf(normal, "%s%s%s %s", boldstring, formatstring, normalstring, message);
					send_to_amulet_string(69, normal);
					SetAmuletByte(94, 0xFF);	// Show Comment
				}

				if(AmuletAutoLinearityTest_retrieved.Inactive){
					SetAmuletByte(92, 0x00);
					strcpy(message, AmuletAutoLinearityTest_retrieved.InactiveReason);
					trim(message);
					get_amulet_message(L_INACTIVE, formatstring);    // "Inactive:"
					sprintf(normal, "%s%s%s %s", boldstring, formatstring, normalstring, message);
					send_to_amulet_string(71, normal);
					SetAmuletByte(95, 0xFF);
				}else{
					SetAmuletByte(92, 0xFF);
					SetAmuletByte(95, 0x00);
				}

				AmuletAutoLinearityTest_testCount = AmuletAutoLinearityTest_getTestCount(&AmuletAutoLinearityTest_retrieved);
				AmuletAutoLinearityTest_calcPredictedAndVariation(&AmuletAutoLinearityTest_retrieved);

				delayloop(30);
				AmuletAutoLinearityTest_displayPage(&AmuletAutoLinearityTest_retrieved);
				delayloop(70);
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_AUTOLINERITYTEST_WAIT;
			break;

		case PHASE_AUTOLINERITYTEST_WAIT:
			if(AmuletAutoLinearityTest_mode == 0){
				if(AmuletAutoLinearityTest_test.AutoLinearityTestID < -1){ // Running, Paused, Recovery
					if(clock_time != AmuletAutoLinearityTest_displayed_t){
						AmuletAutoLinearityTest_showElapsedRemaining();

						gmtime_r(&clock_time, &now);
						now.tm_sec = 0;
						sec_stripped = mk_time(&now);

						for(index=0; index<1210; index++){
							if(AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -2){ 		// Awaiting Measurement
								if(AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredOn < sec_stripped){			// Measurement time has passed
									if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -3) AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = -1;		// Mark as Skipped Paused
									else if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -4) AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = -2;	// Mark as Skipped Recovery
									else AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = -1;																// Mark as Skipped Paused
									AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].AutoLinearityMeasurementID = -3;												// Mark as Measured
								}else if(AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredOn == sec_stripped){	// Measurement time has arrived
									ch_num = current.main_chamber;
									flgOK = TRUE;
									for(jndex=0; jndex<6; jndex++){
										if(AmuletAutoLinearityTest_test.SerialNumber[jndex] != chamb_vals[ch_num].sn[jndex]){
											flgOK = FALSE;
											break;
										}
									}
									if(flgOK){																						// Check if Measurement from Chamber has the same serial number
										if(chamber[ch_num].nuc_index == AmuletAutoLinearityTest_nuclideIndex){						// Check if NuclideID from Chamber is correct
											if(measurement[ch_num].over_flag){
												AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = 1;
											}else{
												if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -2) AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = 0;	// Mark as normal reading
												else if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -3){
													AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = 2;		// Mark as reading after pause
													AmuletAutoLinearityTest_test.AutoLinearityTestID = -2;
												}else if(AmuletAutoLinearityTest_test.AutoLinearityTestID == -4){
													AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = 3;		// Mark as reading after power failure
													AmuletAutoLinearityTest_test.AutoLinearityTestID = -2;
												}else{
													AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = 0;		// Default to normal reading
													AmuletAutoLinearityTest_test.AutoLinearityTestID = -2;
												}

												AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredOn = clock_time;						// Record actual measurement time
												AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredActivity = measurement[ch_num].act0;	// Record activity
											}
											AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].AutoLinearityMeasurementID = -3;	// Mark as measured

											AmuletAutoLinearityTest_testCount = index + 1;		// Total Test counts

											AmuletAutoLinearityTest_calcPredictedAndVariation(&AmuletAutoLinearityTest_test);
											total_measurements = AmuletAutoLinearityTest_getMeasurementCount(&AmuletAutoLinearityTest_test);
											if(total_measurements >= 5) SetAmuletByte(91, 0xFF);

											// Display List
											AmuletAutoLinearityTest_displayPage(&AmuletAutoLinearityTest_test);

											// Save Copy to Disk
											if(AmuletAutoLinearityTest_test.AutoLinearityTestID != -2){
												AutoLinearityTestCopy = malloc(sizeof(AUTOLINEARITYTEST));
												memcpy(AutoLinearityTestCopy, &AmuletAutoLinearityTest_test, sizeof(AUTOLINEARITYTEST));
												AutoLinearityTestCopy->AutoLinearityTestID = -2;
												AmuletAutoLinearityTest_saveAutoLinearityTest(AutoLinearityTestCopy);
												free(AutoLinearityTestCopy);
											}else{
												AmuletAutoLinearityTest_saveAutoLinearityTest(&AmuletAutoLinearityTest_test);
											}
											break;
										}else{
											get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
											get_amulet_message(L_CURRENT_NUCLIDE_IS_INCORRECT, formatstring);    // "Current Nuclide is incorrect"
											Amulet_DisplayError(titlestring, formatstring, TRUE);
											return;
										}
									}else{
										get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
										get_amulet_message(L_CHAMBER_SN_IS_INCORRECT, formatstring);    // "Chamber S/N is incorrect"
										Amulet_DisplayError(titlestring, formatstring, TRUE);
										return;
									}
								}else{
									break;
								}
							}else if(AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -1){
								break; // Reached the end of list
							}
						}
					}
				}
			}
			break;

		case PHASE_AUTOLINERITYTEST_START:
			beep_amulet();
			if(AmuletAutoLinearityTest_nuclideName[0] == 0){
				get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
				get_amulet_message(L_MISSING_NUCLIDE, formatstring);    // "Missing Nuclide"
				Amulet_DisplayError(titlestring, formatstring, TRUE);
				return;
			}else if(AmuletAutoLinearityTest_intervalMinutes == 0){
				get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
				get_amulet_message(L_MISSING_INTERVAL, formatstring);    // "Missing Interval"
				Amulet_DisplayError(titlestring, formatstring, TRUE);
				return;
			}else if(AmuletAutoLinearityTest_totalMinutes == 0){
				get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
				get_amulet_message(L_MISSING_TOTAL_TIME, formatstring);    // "Missing Total Time"
				Amulet_DisplayError(titlestring, formatstring, TRUE);
				return;
			}else{
				total_samples = AmuletAutoLinearityTest_totalMinutes / AmuletAutoLinearityTest_intervalMinutes;
				total_samples++;
				if(total_samples < 5){
					get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
					get_amulet_message(L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4, formatstring);    // "Number of measurements has to be greater than 4"
					Amulet_DisplayError(titlestring, formatstring, TRUE);
					return;
				}else{
					ch_num = current.main_chamber;
					// Switch Nuclide
					AmuletAutoLinearityTest_nuclideIndex = NuclideData_getIndexFromNameIncludingUser(AmuletAutoLinearityTest_nuclideName);
					if(AmuletAutoLinearityTest_nuclideIndex == -1){
						get_amulet_message(L_AUTOLINEARITY, titlestring);    // "AutoLinearity"
						get_amulet_message(L_UNABLE_TO_FIND_NUCLIDE, formatstring);    // "Unable to find Nuclide"
						Amulet_DisplayError(titlestring, formatstring, TRUE);
						return;
					}else{
						AmuletAutoLinearityTest_clearTest();

						chamber[ch_num].nuc_index = AmuletAutoLinearityTest_nuclideIndex;
						set_nuclide_data(AmuletAutoLinearityTest_nuclideIndex, ch_num);

						AmuletAutoLinearityTest_test.AutoLinearityTestID = -2;
						for(index=0; index<6; index++) AmuletAutoLinearityTest_test.SerialNumber[index] = chamb_vals[ch_num].sn[index];
						AmuletAutoLinearityTest_test.SerialNumber[6] = 0;
						AmuletAutoLinearityTest_test.Type = chamb_vals[ch_num].chamb_type;
						strcpy(AmuletAutoLinearityTest_test.NuclideName, AmuletAutoLinearityTest_nuclideName);
						AmuletAutoLinearityTest_test.HalfLife = NuclideData_getHalflife(AmuletAutoLinearityTest_nuclideIndex);
						AmuletAutoLinearityTest_test.HalfLifeUnit = NuclideData_getHalflifeUnit(AmuletAutoLinearityTest_nuclideIndex);
						AmuletAutoLinearityTest_test.IntervalMinutes = AmuletAutoLinearityTest_intervalMinutes;
						AmuletAutoLinearityTest_test.TotalMinutes = AmuletAutoLinearityTest_totalMinutes;
						gmtime_r(&clock_time, &now);
						now.tm_sec = 0;
						AmuletAutoLinearityTest_test.StartedOn = mk_time(&now);
						AmuletAutoLinearityTest_test.StartedOn = AmuletAutoLinearityTest_test.StartedOn + 120; // Start in two minutes
						end_t = AmuletAutoLinearityTest_test.StartedOn;
						seconds = AmuletAutoLinearityTest_test.TotalMinutes * 60;
						end_t = end_t + seconds;
						AmuletAutoLinearityTest_Start_t = AmuletAutoLinearityTest_test.StartedOn;
						AmuletAutoLinearityTest_End_t = end_t;
						measurement_t = AmuletAutoLinearityTest_test.StartedOn;
						index = 0;
						elapsed = 0;
						while((index < 1210) && measurement_t <= end_t){
							AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].AutoLinearityMeasurementID = -2;
							AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredActivity = 0;
							AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredOn = measurement_t;
							AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = 0;
							AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].ElapsedMinutes = elapsed;
							AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].PredictedActivity = 0;
							AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].PercentVariation = 0;
							elapsed += AmuletAutoLinearityTest_test.IntervalMinutes;
							seconds = AmuletAutoLinearityTest_test.IntervalMinutes * 60;
							measurement_t = measurement_t + seconds;
							index++;
						}
						AmuletAutoLinearityTest_showElapsedRemaining();
						AmuletAutoLinearityTest_saveAutoLinearityTest(&AmuletAutoLinearityTest_test);
						SetAmuletByte(81, 0x00); // Hide Nuclide Button
						SetAmuletByte(82, 0xFF); // Show Nuclide Text
						SetAmuletByte(83, 0x00); // Hide Interval Button
						SetAmuletByte(84, 0xFF); // Show Interval Text
						SetAmuletByte(85, 0x00); // Hide Total Label and Button
						SetAmuletByte(86, 0xFF); // Show Elapsed, Remaining Text
						SetAmuletByte(87, 0x00); // Hide Start Button
						SetAmuletByte(90, 0xFF); // Show Abort Button
						SetAmuletByte(91, 0x00); // Hide Save Button
						SetAmuletByte(100, 0xFF); // Show All
					}
				}
			}
			m_iPhase = PHASE_AUTOLINERITYTEST_WAIT;
			break;

		case PHASE_AUTOLINERITYTEST_ABORT:
			beep_amulet();
			AmuletAutoLinearityTest_test.AutoLinearityTestID = -3;
			AmuletAutoLinearityTest_saveAutoLinearityTest(&AmuletAutoLinearityTest_test);
			AmuletGenericYesNo_config = 4;
			SetAmuletHTML(AmuletHTMLIndex[GENERICYESNO_HTM]);
			PushPageStack(AmuletHTMLIndex[GENERICYESNO_HTM]);
			return;

		case PHASE_AUTOLINERITYTEST_HOME:
			beep_amulet();
			if(AmuletAutoLinearityTest_mode == 0){
				if(AmuletAutoLinearityTest_test.AutoLinearityTestID < -1){
					AmuletAutoLinearityTest_test.AutoLinearityTestID = -3;
					AmuletAutoLinearityTest_saveAutoLinearityTest(&AmuletAutoLinearityTest_test);
					AmuletGenericYesNo_config = 2;
					SetAmuletHTML(AmuletHTMLIndex[GENERICYESNO_HTM]);
					PushPageStack(AmuletHTMLIndex[GENERICYESNO_HTM]);
				}else{
					SetAmuletHomeHTML();
				}
			}else{
				SetAmuletHomeHTML();
			}
			return;

		case PHASE_AUTOLINERITYTEST_BACK:
			beep_amulet();
			if(AmuletAutoLinearityTest_mode == 0){
				if(AmuletAutoLinearityTest_test.AutoLinearityTestID < -1){
				AmuletAutoLinearityTest_test.AutoLinearityTestID = -3;
				AmuletAutoLinearityTest_saveAutoLinearityTest(&AmuletAutoLinearityTest_test);
				AmuletGenericYesNo_config = 3;
				SetAmuletHTML(AmuletHTMLIndex[GENERICYESNO_HTM]);
				PushPageStack(AmuletHTMLIndex[GENERICYESNO_HTM]);
				}else{
					SetAmuletBackHTML();
				}
			}else{
				SetAmuletBackHTML();
			}
			return;

		case PHASE_AUTOLINERITYTEST_PGDOWN:
			beep_amulet();
			if(AmuletAutoLinearityTest_currentPage < AmuletAutoLinearityTest_lastPage){
				AmuletAutoLinearityTest_currentPage++;
				if(AmuletAutoLinearityTest_mode == 0) AmuletAutoLinearityTest_displayPage(&AmuletAutoLinearityTest_test);
				else if(AmuletAutoLinearityTest_mode == 1) AmuletAutoLinearityTest_displayPage(&AmuletAutoLinearityTest_retrieved);
			}
			m_iPhase = PHASE_AUTOLINERITYTEST_WAIT;
			break;

		case PHASE_AUTOLINERITYTEST_PGUP:
			beep_amulet();
			if(AmuletAutoLinearityTest_currentPage){
				AmuletAutoLinearityTest_currentPage--;
				if(AmuletAutoLinearityTest_mode == 0) AmuletAutoLinearityTest_displayPage(&AmuletAutoLinearityTest_test);
				else if(AmuletAutoLinearityTest_mode == 1) AmuletAutoLinearityTest_displayPage(&AmuletAutoLinearityTest_retrieved);
			}
			m_iPhase = PHASE_AUTOLINERITYTEST_WAIT;
			break;

		case PHASE_AUTOLINERITYTEST_SAVE:
			beep_amulet();
			AmuletAutoLinearityTest_test.AutoLinearityTestID = -3;
			AmuletAutoLinearityTest_saveAutoLinearityTest(&AmuletAutoLinearityTest_test);
			AmuletGenericYesNo_config = 5;
			AmuletGenericYesNo_comment[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[GENERICYESNO_HTM]);
			PushPageStack(AmuletHTMLIndex[GENERICYESNO_HTM]);
			return;

		case PHASE_AUTOLINERITYTEST_PRINT:
			beep_amulet();
			AmuletAutoLinearityTest_print();
			m_ucClear = 105;
			m_iPhase = PHASE_AUTOLINERITYTEST_PRE_INIT;
			break;

/*		case PHASE_AUTOLINERITYTEST_EXPORT:
			//beep_amulet();
			if(m_ucEnumerated == 2){
				//sprintf(AutoLinFileName, "AutoLinearity_%lld.csv", AmuletAutoLinearityTest_retrieved.AutoLinearityTestID);
				strcpy(AmuletAutoLinearityTest_AutoLinFileName, "1:/1.csv");
				//AmuletAutoLinearityTest_fileInfo.lfname = AmuletAutoLinearityTest_longFileName;
				//AmuletAutoLinearityTest_fileInfo.lfsize = 100;
				AmuletAutoLinearityTest_Result = f_chdrive(1);
				f_chdir("/");
				f_opendir(&AmuletAutoLinearityTest_dirObject, ".");
				f_chdrive(0);
				//if(!AmuletAutoLinearityTest_Result){
					//AmuletAutoLinearityTest_Result = f_chdir("/");
					//if(!AmuletAutoLinearityTest_Result){
						if(f_stat(AmuletAutoLinearityTest_AutoLinFileName, &AmuletAutoLinearityTest_fileInfo) == FR_OK) f_unlink(AmuletAutoLinearityTest_AutoLinFileName);
						f_open(&AmuletAutoLinearityTest_fileObject, AmuletAutoLinearityTest_AutoLinFileName, FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
						message[0] = 0;
						wrap_and_append(message, "AutoLinearity Test");
						wrap_and_append(message, "");
						wrap_and_append(message, "");
						wrap_and_append(message, "");
						wrap_and_append(message, "");
						wrap_and_append(message, "");
						end_line(message);
						//strcpy(message, "hello world");
						//f_write(&AmuletAutoLinearityTest_fileObject, (void *) message, strlen(message), &bytesWritten);
						f_close(&AmuletAutoLinearityTest_fileObject);
					//}
				//}

				SetAmuletByte(100, 0xFF);
				m_iPhase = PHASE_AUTOLINERITYTEST_WAIT;
				if(AmuletAutoLinearityTest_Result){
					//Amulet_DisplayError("AutoLinearity", "USB Drive Error", TRUE);
				}else{
				//	sprintf(message, "Exported File Name: %s", AmuletAutoLinearityTest_AutoLinFileName);
					//Amulet_DisplayNotification("AutoLinearity", "A-OK", TRUE);
				}
				//return;
			}else{
				Amulet_DisplayError("AutoLinearity", "Unable to detect USB Drive", TRUE);
				return;
			}
			break;*/
	}
}

static void AmuletAutoLinearityTest_displayPage(AUTOLINEARITYTEST *tst){
	int index;
	char message[51],datetime_string[51], formatstring[100];

	AmuletAutoLinearityTest_lastPage = AmuletAutoLinearityTest_testCount / 10;
	if(AmuletAutoLinearityTest_testCount % 10) AmuletAutoLinearityTest_lastPage++;
	AmuletAutoLinearityTest_lastPage--;
	if(AmuletAutoLinearityTest_lastPage < 0) AmuletAutoLinearityTest_lastPage = 0;

	if(AmuletAutoLinearityTest_lastPage == 0){
		SetAmuletByte(88, 0x00); // Hide Up Button
		SetAmuletByte(89, 0x00); // Hide Down Button
	}else{
		if(AmuletAutoLinearityTest_currentPage == 0){
			SetAmuletByte(88, 0x00); // Hide Up Button
			SetAmuletByte(89, 0xFF); // Show Down Button
		}else if(AmuletAutoLinearityTest_currentPage == AmuletAutoLinearityTest_lastPage){
			SetAmuletByte(88, 0xFF); // Show Up Button
			SetAmuletByte(89, 0x00); // Hide Down Button
		}else{
			SetAmuletByte(88, 0xFF); // Show Up Button
			SetAmuletByte(89, 0xFF); // Show Down Button
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletAutoLinearityTest_currentPage + index) < AmuletAutoLinearityTest_testCount){
			if((tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].AutoLinearityMeasurementID < -2) || (tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].AutoLinearityMeasurementID >= 0)){
				GetExtendedTimeInfoLanguageSec(&(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].MeasuredOn), datetime_string);
				get_amulet_message(L_ELAPSED, formatstring);    // "%s\nElapsed: %d min"
				sprintf(message,formatstring, datetime_string, tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].ElapsedMinutes);
				send_to_amulet_string(5*index + 100, message);

				if((tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].AutoLinearityMeasurementID == -3) || (tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].AutoLinearityMeasurementID >= 0)){
					if(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].Status == 1){
						//SetAmuletString(5*index + 102, "Over Range");
						send_amulet_message(L_OVERRANGE2, 5*index + 102);    // "Over Range"
						send_to_amulet_string(5*index + 104, "");
					}else if(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].Status == -1){
						//SetAmuletString(5*index + 102, "Paused");
						send_amulet_message(L_PAUSED, 5*index + 102);    // "Paused"
						send_to_amulet_string(5*index + 104, "");
					}else if(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].Status == -2){
						//SetAmuletString(5*index + 102, "Power Fail");
						send_amulet_message(L_POWER_FAIL, 5*index + 102);    // "Power Fail"
						send_to_amulet_string(5*index + 104, "");
					}else{
						format_activity_system(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].MeasuredActivity, message);
						//if(AmuletAutoLinearityTest_test.AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].Status == 2) strcat(message, " PAUSED");
						//else if(AmuletAutoLinearityTest_test.AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].Status == 3) strcat(message, " POWER");
						send_to_amulet_string(5*index + 102, message);
						if(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].PredictedActivity > -990.0){
							sprintf(message, "%.1f", tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].PercentVariation);
							send_to_amulet_string(5*index + 104, message);

						}else{
							send_to_amulet_string(5*index + 104, "");
						}
					}
				}

				if(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].PredictedActivity > -990.0){
					format_activity_system(tst->AutoLinearityMeasurement[10 * AmuletAutoLinearityTest_currentPage + index].PredictedActivity, message);
					send_to_amulet_string(5*index + 103, message);
				}
			}else{
				send_to_amulet_string(5*index + 100, "");
				send_to_amulet_string(5*index + 102, "");
				send_to_amulet_string(5*index + 103, "");
				send_to_amulet_string(5*index + 104, "");
			}
		}else{
			send_to_amulet_string(5*index + 100, "");
			send_to_amulet_string(5*index + 102, "");
			send_to_amulet_string(5*index + 103, "");
			send_to_amulet_string(5*index + 104, "");
		}
	}

	message[0] = 0;
	if(AmuletAutoLinearityTest_lastPage > 0){
		get_amulet_message(L_OF, formatstring);    // "%d of %d"
		sprintf(message, formatstring, AmuletAutoLinearityTest_currentPage + 1, AmuletAutoLinearityTest_lastPage + 1);
	}
	send_to_amulet_string(90, message);

	SetAmuletByte(102, 0xFF);
}

static void AmuletAutoLinearityTest_print(void){
	char message[90], ch1_SN[7], ch2_SN[7], autolinearityteststring[100], amstring1[100], amstring2[100], outputstring[100];
	char prtype;
	char strng[90];
	int index;

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		get_amulet_message(L_AUTOLINEARITY_TEST2, autolinearityteststring);    // "AutoLinearity Test"
		rawheader_languagesec(prtype, autolinearityteststring, clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		if(chamber[0].exists){
			for(index=0; index<6; index++){
				ch1_SN[index] = chamb_vals[0].sn[index];
			}
			ch1_SN[6] = 0;
		}else{
			ch1_SN[0] = 0;
		}

		if(chamber[1].exists){
			for(index=0; index<6; index++){
				ch2_SN[index] = chamb_vals[1].sn[index];
			}
			ch2_SN[6] = 0;
		}else{
			ch2_SN[0] = 0;
		}

		if(strcmp(ch1_SN, AmuletAutoLinearityTest_retrieved.SerialNumber) == 0){
			//insertconst(strng, 11, 1, 0, "Ch: 1");
			insertconst(strng, 0, 0, 0, "Ch: 1");
			pr_write2_languagesec(strng, autolinearityteststring);
		}else if(strcmp(ch2_SN, AmuletAutoLinearityTest_retrieved.SerialNumber) == 0){
			//insertconst(strng, 11, 1, 0, "Ch: 2");
			insertconst(strng, 0, 0, 0, "Ch: 2");
			pr_write2_languagesec(strng, autolinearityteststring);
		}

		//insertconst(strng, 9, 1, 0, "S/N:");
		//insertconst(strng, 11, 0, 0, AmuletAutoLinearityTest_retrieved.SerialNumber);
		get_amulet_message(L_SN2, amstring1);    // "S/N:"
		sprintf(outputstring, "%s %s", amstring1, AmuletAutoLinearityTest_retrieved.SerialNumber);
		strin(strng, outputstring);
		pr_write2_languagesec(strng, autolinearityteststring);

		//insertconst(strng, 9, 1, 0, "Type:");
		//if(AmuletAutoLinearityTest_retrieved.Type == R_CHAMB) insertconst(strng, 11, 0, 0, "R Chamber");
		//else if(AmuletAutoLinearityTest_retrieved.Type == P_CHAMB) insertconst(strng, 11, 0, 0, "PET Chamber");
		get_amulet_message(L_TYPE, amstring1);    // "Type:"
		if(AmuletAutoLinearityTest_retrieved.Type == R_CHAMB) get_amulet_message(L_R_CHAMBER, amstring2);    // "R Chamber"
		else if(AmuletAutoLinearityTest_retrieved.Type == P_CHAMB) get_amulet_message(L_P_CHAMBER, amstring2);    // "P Chamber"
		else if(AmuletAutoLinearityTest_retrieved.Type == B_CHAMB) get_amulet_message(L_BT_CHAMBER, amstring2);    // "BT Chamber"
		else if(AmuletAutoLinearityTest_retrieved.Type == ONE_DOT_EIGHT_CHAMB) get_amulet_message(L_18_CHAMBER, amstring2);    // "1.8 Atm Chamber"
		else if(AmuletAutoLinearityTest_retrieved.Type == C_CHAMB) get_amulet_message(L_C_CHAMBER, amstring2);    // "HR Chamber"
		else if(AmuletAutoLinearityTest_retrieved.Type == K_CHAMB) get_amulet_message(L_K_CHAMBER, amstring2);    // "1K Chamber"
		sprintf(outputstring, "%s %s", amstring1, amstring2);
		strin(strng, outputstring);
		pr_write2_languagesec(strng, autolinearityteststring);

		//insertconst(strng, 9, 1, 0, "Started:");
		//inserttime_t_language(strng, 11, 0, 0, AmuletAutoLinearityTest_retrieved.StartedOn, FALSE);
		get_amulet_message(L_STARTED, amstring1);    // "Started:"
		GetExtendedTimeInfoLanguageSec(&AmuletAutoLinearityTest_retrieved.StartedOn, amstring2);
		sprintf(outputstring, "%s %s", amstring1, amstring2);
		strin(strng, outputstring);
		pr_write2_languagesec(strng, autolinearityteststring);

		//insertconst(strng, 9, 1, 0, "Nuclide:");
		//insertstring(strng, 11, 0, 0, "%s", AmuletAutoLinearityTest_retrieved.NuclideName);
		get_amulet_message(L_NUCLIDE_COLON, amstring1);    // "Nuclide:"
		sprintf(outputstring, "%s %s", amstring1, AmuletAutoLinearityTest_retrieved.NuclideName);
		strin(strng, outputstring);
		pr_write2_languagesec(strng, autolinearityteststring);

		//insertconst(strng, 9, 1, 0, "HalfLife:");
		get_amulet_message(L_PR_HALFLIFE2, amstring1);    // "Halflife:"
		sprintf(message, "%.2f", AmuletAutoLinearityTest_retrieved.HalfLife);
		switch(AmuletAutoLinearityTest_retrieved.HalfLifeUnit){
			case SEC:
				//strcat(message, " sec");
				get_amulet_message(L_SEC2, amstring2);    // " sec"
				strcat(message, amstring2);
				break;

			case MIN:
				//strcat(message, " min");
				get_amulet_message(L_MIN2, amstring2);    // " min"
				strcat(message, amstring2);
				break;

			case HOUR:
				//strcat(message, " hr");
				get_amulet_message(L_HR2, amstring2);    // " hr"
				strcat(message, amstring2);
				break;

			case DAY:
				//strcat(message, " day");
				get_amulet_message(L_DAY2, amstring2);    // " day"
				strcat(message, amstring2);
				break;

			case YEAR:
				if(AmuletAutoLinearityTest_retrieved.HalfLife > 1.0e+6){
					sprintf(message, "%.2f", AmuletAutoLinearityTest_retrieved.HalfLife / 1.0e+6);
					//strcat(message, " Myr");
					get_amulet_message(L_MYR, amstring2);    // " Myr"
					strcat(message, amstring2);
				}else{
					//strcat(message, " yr");
					get_amulet_message(L_YR2, amstring2);    // " yr"
					strcat(message, amstring2);
				}
				break;
		}
		//insertstring(strng, 11, 0, 0, "%s", message);
		sprintf(outputstring, "%s %s", amstring1, message);
		strin(strng, outputstring);
		pr_write2_languagesec(strng, autolinearityteststring);

		//insertconst(strng, 9, 1, 0, "Interval:");
		get_amulet_message(L_INTERVAL2, amstring1);    // "Interval:"
		sprintf(message, "%d mins", AmuletAutoLinearityTest_retrieved.IntervalMinutes);
		//insertstring(strng, 11, 0, 0, "%s", message);
		sprintf(outputstring, "%s %s", amstring1, message);
		strin(strng, outputstring);
		pr_write2_languagesec(strng, autolinearityteststring);

		strcpy(message, AmuletAutoLinearityTest_retrieved.Comment);
		trim(message);
		if(message[0] != 0){
			//insertconst(strng, 9, 1, 0, "Comment:");
			get_amulet_message(L_COMMENT, amstring1);    // "Comment:"
			//insertstring(strng, 11, 0, 0, "%s", message);
			sprintf(outputstring, "%s %s", amstring1, message);
			strin(strng, outputstring);
			pr_write2_languagesec(strng, autolinearityteststring);
		}

		if(AmuletAutoLinearityTest_retrieved.Inactive){
			//insertconst(strng, 9, 1, 0, "Inactive:");
			get_amulet_message(L_INACTIVE, amstring1);    // "Inactive:"
			strcpy(message, AmuletAutoLinearityTest_retrieved.InactiveReason);
			trim(message);
			//insertstring(strng, 11, 0, 0, "%s", message);
			sprintf(outputstring, "%s %s", amstring1, message);
			strin(strng, outputstring);
			pr_write2_languagesec(strng, autolinearityteststring);
		}

		AmuletAutoLinearityTest_testCount = AmuletAutoLinearityTest_getTestCount(&AmuletAutoLinearityTest_retrieved);

		for(index=0; index<AmuletAutoLinearityTest_testCount; index++){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2_languagesec(strng, autolinearityteststring);

			//inserttime_t_language(strng, 0, 0, 0, AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].MeasuredOn, FALSE);
			GetExtendedTimeInfoLanguageSec(&AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].MeasuredOn, amstring1);
			//insertint(strng, 17, 0, 0, ", %d mins", AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].ElapsedMinutes);
			sprintf(outputstring, "%s, %d mins", amstring1, AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].ElapsedMinutes);
			strin(strng, outputstring);
			pr_write2_languagesec(strng, autolinearityteststring);

			if(AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].Status == 1){
				//insertconst(strng, 32, 1, 0, "Over Range");
				get_amulet_message(L_OVERRANGE2, amstring1);    // "Over Range"
				strin(strng, amstring1);
				pr_write2_languagesec(strng, autolinearityteststring);
			}else if(AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].Status == -1){
				//insertconst(strng, 32, 1, 0, "Paused");
				get_amulet_message(L_PAUSED, amstring1);    // "Paused"
				strin(strng, amstring1);
				pr_write2_languagesec(strng, autolinearityteststring);
			}else if(AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].Status == -2){
				//insertconst(strng, 32, 1, 0, "Power Fail");
				get_amulet_message(L_POWER_FAIL, amstring1);    // "Power Fail"
				strin(strng, amstring1);
				pr_write2_languagesec(strng, autolinearityteststring);
			}else{
				//insertconst(strng, 0, 0, 0, " Measured:");
				get_amulet_message(L_MEASURED, amstring1);    // "Measured:"
				format_activity_system(AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].MeasuredActivity, message);
				replace(message, '$', 'u');
				//insertstring(strng, 32, 1, 0, "%s", message);
				sprintf(outputstring, "%s %s", amstring1, message);
				strin(strng, outputstring);
				pr_write2_languagesec(strng, autolinearityteststring);

				//insertconst(strng, 0, 0, 0, "Predicted:");
				get_amulet_message(L_PREDICTED2, amstring1);    // "Predicted:"
				format_activity_system(AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].PredictedActivity, message);
				replace(message, '$', 'u');
				//insertstring(strng, 32, 1, 0, "%s", message);
				sprintf(outputstring, "%s %s", amstring1, message);
				strin(strng, outputstring);
				pr_write2_languagesec(strng, autolinearityteststring);

				//insertconst(strng, 0, 0, 0,"    % Var:");
				get_amulet_message(L_PERCENT_VAR2, amstring1);    // "% Var:"
				//insertfloat(strng, 32, 1, 0, "%.1f%%",  AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].PercentVariation);
				sprintf(outputstring, "%s %.1f%%", amstring1, AmuletAutoLinearityTest_retrieved.AutoLinearityMeasurement[index].PercentVariation);
				strin(strng, outputstring);
				pr_write2_languagesec(strng, autolinearityteststring);
			}
			if(prtype == ROLL_PRINTER) delayloop(100);
		}
		formfeed(prtype);
	}
}

void AmuletAutoLinearityTest_clearTest(void){
	int index;

	AmuletAutoLinearityTest_test.AutoLinearityTestID = -1;
	AmuletAutoLinearityTest_test.SerialNumber[0] = 0;
	AmuletAutoLinearityTest_test.Type = -1;
	AmuletAutoLinearityTest_test.NuclideName[0] = 0;
	AmuletAutoLinearityTest_test.IntervalMinutes = 0;
	AmuletAutoLinearityTest_test.TotalMinutes = 0;
	AmuletAutoLinearityTest_test.StartedOn = 0;
	AmuletAutoLinearityTest_test.Comment[0] = 0;
	AmuletAutoLinearityTest_test.CreatedOn = 0;
	AmuletAutoLinearityTest_test.InactiveReason[0] = 0;
	AmuletAutoLinearityTest_test.Inactive = FALSE;
	for(index = 0; index<1210; index++){
		AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].AutoLinearityMeasurementID = -1;
		AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredActivity = 0;
		AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].MeasuredOn = 0;
		AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].Status = 0;
		AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].ElapsedMinutes = 0;
		AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].PredictedActivity = 0;
		AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].PercentVariation = 0;
	}
}

static void AmuletAutoLinearityTest_showElapsedRemaining(void){
	char message[52], formatstring[100];
	int elapsed, remaining;
	time_t sec_stripped;
	struct tm now;

	gmtime_r(&clock_time, &now);
	now.tm_sec = 0;
	sec_stripped = mk_time(&now);

	if(sec_stripped > AmuletAutoLinearityTest_End_t){
		//if(AmuletAutoLinearityTest_getMeasurementCount(&AmuletAutoLinearityTest_test) >= 5) strcpy(message, "Finished!!!");
		//else strcpy(message, "Can't be saved.\nLess than 5 measurements");
		if(AmuletAutoLinearityTest_getMeasurementCount(&AmuletAutoLinearityTest_test) >= 5) get_amulet_message(L_FINISHED2, message);    // "Finished!!!"
		else get_amulet_message(L_CANT_BE_SAVED_LESS_THAN_5_MEASUREMENTS, message);    // "Can't be saved.\nLess than 5 measurements"
	}else{
		elapsed = sec_stripped - AmuletAutoLinearityTest_Start_t;
		remaining = AmuletAutoLinearityTest_End_t - sec_stripped;
		get_amulet_message(L_ELAPSED_MINUTES_REMAINING_MINUTES, formatstring);    // "Elapsed Minutes: %d\nRemaining Minutes:%d"
		//sprintf(message, "Elapsed Minutes: %d\nRemaining Minutes:%d", elapsed / 60, remaining / 60);
		sprintf(message, formatstring, elapsed / 60, remaining / 60);
	}
	send_to_amulet_string(94, message);
	AmuletAutoLinearityTest_displayed_t = clock_time;
	SetAmuletByte(101, 0xFF);
}

void AmuletAutoLinearityTest_saveAutoLinearityTest(AUTOLINEARITYTEST *test){
	//FIL fileObject;
	//FILINFO fileInfo;
	//char longFileName[100];
	UINT bytesWritten;

	AmuletAutoLinearityTest_fileInfo.lfname = AmuletAutoLinearityTest_longFileName;
	AmuletAutoLinearityTest_fileInfo.lfsize = 100;

	f_chdrive(0);
	f_chdir("/");
	f_chdir("data");
	if(f_stat("ald.tmp", &AmuletAutoLinearityTest_fileInfo) == FR_OK) f_unlink("ald.tmp");
	f_open(&AmuletAutoLinearityTest_fileObject, "ald.tmp", FA_READ | FA_WRITE | FA_CREATE_ALWAYS);
	f_write(&AmuletAutoLinearityTest_fileObject, (void *) test, sizeof(AUTOLINEARITYTEST), &bytesWritten);
	f_close(&AmuletAutoLinearityTest_fileObject);

	if(bytesWritten == sizeof(AUTOLINEARITYTEST)){
		if(f_stat("ald.dat", &AmuletAutoLinearityTest_fileInfo) == FR_OK) f_unlink("ald.dat");
		f_rename("ald.tmp", "ald.dat");
	}else{
		printf("unsuccessful ald.tmp creation\n");
		if(f_stat("ald.tmp", &AmuletAutoLinearityTest_fileInfo) == FR_OK) f_unlink("ald.tmp");
	}
}

void AmuletAutoLinearityTest_eraseAutoLinearityTest(void){
	//FILINFO fileInfo;
	//char longFileName[100];

	AmuletAutoLinearityTest_fileInfo.lfname = AmuletAutoLinearityTest_longFileName;
	AmuletAutoLinearityTest_fileInfo.lfsize = 100;
	f_chdrive(0);
	f_chdir("/");
	f_chdir("data");
	if(f_stat("ald.tmp", &AmuletAutoLinearityTest_fileInfo) == FR_OK) f_unlink("ald.tmp");
	if(f_stat("ald.dat", &AmuletAutoLinearityTest_fileInfo) == FR_OK) f_unlink("ald.dat");
}

void AmuletAutoLinearityTest_matchChamberAndNuclide(int *resumeChamber, int *resumeNuclideID){
	int index, ch_type, nuclideIndex;
	float response;
	bool flgFoundChamber;

	*resumeChamber = -1;
	*resumeNuclideID = -1;

	flgFoundChamber = FALSE;
	if(chamber[0].exists){
		flgFoundChamber = TRUE;
		for(index=0; index<6; index++){
			if(AmuletAutoLinearityTest_test.SerialNumber[index] != chamb_vals[0].sn[index]){
				flgFoundChamber = FALSE;
				break;
			}
		}
	}
	if(flgFoundChamber) *resumeChamber = 0;
	else{
		if(chamber[1].exists){
			flgFoundChamber = TRUE;
			for(index=0; index<6; index++){
				if(AmuletAutoLinearityTest_test.SerialNumber[index] != chamb_vals[1].sn[index]){
					flgFoundChamber = FALSE;
					break;
				}
			}
		}
		if(flgFoundChamber) *resumeChamber = 1;
	}

	if(*resumeChamber != -1){
		ch_type = chamber_type(*resumeChamber);
		nuclideIndex = NuclideData_getIndexFromNameIncludingUser(AmuletAutoLinearityTest_test.NuclideName);
		if(nuclideIndex != -1){
			response = NuclideData_getEffectiveResponse(nuclideIndex, ch_type);
			if(response > 0.0){
				*resumeNuclideID = nuclideIndex;
			}
		}
	}
}

int AmuletAutoLinearityTest_getTestCount(AUTOLINEARITYTEST *tst){
	int index;
	int returnvalue;

	returnvalue = 1210;
	for(index=0; index<1210; index++){
		// Look until Blank or only Prefilled
		if((tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -1) || (tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -2)){
			returnvalue = index;
			break;
		}
	}

	return returnvalue;
}

int AmuletAutoLinearityTest_getMeasurementCount(AUTOLINEARITYTEST *tst){
	int index, testCount, returnvalue;

	testCount = AmuletAutoLinearityTest_getTestCount(tst);

	returnvalue = 0;
	for(index=0; index<testCount; index++){
		// Count only if measurement is not overrange or skipped
		if((tst->AutoLinearityMeasurement[index].Status != 1) && (tst->AutoLinearityMeasurement[index].Status != -1) && (tst->AutoLinearityMeasurement[index].Status != -2) && ((tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -3) || (tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID >= 0))){
			returnvalue++;
		}
	}
	return returnvalue;
}

void AmuletAutoLinearityTest_calcPredictedAndVariation(AUTOLINEARITYTEST *tst){
	int index, testCount, total_measurements, first_measurement;
	float *x, sx, sy, slope;

	testCount = AmuletAutoLinearityTest_getTestCount(tst);
	total_measurements = AmuletAutoLinearityTest_getMeasurementCount(tst);

	// Clear all predicted activity
	for(index=0; index<testCount; index++) tst->AutoLinearityMeasurement[index].PredictedActivity = - 999.0;

	// Generate predicted activity
	if(total_measurements > 0){
		for(index=0; index<testCount; index++){

			if(((tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -3) || tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID >= 0) && (tst->AutoLinearityMeasurement[index].Status != 1) && (tst->AutoLinearityMeasurement[index].Status != -1) && (tst->AutoLinearityMeasurement[index].Status != -2)){
				first_measurement = index;
				break;
			}
		}

		if(total_measurements < 4){
			for(index=0; index<testCount; index++){
				tst->AutoLinearityMeasurement[index].PredictedActivity = nucdecay(tst->AutoLinearityMeasurement[first_measurement].MeasuredActivity, tst->AutoLinearityMeasurement[first_measurement].MeasuredOn, tst->AutoLinearityMeasurement[index].MeasuredOn, tst->HalfLife, tst->HalfLifeUnit);
			}
		}else{
			x = malloc(1210 * sizeof(float));
			sx = 0.0;
			sy = 0.0;
			for(index=0; index<testCount; index++){
				x[index] = nucdecay(1.0, tst->AutoLinearityMeasurement[first_measurement].MeasuredOn, tst->AutoLinearityMeasurement[index].MeasuredOn, tst->HalfLife, tst->HalfLifeUnit);
				if(((tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -3) || (tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID >= 0)) && (tst->AutoLinearityMeasurement[index].Status != 1) && (tst->AutoLinearityMeasurement[index].Status != -1) && (tst->AutoLinearityMeasurement[index].Status != -2)){
					sx += x[index];
					sy += tst->AutoLinearityMeasurement[index].MeasuredActivity;
				}
			}

			slope = sy / sx;

			for(index=0; index<testCount; index++){
				tst->AutoLinearityMeasurement[index].PredictedActivity = slope * x[index];
			}
			free(x);
		}

		for(index=0; index<testCount; index++){
			// Predicted Activity and Non OverRange Measurement
			if(((tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -3) || (tst->AutoLinearityMeasurement[index].AutoLinearityMeasurementID >= 0)) && (tst->AutoLinearityMeasurement[index].Status != 1) && (tst->AutoLinearityMeasurement[index].Status != -1) && (tst->AutoLinearityMeasurement[index].Status != -2) && (tst->AutoLinearityMeasurement[index].PredictedActivity > -990.0)){
				tst->AutoLinearityMeasurement[index].PercentVariation = tst->AutoLinearityMeasurement[index].MeasuredActivity - tst->AutoLinearityMeasurement[index].PredictedActivity;
				tst->AutoLinearityMeasurement[index].PercentVariation /= tst->AutoLinearityMeasurement[index].PredictedActivity;
				tst->AutoLinearityMeasurement[index].PercentVariation *= 100.0;
			}
		}
	}
}

bool AmuletAutoLinearityTest_isFinished(void){
	bool returnvalue;
	int index;

	returnvalue = TRUE;
	for(index=0; index<1210; index++){
		if(AmuletAutoLinearityTest_test.AutoLinearityMeasurement[index].AutoLinearityMeasurementID == -2){
			returnvalue = FALSE;
			break;
		}
	}

	return returnvalue;
}

/*void wrap_and_append(char *message, char *field){
	strcat(message, "\"");
	strcat(message, field);
	strcat(message, "\"");
	strcat(message, ",");
}

void end_line(char *message){
	message[strlen(message) - 1] = 0;
	strcat(message, "\n\r");
}*/
