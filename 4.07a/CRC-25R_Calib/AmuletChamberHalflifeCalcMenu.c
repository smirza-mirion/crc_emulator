/**
 * \file
 * \details This file handles calls from the Amulet Half-life Calculator Test Screen
 */
#define PHASE_HALFLIFE_CALC_PRE_INIT	0
#define PHASE_HALFLIFE_CALC_WAIT		1
#define PHASE_HALFLIFE_CALC_START		2
#define PHASE_HALFLIFE_CALC_STOP		3
#define PHASE_HALFLIFE_CALC_PGDOWN		4
#define PHASE_HALFLIFE_CALC_PGUP		5
#define PHASE_HALFLIFE_CALC_PRINT		6
#define PHASE_HALFLIFE_CALC_CLEAR		7

#include <stdlib.h>
#include <string.h>
#include <math.h>
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
extern time_t low_clock_time;
extern CHAMBERVALS chamb_vals[];
extern CHAMBER  chamber[];
extern MEASUREMENT measurement[];

HALFLIFECALC HalflifeCalc_test;

int AmuletHalflifeCalc_intervalSeconds;
int AmuletHalflifeCalc_totalSeconds;
int AmuletHalflifeCalc_printSummary;

static int AmuletHalflifeCalc_measurementCount;
static int AmuletHalflifeCalc_currentPage;	// Zero based
static int AmuletHalflifeCalc_lastPage;	// Zero based
const float HalflifeCalc_noise[6][2][3] = {
										5.77132e-03,	// 0, 0, 0 R TwoStage High Gain (Low ADC = -1, High ADC = 45)
										3.75e-01, 		// 0, 0, 1 R TwoStage Mid Gain (Not Valid State: Make same as noise for Low Gain)
										3.75e-01,		// 0, 0, 2 R TwoStage Low Gain (Low ADC = 267, High ADC = 270)
										1.189e-01,		// 0, 1, 0 R ThreeStage High Gain (Low ADC = 1000, High ADC = 2189)
										4.8e-01,		// 0, 1, 1 R ThreeStage Mid Gain (Low ADC = 700, High ADC = 748)
										1.4999e+01,		// 0, 1, 2 R ThreeStage Low Gain (Low ADC = 700, High 715)
										2.00577e-03,	// 1, 0, 0 PET TwoStage High Gain (Low ADC = -1, High ADC = 15)
										3.75e-01,		// 1, 0, 1 PET TwoStage Mid Gain (Not Valid State: Make same as noise for Low Gain)
										3.75e-01,		// 1, 0, 2 PET TwoStage Low Gain (Low ADC = 253, High ADC = 256)
										1.42e-02,		// 1, 1, 0 PET ThreeStage High Gain (Low ADC = 1000, High ADC = 1142)
										1.6e-01,		// 1, 1, 1 PET ThreeStage Mid Gain (Low ADC = 700, High ADC = 716)
										1.6e+01,		// 1, 1, 2 PET ThreeStage Low Gain (Low ADC = 700, High ADC = 716)
										2.00577e-03,	// 2, 0, 0 BT TwoStage High Gain (Low ADC = -1, High ADC = 15)
										3.75e-01,		// 2, 0, 1 BT TwoStage Mid Gain (Not Valid State: Make same as noise for Low Gain)
										3.75e-01,		// 2, 0, 2 BT TwoStage Low Gain (Low ADC = 253, High ADC = 256)
										1.42e-02,		// 2, 1, 0 BT ThreeStage High Gain (Low ADC = 1000, High ADC = 1142)
										1.6e-01,		// 2, 1, 1 BT ThreeStage Mid Gain (Low ADC = 700, High ADC = 716)
										1.6e+01,		// 2, 1, 2 BT ThreeStage Low Gain (Low ADC = 700, High ADC = 716)
										2.00577e-03,	// 3, 0, 0 1.8 TwoStage High Gain (Low ADC = -1, High ADC = 15)
										3.75e-01,		// 3, 0, 1 1.8 TwoStage Mid Gain (Not Valid State: Make same as noise for Low Gain)
										3.75e-01,		// 3, 0, 2 1.8 TwoStage Low Gain (Low ADC = 253, High ADC = 256)
										1.42e-02,		// 3, 1, 0 1.8 ThreeStage High Gain (Low ADC = 1000, High ADC = 1142)
										1.6e-01,		// 3, 1, 1 1.8 ThreeStage Mid Gain (Low ADC = 700, High ADC = 716)
										1.6e+01,		// 3, 1, 2 1.8 ThreeStage Low Gain (Low ADC = 700, High ADC = 716)
										2.00577e-03,	// 4, 0, 0 C TwoStage High Gain (Low ADC = -1, High ADC = 15)
										3.75e-01,		// 4, 0, 1 C TwoStage Mid Gain (Not Valid State: Make same as noise for Low Gain)
										3.75e-01,		// 4, 0, 2 C TwoStage Low Gain (Low ADC = 253, High ADC = 256)
										1.42e-02,		// 4, 1, 0 C ThreeStage High Gain (Low ADC = 1000, High ADC = 1142)
										1.6e-01,		// 4, 1, 1 C ThreeStage Mid Gain (Low ADC = 700, High ADC = 716)
										1.6e+01,		// 4, 1, 2 C ThreeStage Low Gain (Low ADC = 700, High ADC = 716)
										2.00577e-03,	// 5, 0, 0 K TwoStage High Gain (Low ADC = -1, High ADC = 15)
										3.75e-01,		// 5, 0, 1 K TwoStage Mid Gain (Not Valid State: Make same as noise for Low Gain)
										3.75e-01,		// 5, 0, 2 K TwoStage Low Gain (Low ADC = 253, High ADC = 256)
										1.42e-02,		// 5, 1, 0 K ThreeStage High Gain (Low ADC = 1000, High ADC = 1142)
										1.6e-01,		// 5, 1, 1 K ThreeStage Mid Gain (Low ADC = 700, High ADC = 716)
										1.6e+01,		// 5, 1, 2 K ThreeStage Low Gain (Low ADC = 700, High ADC = 716)
									};

void AmuletChamberHalflifeCalc_clearTest(void);
static int AmuletChamberHalflifeCalc_calcSamples(int interval, int total);
static void AmuletChamberHalflifeCalc_showElapsedRemaining(void);
static void AmuletChamberHalflifeCalc_displayPage(HALFLIFECALC *tst);
static void AmuletChamberHalflifeCalc_calcHalflife(HALFLIFECALC *tst);
static void AmuletChamberHalflifeCalc_displayResult(void);
static void AmuletChamberHalflifeCalc_print(void);
static void AmuletChamberHalflifeCalc_outputHalflife(char *output);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void trim_and_shrink(char *acByte);
void clear_milli_sec_counter(void);
void prsetprinter_db(void);

