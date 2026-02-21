#define PHASE_WELLRBCSURVIVALTEST_PRE_INIT	0
#define PHASE_WELLRBCSURVIVALTEST_WAIT		1
#define PHASE_WELLRBCSURVIVALTEST_RUN			2
#define PHASE_WELLRBCSURVIVALTEST_PRINT		3
#define PHASE_WELLRBCSURVIVALTEST_LINE1		17
#define PHASE_WELLRBCSURVIVALTEST_LINE2		18
#define PHASE_WELLRBCSURVIVALTEST_LINE3		19
#define PHASE_WELLRBCSURVIVALTEST_LINE4		20
#define PHASE_WELLRBCSURVIVALTEST_LINE5		21
#define PHASE_WELLRBCSURVIVALTEST_LINE6		22
#define PHASE_WELLRBCSURVIVALTEST_LINE7		23
#define PHASE_WELLRBCSURVIVALTEST_LINE8		24
#define PHASE_WELLRBCSURVIVALTEST_LINE9		25
#define PHASE_WELLRBCSURVIVALTEST_LINE10	26
#define PHASE_WELLRBCSURVIVALTEST_ACTIVATE	27

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "sqlite3.h"
#include "database.h"
#include "printer.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern time_t clock_time;
extern CURRENT current;
extern WELLRBCSURVIVALMEAS AmuletWellRBCSurvivalMeasureMenu_measurement;
extern float RBCSurvival_minNormal, RBCSurvival_maxNormal;
extern uchar AmuletGenericItems_config;
extern uchar AmuletInactivate_config;

WELLRBCSURVIVALTEST AmuletWellRBCSurvivalTest;

static long long int drill[10];
static WELLRBCSURVIVALRESULT rbcresult;
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
void AmuletWellRBCSurvivalTest_print(void);
int CalcAge(double julian_date);
int roundfloat(float value);