/**
 * \details Handles the Amulet Screen HalflifeCalculator.htm. HalflifeCalculator.htm runs the half-life calculator test.
 * \param Amulet_Byte_ID Drescription
 * \param 80 Language		(STATE) English = 0, Spanish = 1
 * \param 81 Show Interval Button
 * \param 82 Show Interval Text
 * \param 83 Show Total Label and Button
 * \param 84 Show Elapsed, Remaining Text
 * \param 85 Show Start Button
 * \param 86 Show Page Up Button
 * \param 87 Show Page Down Button
 * \param 88 Show Stop Button
 * \param 89 Show Print Button
 * \param 90 Show Home Button
 * \param 91 Show Back Button
 * \param 92 Show Clear Button
 * \param 93 Show Lot Button
 * \param 100 Show Initial
 * \param 101 Refresh Elapsed/Remaining
 * \param 102 Refresh Grid, PageUp, PageDown, sfPage
 * \param 103 Show All
 * \param 104 Push Stop Button
 * \param 105 Print Summary State, 0=Off 1=On
 * \param Amulet_String_ID Description
 * \param 70_71 Title
 * \param 72 Interval:
 * \param 73 btnIntervalText, sfIntervalText
 * \param 74_75 Enter Interval in seconds:
 * \param 76 Total:
 * \param 77 btnTotalText
 * \param 78_79 Enter Total Time in minutes:
 * \param 80 sfPage
 * \param 81_82 sfElapsedRemaining
 * \param 83 Start
 * \param 84 Stop
 * \param 85 Print
 * \param 86 Clear
 * \param 87 Lot
 * \param 88 btnLotText
 * \param 89_90 Please enter Lot:
 * \param 91 Print Summary
 * \param 92 Nuclide:
 * \param 93_94 Please Select Nuclide
 * \param 95 btnNuclideText
 * \param 96 Elapsed(sec)
 * \param 97 Measured
 * \param 98 Predicted
 * \param 99 % Var
 * \param 100 sfC1R1
 * \param 101 sfC2R1
 * \param 102 sfC3R1
 * \param 103 sfC4R1
 * \param 104 sfC1R2
 * \param 105 sfC2R2
 * \param 106 sfC3R2
 * \param 107 sfC4R2
 * \param 108 sfC1R3
 * \param 109 sfC2R3
 * \param 110 sfC3R3
 * \param 111 sfC4R3
 * \param 112 sfC1R4
 * \param 113 sfC2R4
 * \param 114 sfC3R4
 * \param 115 sfC4R4
 * \param 116 sfC1R5
 * \param 117 sfC2R5
 * \param 118 sfC3R5
 * \param 119 sfC4R5
 * \param 120 sfC1R6
 * \param 121 sfC2R6
 * \param 122 sfC3R6
 * \param 123 sfC4R6
 * \param 124 sfC1R7
 * \param 125 sfC2R7
 * \param 126 sfC3R7
 * \param 127 sfC4R7
 * \param 128 sfC1R8
 * \param 129 sfC2R8
 * \param 130 sfC3R8
 * \param 131 sfC4R8
 * \param 132 sfC1R9
 * \param 133 sfC2R9
 * \param 134 sfC3R9
 * \param 135 sfC4R9
 * \param 136 sfC1R10
 * \param 137 sfC2R10
 * \param 138 sfC3R10
 * \param 139 sfC4R10
 * \returns None
 */
void AmuletChamberHalflifeCalc_menu(void){
	char message[110], formatstring[104], titlestring[100];//, field[50];
	char ch_type_string[5];
	int ch_num, index, total_samples, suggested_total, suggested_interval, validReadingIndex, totalStages;
	//UINT bytesWritten;

	switch(m_iPhase){
		case PHASE_HALFLIFE_CALC_PRE_INIT:
			if(m_ucClear == 113){
				AmuletHalflifeCalc_intervalSeconds = 0;
				AmuletHalflifeCalc_totalSeconds = 0;
				AmuletHalflifeCalc_printSummary = 1;
				HalflifeCalc_test.NuclideName[0] = 0;
				AmuletHalflifeCalc_measurementCount = 0;
				AmuletHalflifeCalc_currentPage = 0;
				AmuletHalflifeCalc_lastPage = 0;
				AmuletChamberHalflifeCalc_clearTest();
				m_ucClear = 0;
			}

			ch_num = current.main_chamber;

			SetAmuletByte(80, current.language);

			if(chamber_type(ch_num) == R_CHAMB) strcpy(ch_type_string, "R");
			else if(chamber_type(ch_num) == P_CHAMB) strcpy(ch_type_string, "PET");
			else if(chamber_type(ch_num) == C_CHAMB) strcpy(ch_type_string, "HR");
			else if(chamber_type(ch_num) == K_CHAMB) strcpy(ch_type_string, "1K");
			else strcpy(ch_type_string, "");
			get_amulet_message(L_HALFLIFE_CALC2, formatstring);    // "Half-life Calc - Ch:%d, %s"
			sprintf(message, formatstring, ch_num + 1, ch_type_string);
			send_to_amulet_string(70, message);

			send_amulet_message(L_INTERVAL2, 72);    // "Interval:"

			if(AmuletHalflifeCalc_intervalSeconds == 0){
				message[0] = 0;
			}else{
				get_amulet_message(L_SECS, formatstring);    // "%d sec"
				sprintf(message, formatstring, AmuletHalflifeCalc_intervalSeconds);
			}
			send_to_amulet_string(73, message);

			send_amulet_message(L_ENTER_INTERVAL_SEC, 74);    // "Enter Interval in seconds:"

			send_amulet_message(L_PR_TOTAL, 76);    // "Total:"

			if(AmuletHalflifeCalc_totalSeconds == 0){
				message[0] = 0;
			}else{
				get_amulet_message(L_MINS, formatstring);    // "%d min"
				sprintf(message, formatstring, AmuletHalflifeCalc_totalSeconds / 60);
			}
			send_to_amulet_string(77, message);

			send_amulet_message(L_ENTER_TOTAL_TIME_IN_MINUTES, 78);    // "Enter Total Time in minutes:"

			send_amulet_message(L_START, 83);    // "Start"
			send_amulet_message(L_STOP, 84);    // "Stop"
			send_amulet_message(L_PRINT, 85);    // "Print"
			send_amulet_message(L_CLEAR_BUTTON, 86);    // "Clear"
			send_amulet_message(L_LOT_COLON, 87);    // "Lot:"
			send_amulet_message(L_PLEASE_ENTER_LOT, 89);    // "Please Enter Lot:"
			send_amulet_message(L_PRINT_SUMMARY, 91);    // "Print\nSummary"
			send_amulet_message(L_NUCLIDE_COLON, 92);    // "Nuclide:"
			send_amulet_message(L_PLEASE_SELECT_NUCLIDE, 93);    // "Please Select Nuclide"
			send_to_amulet_string(95, HalflifeCalc_test.NuclideName);

			send_amulet_message(L_ELAPSED_SEC, 96);    // "Elapsed (sec)"
			send_amulet_message(L_MEASURED2, 97);    // "Measured"
			send_amulet_message(L_PREDICTED, 98);    // "Predicted"
			send_amulet_message(L_PERCENT_VAR, 99);    // "% Var"

			SetAmuletByte(105, AmuletHalflifeCalc_printSummary);

			if(HalflifeCalc_test.HalflifeCalcID == -1){
				send_to_amulet_string(80, "");
				send_to_amulet_string(81, "");
				SetAmuletByte(81, 0xFF);	// Show Interval Button
				SetAmuletByte(82, 0x00);	// Hide Interval Text
				SetAmuletByte(83, 0xFF);	// Show Total Label and Button
				SetAmuletByte(84, 0x00);	// Hide Elapsed, Remaining Text
				SetAmuletByte(85, 0xFF);	// Show Start Button
				SetAmuletByte(86, 0x00);	// Hide Page Up Button
				SetAmuletByte(87, 0x00);	// Hide Page Down Button
				SetAmuletByte(88, 0x00);	// Hide Stop Button
				SetAmuletByte(89, 0x00);	// Hide Print Button
				SetAmuletByte(90, 0xFF);	// Show Home Button
				SetAmuletByte(91, 0xFF); 	// Show Back Button
				SetAmuletByte(92, 0x00);	// Hide Clear Button
				SetAmuletByte(93, 0x00);	// Hide Lot Button
			}else{
				AmuletChamberHalflifeCalc_displayResult();
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_HALFLIFE_CALC_WAIT;
			break;

		case PHASE_HALFLIFE_CALC_WAIT:
			if(HalflifeCalc_test.HalflifeCalcID == -2){
				if(AmuletHalflifeCalc_measurementCount != HalflifeCalc_test.MeasurementCount){
					AmuletHalflifeCalc_measurementCount = HalflifeCalc_test.MeasurementCount;
					AmuletChamberHalflifeCalc_showElapsedRemaining();
					AmuletChamberHalflifeCalc_displayPage(&HalflifeCalc_test);
					if(AmuletHalflifeCalc_measurementCount == HalflifeCalc_test.MeasurementTotal){
						SetAmuletByte(104, 0xFF);
					}else if(HalflifeCalc_test.CheckInitialMeasurement){
						if(AmuletHalflifeCalc_measurementCount){
							index = 0;
							validReadingIndex = -1;
							do{
								if(HalflifeCalc_test.HalflifeCalcMeasurement[index].Status == 0) validReadingIndex = index;
								index++;
							}while((index < AmuletHalflifeCalc_measurementCount) && (validReadingIndex == -1));

							if(validReadingIndex != -1){
								if(chamber_one_gain_relay(HalflifeCalc_test.ChamberNumber)) totalStages = 0;
								else totalStages = 1;

								if(HalflifeCalc_test.HalflifeCalcMeasurement[validReadingIndex].Voltage < (7 * HalflifeCalc_noise[chamber_type(HalflifeCalc_test.ChamberNumber)][totalStages][HalflifeCalc_test.InitialGain])){
									HalflifeCalc_test.InitialMeasurementStatus = 1;
									SetAmuletByte(104, 0xFF);
								}else{
									HalflifeCalc_test.InitialMeasurementStatus = 0;
								}
								HalflifeCalc_test.CheckInitialMeasurement = FALSE;
							}
						}
					}
				}
			}
			break;

		case PHASE_HALFLIFE_CALC_START:
			if(HalflifeCalc_test.HalflifeCalcID == -1){
				beep_amulet();
				if(AmuletHalflifeCalc_intervalSeconds == 0){
					get_amulet_message(L_HALFLIFE_CALC, titlestring);    // "Half-life Calculator"
					get_amulet_message(L_MISSING_INTERVAL, formatstring);    // "Missing Interval"
					Amulet_DisplayError(titlestring, formatstring, TRUE);
					return;
				}else if(AmuletHalflifeCalc_totalSeconds == 0){
					get_amulet_message(L_HALFLIFE_CALC, titlestring);    // "Half-life Calculator"
					get_amulet_message(L_MISSING_TOTAL_TIME, formatstring);    // "Missing Total Time"
					Amulet_DisplayError(titlestring, formatstring, TRUE);
					return;
				}else{
					total_samples = AmuletChamberHalflifeCalc_calcSamples(AmuletHalflifeCalc_intervalSeconds, AmuletHalflifeCalc_totalSeconds);

					if(total_samples < 5){
						get_amulet_message(L_HALFLIFE_CALC, titlestring);    // "Half-life Calculator"
						get_amulet_message(L_NUM_OF_MEAS_HAS_TO_BE_GREATER_THAN_4, formatstring);    // "Number of measurements has to be greater than 4"
						Amulet_DisplayError(titlestring, formatstring, TRUE);
						return;
					}else if(total_samples > 1201){
						suggested_total = 20 * AmuletHalflifeCalc_intervalSeconds; // 1200 / 60, total_minutes
						suggested_interval = AmuletHalflifeCalc_totalSeconds / 1200;
						if(suggested_interval < 1) suggested_interval = 1;
						while(AmuletChamberHalflifeCalc_calcSamples(suggested_interval, AmuletHalflifeCalc_totalSeconds) > 1201 ){
							suggested_interval++;
						}
						get_amulet_message(L_HALFLIFE_CALC, titlestring);    // "Half-life Calculator"
						get_amulet_message(L_NUM_OF_MEAS_GREATER_THAN_1201, formatstring);    // "Count is greater than 1201\nTry Total: %d or Interval: %d"
						sprintf(message, formatstring, suggested_total, suggested_interval);
						Amulet_DisplayError(titlestring, message, TRUE);
						return;
					}else{
						ch_num = current.main_chamber;
						AmuletChamberHalflifeCalc_clearTest();

						HalflifeCalc_test.ChamberNumber = ch_num;

						for(index=0; index<6; index++) HalflifeCalc_test.SerialNumber[index] = chamb_vals[ch_num].sn[index];
						HalflifeCalc_test.SerialNumber[6] = 0;

						HalflifeCalc_test.ChamberType = chamb_vals[ch_num].chamb_type;

						/*strcpy(nucshrink, chamber[ch_num].nucdata.name);
						trim_and_shrink(nucshrink);
						if(nucshrink[0] != 0){
							strcpy(HalflifeCalc_test.NuclideName, nucshrink);
						}else{
							cal_from_response(nucshrink, measurement[ch_num].resp0);
							for (index = 0; index < 8; index++) {
								switch(HalflifeCalc_test.NuclideName[index]) {
									case DIVSIGN:
										HalflifeCalc_test.NuclideName[index] = 0x2F;
										break;

									default:
										HalflifeCalc_test.NuclideName[index] = nucshrink[index];
										break;
								}
							}
							HalflifeCalc_test.NuclideName[8] = 0x00;
						}*/

						HalflifeCalc_test.IntervalSeconds = AmuletHalflifeCalc_intervalSeconds;

						HalflifeCalc_test.TotalSeconds = AmuletHalflifeCalc_totalSeconds;

						HalflifeCalc_test.StartedOn = low_clock_time;

						HalflifeCalc_test.CreatedOn = HalflifeCalc_test.StartedOn;

						HalflifeCalc_test.MeasurementTotal = 0;
						for(index=0; index<1210; index++){
							if(index * AmuletHalflifeCalc_intervalSeconds <= AmuletHalflifeCalc_totalSeconds){
								HalflifeCalc_test.HalflifeCalcMeasurement[index].TargetElapsedSecond = index * AmuletHalflifeCalc_intervalSeconds;
								HalflifeCalc_test.HalflifeCalcMeasurement[index].HalflifeCalcMeasurementID = -2;
								HalflifeCalc_test.MeasurementTotal++;
								if(index * AmuletHalflifeCalc_intervalSeconds == AmuletHalflifeCalc_totalSeconds){
									break;
								}
							}else{
								HalflifeCalc_test.HalflifeCalcMeasurement[index].TargetElapsedSecond = AmuletHalflifeCalc_totalSeconds;
								HalflifeCalc_test.HalflifeCalcMeasurement[index].HalflifeCalcMeasurementID = -2;
								HalflifeCalc_test.MeasurementTotal++;
								break;
							}
						}

						AmuletHalflifeCalc_measurementCount = 0;
						AmuletHalflifeCalc_currentPage = 0;
						AmuletHalflifeCalc_lastPage = 0;

						AmuletChamberHalflifeCalc_showElapsedRemaining();

						SetAmuletByte(81, 0x00); // Hide Interval Button
						SetAmuletByte(82, 0xFF); // Show Interval Text
						SetAmuletByte(83, 0x00); // Hide Total Label and Button
						SetAmuletByte(84, 0xFF); // Show Elapsed, Remaining Text
						SetAmuletByte(85, 0x00); // Hide Start Button
						SetAmuletByte(88, 0xFF); // Show Stop Button
						SetAmuletByte(89, 0x00); // Hide Print Button
						SetAmuletByte(90, 0x00); // Hide Home Button
						SetAmuletByte(91, 0x00); // Hide Back Button
						SetAmuletByte(92, 0x00); // Hide Clear Button)
						SetAmuletByte(103, 0xFF); // Show All
						clear_milli_sec_counter();
						HalflifeCalc_test.HalflifeCalcID = -2;
					}
				}
			}
			m_iPhase = PHASE_HALFLIFE_CALC_WAIT;
			break;

		case PHASE_HALFLIFE_CALC_STOP:
			if(HalflifeCalc_test.HalflifeCalcID == -2){
				beep_amulet();
				AmuletChamberHalflifeCalc_displayResult();
			}
			m_iPhase = PHASE_HALFLIFE_CALC_WAIT;
			break;

		case PHASE_HALFLIFE_CALC_PGDOWN:
			beep_amulet();
			if(AmuletHalflifeCalc_currentPage < AmuletHalflifeCalc_lastPage){
				AmuletHalflifeCalc_currentPage++;
				AmuletChamberHalflifeCalc_displayPage(&HalflifeCalc_test);
			}
			m_iPhase = PHASE_HALFLIFE_CALC_WAIT;
			break;

		case PHASE_HALFLIFE_CALC_PGUP:
			beep_amulet();
			if(AmuletHalflifeCalc_currentPage){
				AmuletHalflifeCalc_currentPage--;
				AmuletChamberHalflifeCalc_displayPage(&HalflifeCalc_test);
			}
			m_iPhase = PHASE_HALFLIFE_CALC_WAIT;
			break;

		case PHASE_HALFLIFE_CALC_PRINT:
			beep_amulet();
			AmuletChamberHalflifeCalc_print();
			SetAmuletByte(103, 0xFF); // Show All
			SetAmuletByte(102, 0xFF); // Show Grid
			m_iPhase = PHASE_HALFLIFE_CALC_WAIT;
			break;

		case PHASE_HALFLIFE_CALC_CLEAR:
			if(HalflifeCalc_test.HalflifeCalcID == -3){
				beep_amulet();
				if(AmuletHalflifeCalc_intervalSeconds == 0){
					message[0] = 0;
				}else{
					get_amulet_message(L_SECS, formatstring);    // "%d sec"
					sprintf(message, formatstring, AmuletHalflifeCalc_intervalSeconds);
				}
				send_to_amulet_string(73, message);

				if(AmuletHalflifeCalc_totalSeconds == 0){
					message[0] = 0;
				}else{
					get_amulet_message(L_MINS, formatstring);    // "%d min"
					sprintf(message, formatstring, AmuletHalflifeCalc_totalSeconds / 60);
				}
				send_to_amulet_string(77, message);
				send_to_amulet_string(80, "");
				send_to_amulet_string(81, "");

				AmuletHalflifeCalc_measurementCount = 0;
				AmuletHalflifeCalc_currentPage = 0;
				AmuletHalflifeCalc_lastPage = 0;
				AmuletChamberHalflifeCalc_clearTest();

				SetAmuletByte(81, 0xFF);	// Show Interval Button
				SetAmuletByte(82, 0x00);	// Hide Interval Text
				SetAmuletByte(83, 0xFF);	// Show Total Label and Button
				SetAmuletByte(84, 0x00);	// Hide Elapsed, Remaining Text
				SetAmuletByte(85, 0xFF);	// Show Start Button
				SetAmuletByte(86, 0x00);	// Hide Page Up Button
				SetAmuletByte(87, 0x00);	// Hide Page Down Button
				SetAmuletByte(88, 0x00);	// Hide Stop Button
				SetAmuletByte(89, 0x00);	// Hide Print Button
				SetAmuletByte(90, 0xFF);	// Show Home Button
				SetAmuletByte(91, 0xFF); 	// Show Back Button
				SetAmuletByte(92, 0x00);	// Hide Clear Button
				SetAmuletByte(93, 0x00);	// Hide Lot Button
				SetAmuletByte(103, 0xFF);	// Show All


				AmuletChamberHalflifeCalc_displayPage(&HalflifeCalc_test);
			}

			m_iPhase = PHASE_HALFLIFE_CALC_WAIT;
			break;
	}
}

static int AmuletChamberHalflifeCalc_calcSamples(int interval, int total){
	int return_value;

	return_value = (total/interval) + 1;
	if((total % interval) != 0) return_value++;

	return return_value;
}

static void AmuletChamberHalflifeCalc_displayPage(HALFLIFECALC *tst){
	int index, measurementIndex, measurementCount;
	char message[51], formatstring[100];

	measurementCount = tst->MeasurementCount;
	AmuletHalflifeCalc_lastPage = measurementCount / 10;
	if(measurementCount % 10) AmuletHalflifeCalc_lastPage++;
	AmuletHalflifeCalc_lastPage--;
	if(AmuletHalflifeCalc_lastPage < 0) AmuletHalflifeCalc_lastPage = 0;

	if(AmuletHalflifeCalc_lastPage == 0){
		SetAmuletByte(86, 0x00); // Hide Up Button
		SetAmuletByte(87, 0x00); // Hide Down Button
	}else{
		if(AmuletHalflifeCalc_currentPage == 0){
			SetAmuletByte(86, 0x00); // Hide Up Button
			SetAmuletByte(87, 0xFF); // Show Down Button
		}else if(AmuletHalflifeCalc_currentPage == AmuletHalflifeCalc_lastPage){
			SetAmuletByte(86, 0xFF); // Show Up Button
			SetAmuletByte(87, 0x00); // Hide Down Button
		}else{
			SetAmuletByte(86, 0xFF); // Show Up Button
			SetAmuletByte(87, 0xFF); // Show Down Button
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		measurementIndex = 10 * AmuletHalflifeCalc_currentPage + index;
		if(measurementIndex < measurementCount){
			if(tst->HalflifeCalcMeasurement[measurementIndex].HalflifeCalcMeasurementID == -3){
				if(tst->HalflifeCalcMeasurement[measurementIndex].ElapsedMilliSecond != 0){
					sprintf(message, "  %d.%03d", tst->HalflifeCalcMeasurement[measurementIndex].ElapsedSecond, tst->HalflifeCalcMeasurement[measurementIndex].ElapsedMilliSecond);
				}else{
					sprintf(message, "  %d", tst->HalflifeCalcMeasurement[measurementIndex].ElapsedSecond);
				}
				send_to_amulet_string(4*index + 100, message);

				//format_activity_system(tst->HalflifeCalcMeasurement[measurementIndex].MeasuredActivity, message);
				if(tst->HalflifeCalcMeasurement[measurementIndex].Status == 0){
					sprintf(message, "%e", tst->HalflifeCalcMeasurement[measurementIndex].Voltage);
					send_to_amulet_string(4*index + 101, message);

					if(tst->HalflifeCalcMeasurement[measurementIndex].PredictedVoltage < -999999){
						send_to_amulet_string(4*index + 102, "");
					}else{
						//format_activity_system(tst->HalflifeCalcMeasurement[measurementIndex].PredictedVoltage, message);
						sprintf(message, "%e", tst->HalflifeCalcMeasurement[measurementIndex].PredictedVoltage);
						send_to_amulet_string(4*index + 102, message);
					}

					if(tst->HalflifeCalcMeasurement[measurementIndex].PredictedVariation < -999999){
						send_to_amulet_string(4*index + 103, "");
					}else{
						sprintf(message, "%.1f", tst->HalflifeCalcMeasurement[measurementIndex].PredictedVariation);
						send_to_amulet_string(4*index + 103, message);
					}
				}else if(tst->HalflifeCalcMeasurement[measurementIndex].Status == 1){
					send_amulet_message(L_OVERRANGE2, 4*index + 101);    // "Over Range"
					send_to_amulet_string(4*index + 102, "");
					send_to_amulet_string(4*index + 103, "");
				}else if(tst->HalflifeCalcMeasurement[measurementIndex].Status == 2){
					send_amulet_message(L_UNDERRANGE, 4*index + 101);    // "Under Range"
					send_to_amulet_string(4*index + 102, "");
					send_to_amulet_string(4*index + 103, "");
				}else{
					send_to_amulet_string(4*index + 101, "");
					send_to_amulet_string(4*index + 102, "");
					send_to_amulet_string(4*index + 103, "");
				}
			}else if(tst->HalflifeCalcMeasurement[measurementIndex].HalflifeCalcMeasurementID == -2){
				sprintf(message, "  %d", tst->HalflifeCalcMeasurement[measurementIndex].TargetElapsedSecond);
				send_to_amulet_string(4*index + 100, message);
				send_amulet_message(L_SKIPPED, 4*index + 101);    // "Skipped"
				send_to_amulet_string(4*index + 102, "");
				send_to_amulet_string(4*index + 103, "");
			}else{
				send_to_amulet_string(4*index + 100, "");
				send_to_amulet_string(4*index + 101, "");
				send_to_amulet_string(4*index + 102, "");
				send_to_amulet_string(4*index + 103, "");
			}
		}else{
			send_to_amulet_string(4*index + 100, "");
			send_to_amulet_string(4*index + 101, "");
			send_to_amulet_string(4*index + 102, "");
			send_to_amulet_string(4*index + 103, "");
		}
	}

	message[0] = 0;
	if(AmuletHalflifeCalc_lastPage > 0){
		get_amulet_message(L_OF, formatstring);    // "%d of %d"
		sprintf(message, formatstring, AmuletHalflifeCalc_currentPage + 1, AmuletHalflifeCalc_lastPage + 1);
	}
	send_to_amulet_string(80, message);

	SetAmuletByte(102, 0xFF);
}

static void AmuletChamberHalflifeCalc_print(void){
	char prtype;
	char strng[90], tempSECTION[40], message[90], date[20], time[20], elapseHEADER[25], measuredHEADER[25], predictedHEADER[25], varHEADER[25];
	int i, measurementCount;

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		prhead_language_db(prtype);
		feed(1, prtype);

		pr_set_linecnt(5);

		prsetprinter_db();

		lininit_db(strng, TRUE, prtype);

		if(prtype == USB_PRINTER) pcl_cpi("12", TRUE);
		if(prtype == USB_EPS_PRINTER) eps_cpi(12);

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
		linupdate_db(strng, FALSE, TRUE);

		get_amulet_message(L_HALFLIFE_CALC, tempSECTION);    // "Half-life Calculator"
		insertconst(strng, 0, 0, 0, tempSECTION);
		pr_write2_language_dbsec(strng, "");

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
		linupdate_db(strng, FALSE, FALSE);

		get_amulet_message(L_CHAMBER_SN_2, tempSECTION);    // "Chamber S/N:"
		insertconst(strng, 11, 1, 0, tempSECTION);

		strcpy(message, HalflifeCalc_test.SerialNumber);

		get_amulet_message(L_TYPE, tempSECTION);    // "Type:"
		if(HalflifeCalc_test.ChamberType == R_CHAMB){
			strcat(message, ", ");
			strcat(message, tempSECTION);
			strcat(message, " R");
		}else if(HalflifeCalc_test.ChamberType == P_CHAMB){
			strcat(message, ", ");
			strcat(message, tempSECTION);
			strcat(message, " PET");
		}
		insertconst(strng, 13, 0, 0, message);
		pr_write2_language_dbsec(strng, "");

		get_amulet_message(L_NUCLIDE_COLON, tempSECTION);    // "Nuclide:"
		insertconst(strng, 11, 1, 0, tempSECTION);
		insertconst(strng, 13, 0, 0, HalflifeCalc_test.NuclideName);
		pr_write2_language_dbsec(strng, "");

		get_amulet_message(L_PR_LOT, tempSECTION);    // "Lot #:"
		insertconst(strng, 11, 1, 0, tempSECTION);
		insertconst(strng, 13, 0, 0, HalflifeCalc_test.LotNum);
		pr_write2_language_dbsec(strng, "");

		get_amulet_message(L_STARTED, tempSECTION);    // "Started:"
		insertconst(strng, 11, 1, 0, tempSECTION);
		dateout_language(date, &HalflifeCalc_test.StartedOn, 4);
		timeoutsec(time, &HalflifeCalc_test.StartedOn);
		sprintf(message, "%s %s", date, time);
		insertconst(strng, 13, 0, 0, message);
		pr_write2_language_dbsec(strng, "");

		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
		linupdate_db(strng, FALSE, TRUE);
		AmuletChamberHalflifeCalc_outputHalflife(message);
		if(current.language == ENGLISH){
			insertconst(strng, 2, 0, 0, message);
		}else{
			insertconst(strng, 1, 0, 0, message);
		}
		pr_write2_language_dbsec(strng, "");
		if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
		linupdate_db(strng, FALSE, FALSE);

		get_amulet_message(L_ELAPSED_SEC, elapseHEADER);    // "Elapsed (sec)"
		get_amulet_message(L_MEASURED2, measuredHEADER);    // "Measured"
		get_amulet_message(L_PREDICTED, predictedHEADER);    // "Predicted"
		get_amulet_message(L_PERCENT_VAR, varHEADER);    // "% Var"

		measurementCount = HalflifeCalc_test.MeasurementCount;

		if(prtype == OKI_PRINTER || prtype < 0 || prtype == LX_PRINTER){
			if(!AmuletHalflifeCalc_printSummary){
				pr_write2_language_dbsec(strng, "");

				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(TRUE);
				linupdate_db(strng, FALSE, TRUE);

				insertconst(strng, 0, 0, 0, elapseHEADER);
				insertconst(strng, 18, 0, 0, measuredHEADER);
				insertconst(strng, 36, 0, 0, predictedHEADER);
				insertconst(strng, 54, 0, 0, varHEADER);
				pr_write2_language_dbsec(strng, "");

				if(prtype == OKI_PRINTER || prtype == USB_PRINTER || prtype == LX_PRINTER) bold(FALSE);
				linupdate_db(strng, FALSE, FALSE);

				for(i=0; i<measurementCount; i++){
					if(HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID == -3){
						if(HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedMilliSecond != 0){
							sprintf(message, "%d.%03d", HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedSecond, HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedMilliSecond);
						}else{
							sprintf(message, "%d", HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedSecond);
						}
						insertconst(strng, 2, 0, 0, message);

						switch(HalflifeCalc_test.HalflifeCalcMeasurement[i].Status){
							case 0:
								sprintf(message, "%e", HalflifeCalc_test.HalflifeCalcMeasurement[i].Voltage);
								insertconst(strng, 18, 0, 0, message);

								if(HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVoltage >= -999999){
									sprintf(message, "%e", HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVoltage);
									insertconst(strng, 36, 0, 0, message);
								}

								if(HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVariation >= -999999){
									sprintf(message, "%.1f", HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVariation);
									insertconst(strng, 54, 0, 0, message);
								}
								break;

							case 1:
								get_amulet_message(L_OVERRANGE2, message);    // "Over Range"
								insertconst(strng, 18, 0, 0, message);
								break;

							case 2:
								get_amulet_message(L_UNDERRANGE, message);    // "Under Range"
								insertconst(strng, 18, 0, 0, message);
								break;
						}
						pr_write2_language_dbsec(strng, "");
					}else if(HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID == -2){
						sprintf(message, "%d", HalflifeCalc_test.HalflifeCalcMeasurement[i].TargetElapsedSecond);
						insertconst(strng, 2, 0, 0, message);

						get_amulet_message(L_SKIPPED, message);    // "Skipped"
						insertconst(strng, 18, 0, 0, message);

						pr_write2_language_dbsec(strng, "");
					}
				}
			}

			formfeed(prtype);
		}else{
			pr_write2_language_dbsec(strng, "");

			if(!AmuletHalflifeCalc_printSummary){
				// Roll printer
				strcat(elapseHEADER, ":");
				strcat(measuredHEADER, ":");
				strcat(predictedHEADER, ":");
				strcat(varHEADER, ":");

				for(i=0; i<measurementCount; i++){
					if(HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID == -3){
						insertconst(strng, 14, 1, 0, elapseHEADER);
						if(HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedMilliSecond != 0){
							sprintf(message, "%d.%03d", HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedSecond, HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedMilliSecond);
						}else{
							sprintf(message, "%d", HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedSecond);
						}
						insertconst(strng, 16, 0, 0, message);
						pr_write2_language_dbsec(strng, "");

						switch(HalflifeCalc_test.HalflifeCalcMeasurement[i].Status){
							case 0:
								insertconst(strng, 14, 1, 0, measuredHEADER);
								sprintf(message, "%e", HalflifeCalc_test.HalflifeCalcMeasurement[i].Voltage);
								insertconst(strng, 16, 0, 0, message);
								pr_write2_language_dbsec(strng, "");

								if(HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVoltage >= -999999){
									insertconst(strng, 14, 1, 0, predictedHEADER);
									sprintf(message, "%e", HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVoltage);
									insertconst(strng, 16, 0, 0, message);
									pr_write2_language_dbsec(strng, "");
								}

								if(HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVariation >= -999999){
									insertconst(strng, 14, 1, 0, varHEADER);
									sprintf(message, "%.1f", HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVariation);
									insertconst(strng, 16, 0, 0, message);
									pr_write2_language_dbsec(strng, "");
								}
								break;

							case 1:
								get_amulet_message(L_OVERRANGE2, message);    // "Over Range"
								insertconst(strng, 0, 0, 0, message);
								pr_write2_language_dbsec(strng, "");
								break;

							case 2:
								get_amulet_message(L_UNDERRANGE, message);    // "Under Range"
								insertconst(strng, 0, 0, 0, message);
								pr_write2_language_dbsec(strng, "");
								break;
						}
						pr_write2_language_dbsec(strng, "");
						if(prtype == ROLL_PRINTER) delayloop(60);
					}else if(HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID == -2){
						insertconst(strng, 14, 1, 0, elapseHEADER);
						sprintf(message, "%d", HalflifeCalc_test.HalflifeCalcMeasurement[i].TargetElapsedSecond);
						insertconst(strng, 16, 0, 0, message);
						pr_write2_language_dbsec(strng, "");

						get_amulet_message(L_SKIPPED, message);    // "Skipped"
						insertconst(strng, 0, 0, 0, message);
						pr_write2_language_dbsec(strng, "");

						pr_write2_language_dbsec(strng, "");
						if(prtype == ROLL_PRINTER) delayloop(60);
					}
				}
			}
			pr_write2_language_dbsec(strng, "");
			pr_write2_language_dbsec(strng, "");
			pr_write2_language_dbsec(strng, "");
			pr_write2_language_dbsec(strng, "");
		}
	}
}

static void AmuletChamberHalflifeCalc_showElapsedRemaining(void){
	char message[52], formatstring[100];
	int elapsedSec, remainingSec, measurementCount;

	measurementCount = HalflifeCalc_test.MeasurementCount;
	if(measurementCount == HalflifeCalc_test.MeasurementTotal){
		get_amulet_message(L_FINISHED2, message);    // "Finished!!!"
	}else{
		if(measurementCount == 0){
			get_amulet_message(L_PLEASE_WAIT, message);    // "Please Wait"
		}else{
			elapsedSec = (measurementCount - 1) * HalflifeCalc_test.IntervalSeconds;
			remainingSec = HalflifeCalc_test.TotalSeconds - elapsedSec;

			get_amulet_message(L_ELAPSED_REMAINING, formatstring);    // "Elapsed: %d:%02d\nRemaining: %d:%02d"

			sprintf(message, formatstring, elapsedSec/60, elapsedSec%60, remainingSec/60, remainingSec%60);
		}
	}

	send_to_amulet_string(81, message);
	SetAmuletByte(101, 0xFF);
}

void AmuletChamberHalflifeCalc_clearTest(void){
	int i;

	HalflifeCalc_test.HalflifeCalcID = -1;
	HalflifeCalc_test.ChamberNumber = -1;
	HalflifeCalc_test.SerialNumber[0] = 0;
	HalflifeCalc_test.ChamberType = -1;
	HalflifeCalc_test.LotNum[0] = 0;
	HalflifeCalc_test.IntervalSeconds = -1;
	HalflifeCalc_test.TotalSeconds = -1;
	HalflifeCalc_test.StartedOn = -1;
	HalflifeCalc_test.Comment[0] = 0;
	HalflifeCalc_test.CreatedOn = -1;
	HalflifeCalc_test.InactiveReason[0] = 0;
	HalflifeCalc_test.Inactive = FALSE;
	HalflifeCalc_test.MeasurementCount = 0;
	HalflifeCalc_test.MeasurementTotal = 0;
	for(i=0; i<1210; i++){
		HalflifeCalc_test.HalflifeCalcMeasurement[i].HalflifeCalcMeasurementID = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].TargetElapsedSecond = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].Voltage = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].CalcLnVoltage = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].CalcActivity = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].Status = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].MilliSecStamp = 0xFFFFFFFF;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedSecond = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].ElapsedMilliSecond = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVoltage = -1000000;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].PredictedVariation = -1000000;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].CalcDeltaLnVoltage = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].CalcDeltaSec = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].CalcProductOfDeltaLnVoltageAndDeltaSec = -1;
		HalflifeCalc_test.HalflifeCalcMeasurement[i].CalcDeltaSecSquared = -1;
	}
	HalflifeCalc_test.InitialGain = 2;
	HalflifeCalc_test.CheckInitialMeasurement = TRUE;
	HalflifeCalc_test.InitialMeasurementStatus = 0;
	HalflifeCalc_test.CalcValidMeasurementCount = -1;
	HalflifeCalc_test.CalcLnVoltageAvg = -1;
	HalflifeCalc_test.CalcSecAvg = -1;
	HalflifeCalc_test.CalcSumOfProductOfDeltaLnVoltageAndDeltaSec = -1;
	HalflifeCalc_test.CalcSumOfDeltaSecSquared = -1;
	HalflifeCalc_test.CalcHalflifeStatus = -1;
	HalflifeCalc_test.CalcHalflife  = -1;
	HalflifeCalc_test.CalcHalflifeUnit = -1;
}