void AmuletWellRBCSurvivalTest_menu(void){
	char message[76], message2[76], timestring[40], boldstring[2], normalstring[2];
	float cpm_cps;
	int index, curline, days, NextMeasurementNumber, Age;

	switch(m_iPhase){
		case PHASE_WELLRBCSURVIVALTEST_PRE_INIT:
			if(m_ucClear == 101){
				m_ucClear = 0;
			}

			DB_RetrieveWellRBCSurvivalTest(&AmuletWellRBCSurvivalTest);
			if(AmuletWellRBCSurvivalTest.WellRBCSurvivalTestID <= 0){
				Amulet_DisplayError("RBC Survival", "Unable to retrieve Test", TRUE);
				return;
			}

			SetAmuletString(100, AmuletWellRBCSurvivalTest.PatientID);

			message[0] = 0;
			strcat(message, AmuletWellRBCSurvivalTest.LastName);
			strcat(message, ", ");
			strcat(message, AmuletWellRBCSurvivalTest.FirstName);
			delayloop(2);
			SetAmuletString(101, message);

			dateout_julian(message, AmuletWellRBCSurvivalTest.DateOfBirth);
			delayloop(2);
			SetAmuletString(103, message);

			Age = CalcAge(AmuletWellRBCSurvivalTest.DateOfBirth);
			sprintf(message, "%d", Age);
			delayloop(2);
			SetAmuletString(104, message);

			delayloop(2);
			SetAmuletString(105, AmuletWellRBCSurvivalTest.Sex);
			delayloop(2);
			SetAmuletString(106, AmuletWellRBCSurvivalTest.TechID);

			sprintf(message, "%d", AmuletWellRBCSurvivalTest.CountingTime);
			delayloop(2);
			SetAmuletString(107, message);
			delayloop(2);
			SetAmuletString(108, AmuletWellRBCSurvivalTest.LotNum);
			delayloop(2);
			SetAmuletString(109, AmuletWellRBCSurvivalTest.Physician);

			if(current.system == CI) format_activity_system2(AmuletWellRBCSurvivalTest.CalibrationActivity, message);
			else format_activity_system_kbq(AmuletWellRBCSurvivalTest.CalibrationActivity, message);
			delayloop(2);
			SetAmuletString(110, message);

			GetExtendedTimeInfo(&AmuletWellRBCSurvivalTest.CalibrationDate, message);
			delayloop(2);
			SetAmuletString(111, message);

			GetExtendedTimeInfo(&AmuletWellRBCSurvivalTest.InjectionDate, message);
			delayloop(2);
			SetAmuletString(112, message);

			sprintf(message, "%.1f - %.1f", AmuletWellRBCSurvivalTest.StartROI, AmuletWellRBCSurvivalTest.EndROI);
			delayloop(2);
			SetAmuletString(113, message);

			if(!AmuletWellRBCSurvivalTest.Inactive){
				if(AmuletWellRBCSurvivalTest.WellRBCSurvivalTestPhase < 0){
					SetAmuletByte(94, 0x00);
					SetAmuletByte(99, 0x00);
				}else{
					SetAmuletByte(99, 0xFF); // Show Run Button
					if(AmuletWellRBCSurvivalTest.WellRBCSurvivalTestPhase == 0){
						SetAmuletByte(94, 0x00);
					}else{
						SetAmuletByte(94, 0xFF);
						SetAmuletString(97, "Complete");
					}
				}
				SetAmuletByte(98, 0x00);
			}else{
				SetAmuletByte(94, 0xFF);
				SetAmuletString(97, "ReActivate");
				SetAmuletByte(99, 0x00);
				if(strlen(AmuletWellRBCSurvivalTest.InactiveReason) > 0){
					SetAmuletString(98, AmuletWellRBCSurvivalTest.InactiveReason);
					SetAmuletByte(98, 0xFF);
				}else{
					SetAmuletByte(98, 0x00);
				}
			}

			for(index=0; index<10; index++) drill[index] = -1;

			boldstring[0] = 5;
			boldstring[1] = 0;
			normalstring[0] = 3;
			normalstring[1] = 0;

			curline = 0;

			rbcresult.WellRBCSurvivalResultID = -1;
			rbcresult.WellRBCSurvivalTestID = AmuletWellRBCSurvivalTest.WellRBCSurvivalTestID;
			DB_RetrieveWellRBCSurvivalResult(&rbcresult);
			if(rbcresult.WellRBCSurvivalResultID != -1){
				sprintf(message, " %s50%% Survival at %.1f Days%s", boldstring, rbcresult.HalfDays, normalstring);
				if((RBCSurvival_minNormal > 0) && (RBCSurvival_maxNormal > 0)){
					if((rbcresult.HalfDays < RBCSurvival_minNormal) || (rbcresult.HalfDays > RBCSurvival_maxNormal)){
						sprintf(message2, " %s= Outside of Normal Range%s", boldstring, normalstring);
						strcat(message, message2);
					}else{
						sprintf(message2, " %s= Inside of Normal Range%s", boldstring, normalstring);
						strcat(message, message2);
					}
				}
				SetAmuletString((3*curline) + 51, message);
				SetAmuletByte(curline + 81, 0xFF);
				curline++;
			}
			NextMeasurementNumber = DB_RBCSurvivalMeasurementCount(&AmuletWellRBCSurvivalTest) + 1;
			if(NextMeasurementNumber > 1){
				for(index=1; index<NextMeasurementNumber; index++){
					AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalTestID = AmuletWellRBCSurvivalTest.WellRBCSurvivalTestID;
					AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber = index;
					DB_RetrieveWellRBCSurvivalMeasurement(&AmuletWellRBCSurvivalMeasureMenu_measurement);
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID != -1){
						if((curline == 9) && (index < NextMeasurementNumber - 1)){
							sprintf(message, " %sPlease print test to view additional measurements%s", boldstring, normalstring);
							delayloop(10);
							SetAmuletString((3*curline) + 51, message);
							SetAmuletByte(curline + 81, 0xFF);
							curline++;
							break;
						}

						drill[curline] = AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID;
						GetExtendedTimeInfo(&AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn, timestring);
						days = roundfloat(AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays);
						cpm_cps = AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;

						sprintf(message, "%s %s %sAt %d Days]%s RBC Remaining: %s%.1f %%%s", normalstring, timestring, boldstring, days, normalstring, boldstring, AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining, normalstring);
						delayloop(10);
						SetAmuletString((3*curline) + 51, message);
						SetAmuletByte(curline + 81, 0xFF);
					}
					curline++;
				}
			}
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER)) SetAmuletByte(91, 0xFF);
			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLRBCSURVIVALTEST_WAIT;
			break;

		case PHASE_WELLRBCSURVIVALTEST_WAIT:
			break;

		case PHASE_WELLRBCSURVIVALTEST_RUN:
			beep_amulet();
			if(AmuletWellRBCSurvivalTest.WellRBCSurvivalTestPhase >= 0){
				AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID = -1;
				AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalTestID = AmuletWellRBCSurvivalTest.WellRBCSurvivalTestID;
				m_ucClear = 102;
				SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLRBCSURVIVALTEST_WAIT;
			break;

		case PHASE_WELLRBCSURVIVALTEST_PRINT:
			beep_amulet();
			AmuletWellRBCSurvivalTest_print();
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_WELLRBCSURVIVALTEST_WAIT;
			break;

		case PHASE_WELLRBCSURVIVALTEST_LINE1:
		case PHASE_WELLRBCSURVIVALTEST_LINE2:
		case PHASE_WELLRBCSURVIVALTEST_LINE3:
		case PHASE_WELLRBCSURVIVALTEST_LINE4:
		case PHASE_WELLRBCSURVIVALTEST_LINE5:
		case PHASE_WELLRBCSURVIVALTEST_LINE6:
		case PHASE_WELLRBCSURVIVALTEST_LINE7:
		case PHASE_WELLRBCSURVIVALTEST_LINE8:
		case PHASE_WELLRBCSURVIVALTEST_LINE9:
		case PHASE_WELLRBCSURVIVALTEST_LINE10:
			beep_amulet();
			if(drill[m_iPhase - PHASE_WELLRBCSURVIVALTEST_LINE1] > 0){
				AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID =  drill[m_iPhase - PHASE_WELLRBCSURVIVALTEST_LINE1];
				m_ucClear = 102;
				SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVALMEASUREMENT_HTM]);
				return;
			}

			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_WELLRBCSURVIVALTEST_WAIT;
			break;

		case PHASE_WELLRBCSURVIVALTEST_ACTIVATE:
			beep_amulet();
			if(!AmuletWellRBCSurvivalTest.Inactive){
				AmuletInactivate_config = 10;
				m_ucClear = 63;
				SetAmuletHTML(AmuletHTMLIndex[INACTIVATE_HTM]);
				PushPageStack(AmuletHTMLIndex[INACTIVATE_HTM]);
				return;
			}else{
				AmuletGenericItems_config = 17;
				SetAmuletHTML(AmuletHTMLIndex[GENERICITEMS_HTM]);
				PushPageStack(AmuletHTMLIndex[GENERICITEMS_HTM]);
				return;
			}
			break;
	}
}