static void AmuletChamberHalflifeCalc_calcHalflife(HALFLIFECALC *tst){
	int i, measurement_count, totalStages;
	float ln_voltage_avg, sec_avg, sec_float, msec_float, measurement_count_float, sum_product, sum_squared, slope, halflife_sec, offset;
	float firstVoltage, lastVoltage;

	if(tst->HalflifeCalcID == -2){
		tst->HalflifeCalcID = -3;
		tst->CalcHalflifeStatus = 0;

		measurement_count = 0;
		ln_voltage_avg = 0.0;
		sec_avg = 0.0;
		firstVoltage = -1000000.0;
		lastVoltage = -1000000.0;
		for(i=0; i<1210; i++){
			if(tst->HalflifeCalcMeasurement[i].Status == 0){
				ln_voltage_avg += tst->HalflifeCalcMeasurement[i].CalcLnVoltage;

				if(firstVoltage < -999999.0) firstVoltage = tst->HalflifeCalcMeasurement[i].Voltage;
				lastVoltage = tst->HalflifeCalcMeasurement[i].Voltage;

				sec_float = tst->HalflifeCalcMeasurement[i].ElapsedSecond;
				if(tst->HalflifeCalcMeasurement[i].ElapsedMilliSecond != 0){
					msec_float = tst->HalflifeCalcMeasurement[i].ElapsedMilliSecond;
					msec_float /= 1000.0;
					sec_float += msec_float;
				}
				sec_avg += sec_float;

				measurement_count++;
			}
		}

		tst->CalcValidMeasurementCount = measurement_count;

		if(measurement_count > 4){
			measurement_count_float = measurement_count;

			ln_voltage_avg /= measurement_count_float;
			tst->CalcLnVoltageAvg = ln_voltage_avg;

			sec_avg /= measurement_count_float;
			tst->CalcSecAvg = sec_avg;

			sum_product = 0;
			sum_squared = 0;
			for(i=0; i<1210; i++){
				if(tst->HalflifeCalcMeasurement[i].Status == 0){
					tst->HalflifeCalcMeasurement[i].CalcDeltaLnVoltage = tst->HalflifeCalcMeasurement[i].CalcLnVoltage - ln_voltage_avg;

					sec_float = tst->HalflifeCalcMeasurement[i].ElapsedSecond;
					if(tst->HalflifeCalcMeasurement[i].ElapsedMilliSecond != 0){
						msec_float = tst->HalflifeCalcMeasurement[i].ElapsedMilliSecond;
						msec_float /= 1000.0;
						sec_float += msec_float;
					}
					tst->HalflifeCalcMeasurement[i].CalcDeltaSec = sec_float - sec_avg;

					tst->HalflifeCalcMeasurement[i].CalcProductOfDeltaLnVoltageAndDeltaSec = tst->HalflifeCalcMeasurement[i].CalcDeltaLnVoltage * tst->HalflifeCalcMeasurement[i].CalcDeltaSec;
					tst->HalflifeCalcMeasurement[i].CalcDeltaSecSquared = tst->HalflifeCalcMeasurement[i].CalcDeltaSec * tst->HalflifeCalcMeasurement[i].CalcDeltaSec;

					sum_product += tst->HalflifeCalcMeasurement[i].CalcProductOfDeltaLnVoltageAndDeltaSec;
					sum_squared += tst->HalflifeCalcMeasurement[i].CalcDeltaSecSquared;
				}
			}

			tst->CalcSumOfProductOfDeltaLnVoltageAndDeltaSec = sum_product;
			tst->CalcSumOfDeltaSecSquared = sum_squared;
			if(sum_squared != 0){
				slope = sum_product / sum_squared;
				if(slope < 0.0){
					if(chamber_one_gain_relay(tst->ChamberNumber)) totalStages = 0;
					else totalStages = 1;

					if((firstVoltage - lastVoltage) > (5 * HalflifeCalc_noise[chamber_type(tst->ChamberNumber)][totalStages][tst->InitialGain])){
						halflife_sec = -1 * log(2) / slope;
						//printf("Slope: %f, %f\n", slope, halflife_sec);
						if(halflife_sec < 120.0){
							tst->CalcHalflife = halflife_sec;
							tst->CalcHalflifeUnit = SEC;
						}else if(halflife_sec < 7200.0){
							tst->CalcHalflife = halflife_sec / 60.0;
							tst->CalcHalflifeUnit = MIN;
						}else if(halflife_sec < 172800.0){
							tst->CalcHalflife = halflife_sec / 3600.0;
							tst->CalcHalflifeUnit = HOUR;
						}else if(halflife_sec < 63115200.0){
							tst->CalcHalflife = halflife_sec / 86400.0;
							tst->CalcHalflifeUnit = DAY;
						}else{
							tst->CalcHalflife = halflife_sec / 31557600.0;
							tst->CalcHalflifeUnit = YEAR;
						}

						offset = ln_voltage_avg - (slope * sec_avg);

						for(i=0; i<1210; i++){
							if(tst->HalflifeCalcMeasurement[i].Status == 0){
								if(tst->HalflifeCalcMeasurement[i].Voltage > 0){
									sec_float = tst->HalflifeCalcMeasurement[i].ElapsedSecond;
									if(tst->HalflifeCalcMeasurement[i].ElapsedMilliSecond != 0){
										msec_float = tst->HalflifeCalcMeasurement[i].ElapsedMilliSecond;
										msec_float /= 1000.0;
										sec_float += msec_float;
									}

									tst->HalflifeCalcMeasurement[i].PredictedVoltage = exp((slope * sec_float) + offset);
									tst->HalflifeCalcMeasurement[i].PredictedVariation = (tst->HalflifeCalcMeasurement[i].Voltage - tst->HalflifeCalcMeasurement[i].PredictedVoltage) / tst->HalflifeCalcMeasurement[i].PredictedVoltage * 100.0;
								}
							}
						}
					}else{
						tst->CalcHalflifeStatus = 3; // Halflife Too Long
					}
				}else{
					if(slope == 0.0){
						tst->CalcHalflifeStatus = 3; // Halflife Too Long
					}else{
						tst->CalcHalflifeStatus = 4; // Halflife is Negative
					}
				}
			}else{
				tst->CalcHalflifeStatus = 2; // Halflife Too Short
			}
		}else{
			tst->CalcHalflifeStatus = 1; 	// Not Enough Measurements
		}
	}
}

static void AmuletChamberHalflifeCalc_displayResult(void){
	char message[110], message2[30];

	AmuletChamberHalflifeCalc_calcHalflife(&HalflifeCalc_test);

	get_amulet_message(L_FINISHED2, message);    // "Finished!!!"
	strcat(message, "\n");
	AmuletChamberHalflifeCalc_outputHalflife(message2);
	strcat(message, message2);
	send_to_amulet_string(81, message);
	SetAmuletByte(101, 0xFF);

	send_to_amulet_string(88, HalflifeCalc_test.LotNum);

	AmuletChamberHalflifeCalc_displayPage(&HalflifeCalc_test);

	SetAmuletByte(81, 0x00);	// Hide Interval Button
	SetAmuletByte(82, 0x00);	// Show Interval Text
	SetAmuletByte(83, 0x00);	// Hide Total Label and Button
	SetAmuletByte(84, 0xFF);	// Show Elapsed, Remaining Text
	SetAmuletByte(85, 0x00);	// Hide Start Button
	SetAmuletByte(88, 0x00);	// Hide Stop Button
	if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER)) SetAmuletByte(89, 0xFF);	// Show Print Button
	else SetAmuletByte(89, 0x00);	// Hide Print Button
	SetAmuletByte(90, 0x00);	// Hide Home Button
	SetAmuletByte(91, 0x00); 	// Hide Back Button
	SetAmuletByte(92, 0xFF);	// Show Clear Button
	SetAmuletByte(93, 0xFF);	// Show Lot Button
	SetAmuletByte(103, 0xFF);
}