void AmuletWellRBCSurvivalTest_print(void){
	float cpm_cps;
	int index, NextMeasurementNumber, days;
	char prtype, strng[200], message[51], timestring[80], daystring[30];

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
	    rawheader(prtype, "RBC Survival Report",  clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertstring(strng, 0, 0, 0, "        ID: %s", AmuletWellRBCSurvivalTest.PatientID);
		pr_write2(strng, "RBC Survival Report");

		insertstring(strng, 0, 0, 0, "     First: %s", AmuletWellRBCSurvivalTest.FirstName);
		pr_write2(strng, "RBC Survival Report");

		insertstring(strng, 0, 0, 0, "      Last: %s", AmuletWellRBCSurvivalTest.LastName);
		pr_write2(strng, "RBC Survival Report");

		dateout_julian(message, AmuletWellRBCSurvivalTest.DateOfBirth);
		insertstring(strng, 0, 0, 0, "       DOB: %s", message);
		pr_write2(strng, "RBC Survival Report");

		insertint(strng, 0, 0, 0, "       Age: %d", CalcAge(AmuletWellRBCSurvivalTest.DateOfBirth));
		pr_write2(strng, "RBC Survival Report");

		// Sex
		insertstring(strng, 0, 0, 0, "       Sex: %s", AmuletWellRBCSurvivalTest.Sex);
		pr_write2(strng, "RBC Survival Report");

		// Phys
		insertstring(strng, 0, 0, 0, " Physician: %s", AmuletWellRBCSurvivalTest.Physician);
		pr_write2(strng, "RBC Survival Report");

		// Tech
		insertstring(strng, 0, 0, 0, "      Tech: %s", AmuletWellRBCSurvivalTest.TechID);
		pr_write2(strng, "RBC Survival Report");

		if(strlen(AmuletWellRBCSurvivalTest.InactiveReason)>0){
			insertstring(strng, 0, 0, 0, "INACTIVE: %s", AmuletWellRBCSurvivalTest.InactiveReason);
			pr_write2(strng, "RBC Survival Report");
		}

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2(strng, "RBC Survival Report");

		// Nuc
		insertstring(strng, 0, 0, 0, "   Nuclide: %s", AmuletWellRBCSurvivalTest.NuclideName);
		pr_write2(strng, "RBC Survival Report");

		// ROI
		sprintf(message, "%.1f - %.1f", AmuletWellRBCSurvivalTest.StartROI, AmuletWellRBCSurvivalTest.EndROI);
		insertstring(strng, 0, 0, 0, "  ROI(keV): %s", message);
		pr_write2(strng, "RBC Survival Report");

		// Halflife
		sprintf(message, "  Halflife: %.2f", AmuletWellRBCSurvivalTest.HalfLife);
		switch(AmuletWellRBCSurvivalTest.HalfLifeUnit){
			case SEC:
				strcat(message, " sec");
				break;

			case MIN:
				strcat(message, " min");
				break;

			case HOUR:
				strcat(message, " hr");
				break;

			case DAY:
				strcat(message, " day");
				break;

			case YEAR:
				strcat(message, " yr");
				break;
		}
		insertstring(strng, 0, 0, 0, "%s", message);
		pr_write2(strng, "RBC Survival Report");

		// Lot Num
		insertstring(strng, 0, 0, 0, "   Lot Num: %s", AmuletWellRBCSurvivalTest.LotNum);
		pr_write2(strng, "RBC Survival Report");

		// CountTime
	   	insertint(strng, 0, 0, 0, "Count Time: %d sec", AmuletWellRBCSurvivalTest.CountingTime);
		pr_write2(strng, "RBC Survival Report");

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2(strng, "RBC Survival Report");

		// Calibrated Activity
		insertconst(strng, 0, 0, 0, "Calibrated");
		pr_write2(strng, "RBC Survival Report");
		if(current.system == CI) format_activity_system2(AmuletWellRBCSurvivalTest.CalibrationActivity, message);
		else format_activity_system_kbq(AmuletWellRBCSurvivalTest.CalibrationActivity, message);
		replace(message, '$', 'u');
		insertstring(strng, 0, 0, 0, "  Activity: %s", message);
		pr_write2(strng, "RBC Survival Report");

		// Calibrated Date
		insertconst(strng, 0, 0, 0, "Calibrated");
		pr_write2(strng, "RBC Survival Report");
		GetExtendedTimeInfo(&AmuletWellRBCSurvivalTest.CalibrationDate, message);
		insertstring(strng, 0, 0, 0, "      Date: %s", message);
		pr_write2(strng, "RBC Survival Report");

		// Injection Date
		insertconst(strng, 0, 0, 0, " Injection");
		pr_write2(strng, "RBC Survival Report");
		GetExtendedTimeInfo(&AmuletWellRBCSurvivalTest.InjectionDate, message);
		insertstring(strng, 0, 0, 0, "      Date: %s", message);
		pr_write2(strng, "RBC Survival Report");

		rbcresult.WellRBCSurvivalResultID = -1;
		rbcresult.WellRBCSurvivalTestID = AmuletWellRBCSurvivalTest.WellRBCSurvivalTestID;
		DB_RetrieveWellRBCSurvivalResult(&rbcresult);
		if(rbcresult.WellRBCSurvivalResultID != -1){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "RBC Survival Report");
			insertfloat(strng, 0, 0, 0, "50%% Survival at %.1f Days", rbcresult.HalfDays);
			pr_write2(strng, "RBC Survival Report");
			if((RBCSurvival_minNormal > 0) && (RBCSurvival_maxNormal > 0)){
				if((rbcresult.HalfDays < RBCSurvival_minNormal) || (rbcresult.HalfDays > RBCSurvival_maxNormal)){
					insertconst(strng, 0, 0, 0, "Outide of Normal Range");
				}else{
					insertconst(strng, 0, 0, 0, "Inside of Normal Range");
				}
				pr_write2(strng, "RBC Survival Report");

				sprintf(message, "Normal Range: %.1f - %.1f Days", RBCSurvival_minNormal, RBCSurvival_maxNormal);
				insertconst(strng, 0, 0, 0, message);
				pr_write2(strng, "RBC Survival Report");
			}
		}

		NextMeasurementNumber = DB_RBCSurvivalMeasurementCount(&AmuletWellRBCSurvivalTest) + 1;
		if(NextMeasurementNumber > 1){
			for(index=1; index<NextMeasurementNumber; index++){
				AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID = -1;
				AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalTestID = AmuletWellRBCSurvivalTest.WellRBCSurvivalTestID;
				AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber = index;
				DB_RetrieveWellRBCSurvivalMeasurement(&AmuletWellRBCSurvivalMeasureMenu_measurement);

				if(AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID != -1){
					insertconst(strng, 0, 0, 0, "==================================");
					pr_write2(strng, "RBC Survival Report");
					GetExtendedTimeInfo(&AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn, timestring);
					days = roundfloat(AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays);
					sprintf(daystring, " At %d Days", days);
					strcat(timestring, daystring);
					insertconst(strng, 0, 0, 0, timestring);
					pr_write2(strng, "RBC Survival Report");

					cpm_cps = AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "     Background(cpm): %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "     Background(cps): %.0f", cpm_cps);
					pr_write2(strng, "RBC Survival Report");

					cpm_cps = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Measurement 1(cpm): %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "  Measurement 1(cps): %.0f", cpm_cps);
					pr_write2(strng, "RBC Survival Report");

					if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_SpectraID != -1){
						cpm_cps = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Measurement 2(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "  Measurement 2(cps): %.0f", cpm_cps);
						pr_write2(strng, "RBC Survival Report");
					}

					cpm_cps = AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "        Average(cpm): %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "        Average(cps): %.0f", cpm_cps);
					pr_write2(strng, "RBC Survival Report");

					cpm_cps = AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "            Net(cpm): %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "            Net(cps): %.0f", cpm_cps);
					pr_write2(strng, "RBC Survival Report");

					cpm_cps = AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "Decay Corrected(cpm): %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "Decay Corrected(cps): %.0f", cpm_cps);
					pr_write2(strng, "RBC Survival Report");

					insertfloat(strng, 0, 0, 0, "       Hematocrit(%%): %.1f", AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit);
					pr_write2(strng, "RBC Survival Report");

					insertfloat(strng, 0, 0, 0, "        Remaining(%%): %.1f", AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining);
					pr_write2(strng, "RBC Survival Report");

					if(strlen(AmuletWellRBCSurvivalMeasureMenu_measurement.comment) > 0){
						insertstring(strng, 0, 0, 0, "Comment: %s", AmuletWellRBCSurvivalMeasureMenu_measurement.comment);
						pr_write2(strng, "RBC Survival Report");
					}
				}
			}
		}
		formfeed(prtype);
	}
}