static void AmuletChamberHalflifeCalc_outputHalflife(char *output){
	char message[25], message2[25];

	*output = 0;

	if(HalflifeCalc_test.InitialMeasurementStatus == 0){
		get_amulet_message(L_PR_HALFLIFE, message);    // "Half-Life"
		strcat(message, ":");

		switch(HalflifeCalc_test.CalcHalflifeStatus){
			case 0:
				message2[0] = 0;
				switch(HalflifeCalc_test.CalcHalflifeUnit){
					case SEC:
						get_amulet_message(L_SEC2, message2);    // " sec"
						break;

					case MIN:
						get_amulet_message(L_MIN2, message2);    // " min"
						break;

					case HOUR:
						get_amulet_message(L_HR2, message2);    // " hr"
						break;

					case DAY:
						get_amulet_message(L_DAY2, message2);    // " day"
						break;

					case YEAR:
						get_amulet_message(L_YR2, message2);    // " yr"
						break;
				}
				sprintf(output, "%s %.2f%s", message, HalflifeCalc_test.CalcHalflife, message2);
				break;

			case 1:
				get_amulet_message(L_LESS_THAN_5_MEASUREMENTS, output);    // "Less than 5 measurements"
				break;

			case 2:
				get_amulet_message(L_HALFLIFE_TOO_SHORT, output);    // "Half-life is too short"
				break;

			case 3:
				get_amulet_message(L_HALFLIFE_TOO_LONG, output);    // "Half-life is too long"
				break;

			case 4:
				get_amulet_message(L_HALFLIFE_NEGATIVE, output);    // "Half-life is negative"
				break;
		}
	}else{
		switch(HalflifeCalc_test.InitialMeasurementStatus){
			case 1:
				get_amulet_message(L_ACTIVITY_TOO_LOW_ERROR, output);    // "Activity Too Low Error"
				break;
		}
	}
}
