#define PHASE_ADDEDITRBCSURVIVALTEST_PRE_INIT			0
#define PHASE_ADDEDITRBCSURVIVALTEST_WAIT				1
#define PHASE_ADDEDITRBCSURVIVALTEST_SAVE				2


#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "wipes.h"
#include "database.h"
#include "amulet.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

WELLRBCSURVIVALTEST AmuletAddEditRBCSurvivalTest_test;

void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void trim(char *acByte);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);

void AmuletAddEditRBCSurvivalTest_menu(void){
	bool flgComplete;
	int NucIndex;
	char message[26];

	switch(m_iPhase){
		case PHASE_ADDEDITRBCSURVIVALTEST_PRE_INIT:
			if(m_ucClear == 100){

				m_ucClear = 0;
				if(AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestID == -1){
					AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestPhase = -1;
					AmuletAddEditRBCSurvivalTest_test.PatientID[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.FirstName[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.LastName[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.DateOfBirth = 0;
					AmuletAddEditRBCSurvivalTest_test.Sex[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.Physician[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.TechID[0] = 0;
					strcpy(AmuletAddEditRBCSurvivalTest_test.NuclideName, "Cr51");
					NucIndex = NuclideData_getIndexFromName(AmuletAddEditRBCSurvivalTest_test.NuclideName);
					AmuletAddEditRBCSurvivalTest_test.HalfLife = NuclideData_getHalflife(NucIndex);
					AmuletAddEditRBCSurvivalTest_test.HalfLifeUnit = NuclideData_getHalflifeUnit(NucIndex);
					AmuletAddEditRBCSurvivalTest_test.StartROI = 100;
					AmuletAddEditRBCSurvivalTest_test.EndROI = 500;
					AmuletAddEditRBCSurvivalTest_test.PeakEV = NuclideData_getEffectivePrimary(NucIndex);
					AmuletAddEditRBCSurvivalTest_test.CountingTime = 60;
					AmuletAddEditRBCSurvivalTest_test.InjectionDate = (time_t) 0;
					AmuletAddEditRBCSurvivalTest_test.CalibrationActivity = -1;
					AmuletAddEditRBCSurvivalTest_test.CalibrationDate = (time_t) 0;
					AmuletAddEditRBCSurvivalTest_test.LotNum[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.AccessionNumber[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.CreatedOn = (time_t) 0;
					AmuletAddEditRBCSurvivalTest_test.LastUpdated = (time_t) 0;
					AmuletAddEditRBCSurvivalTest_test.InactiveReason[0] = 0;
					AmuletAddEditRBCSurvivalTest_test.Inactive = FALSE;
				}else{
					DB_RetrieveWellRBCSurvivalTest(&AmuletAddEditRBCSurvivalTest_test);
					if(AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestID<=0){
						Amulet_DisplayError("RBC Survival", "Unable to retrieve RBC Survival Test", TRUE);
					}
				}
			}

			if(AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestID == -1){
				SetAmuletString(100, "Add RBC Survival Test");
			}else{
				SetAmuletString(100, "Edit RBC Survival Test");
			}

			trim(AmuletAddEditRBCSurvivalTest_test.FirstName);
			SetAmuletString(102, AmuletAddEditRBCSurvivalTest_test.FirstName); //FirstName
			trim(AmuletAddEditRBCSurvivalTest_test.LastName);
			SetAmuletString(103, AmuletAddEditRBCSurvivalTest_test.LastName); //LastName
			trim(AmuletAddEditRBCSurvivalTest_test.PatientID);
			SetAmuletString(104, AmuletAddEditRBCSurvivalTest_test.PatientID); //PatientID
			if(AmuletAddEditRBCSurvivalTest_test.DateOfBirth != 0.0){
				dateout_julian(message, AmuletAddEditRBCSurvivalTest_test.DateOfBirth);
				SetAmuletString(105, message); //DOB
			}else{
				SetAmuletString(105, ""); //DOB
			}

			SetAmuletString(106, AmuletAddEditRBCSurvivalTest_test.Sex); //Sex
			trim(AmuletAddEditRBCSurvivalTest_test.Physician);
			SetAmuletString(107, AmuletAddEditRBCSurvivalTest_test.Physician); //Physician
			trim(AmuletAddEditRBCSurvivalTest_test.TechID);
			SetAmuletString(108, AmuletAddEditRBCSurvivalTest_test.TechID); //TechID

			if(AmuletAddEditRBCSurvivalTest_test.CountingTime == -1){
				SetAmuletString(109, ""); //CountTime
			}else{
				sprintf(message, "%d sec", AmuletAddEditRBCSurvivalTest_test.CountingTime);
				SetAmuletString(109, message); //CountTime
			}

			if(AmuletAddEditRBCSurvivalTest_test.CalibrationActivity < 0){
				SetAmuletString(110, ""); //Activity
			}else{
				if(current.system == CI) format_activity_system2(AmuletAddEditRBCSurvivalTest_test.CalibrationActivity, message);
				else format_activity_system_kbq(AmuletAddEditRBCSurvivalTest_test.CalibrationActivity, message);
				SetAmuletString(110, message); //Activity
			}

			if(AmuletAddEditRBCSurvivalTest_test.CalibrationDate == (time_t) 0){
				SetAmuletString(111, ""); //Cal Date
			}else{
				GetExtendedTimeInfo(&AmuletAddEditRBCSurvivalTest_test.CalibrationDate, message);
				SetAmuletString(111, message); //Cal Date
			}

			trim(AmuletAddEditRBCSurvivalTest_test.LotNum);
			SetAmuletString(112, AmuletAddEditRBCSurvivalTest_test.LotNum); // Lot Num

			if(AmuletAddEditRBCSurvivalTest_test.InjectionDate == (time_t) 0){
				SetAmuletString(113, ""); //Injection Date
			}else{
				GetExtendedTimeInfo(&AmuletAddEditRBCSurvivalTest_test.InjectionDate, message);
				SetAmuletString(113, message); //Injection Date
			}

			if(AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestPhase < 1){
				if(AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestID != -1) SetAmuletByte(88, 0xFF);
				SetAmuletByte(90, 0xFF);
				SetAmuletByte(92, 0xFF);
				SetAmuletByte(94, 0xFF);
				SetAmuletByte(96, 0xFF);
				SetAmuletByte(98, 0xFF);
			}else{
				//if(AmuletAddEditRBCSurvivalTest_test.Inactive){
				//	SetAmuletByte(87, 0xFF);
				//}else{
				//	SetAmuletByte(89, 0xFF);
				//}
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(93, 0xFF);
				SetAmuletByte(95, 0xFF);
				SetAmuletByte(97, 0xFF);
				SetAmuletByte(99, 0xFF);

			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_ADDEDITRBCSURVIVALTEST_WAIT;
			break;

		case PHASE_ADDEDITRBCSURVIVALTEST_WAIT:
			break;

		case PHASE_ADDEDITRBCSURVIVALTEST_SAVE:
			beep_amulet();
			if(AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestPhase != -1){
				if(AmuletAddEditRBCSurvivalTest_test.FirstName[0] == 0){
					Amulet_DisplayError("RBC Survival Test", "Missing First Name", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.LastName[0] == 0){
					Amulet_DisplayError("RBC Survival Test", "Missing Last Name", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.PatientID[0] == 0){
					Amulet_DisplayError("RBC Survival Test", "Missing Patient ID", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.DateOfBirth == 0){
					Amulet_DisplayError("RBC Survival Test", "Missing Date of Birth", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.CountingTime == -1){
					Amulet_DisplayError("RBC Survival Test", "Missing Count Time", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.CalibrationActivity == -1){
					Amulet_DisplayError("RBC Survival Test", "Missing Calibration Activity", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.CalibrationDate == (time_t) 0){
					Amulet_DisplayError("RBC Survival Test", "Missing Calibration Date", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.LotNum[0] == 0){
					Amulet_DisplayError("RBC Survival Test", "Missing Lot Number", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.InjectionDate == (time_t) 0){
					Amulet_DisplayError("RBC Survival Test", "Missing Injection Date", TRUE);
					return;
				}
			}else{
				if(AmuletAddEditRBCSurvivalTest_test.FirstName[0] == 0){
					Amulet_DisplayError("RBC Survival Test", "Missing First Name", TRUE);
					return;
				}

				if(AmuletAddEditRBCSurvivalTest_test.LastName[0] == 0){
					Amulet_DisplayError("RBC Survival Test", "Missing Last Name", TRUE);
					return;
				}

				flgComplete = TRUE;
				if(AmuletAddEditRBCSurvivalTest_test.PatientID[0] == 0) flgComplete = FALSE;
				if(AmuletAddEditRBCSurvivalTest_test.DateOfBirth == 0) flgComplete = FALSE;
				if(AmuletAddEditRBCSurvivalTest_test.CountingTime == -1) flgComplete = FALSE;
				if(AmuletAddEditRBCSurvivalTest_test.CalibrationActivity == -1) flgComplete = FALSE;
				if(AmuletAddEditRBCSurvivalTest_test.CalibrationDate == (time_t) 0) flgComplete = FALSE;
				if(AmuletAddEditRBCSurvivalTest_test.LotNum[0] == 0) flgComplete = FALSE;
				if(AmuletAddEditRBCSurvivalTest_test.InjectionDate == (time_t) 0) flgComplete = FALSE;

				if(flgComplete) AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestPhase = 0;
			}

			DB_WriteRBCSurvivalTest(&AmuletAddEditRBCSurvivalTest_test, TRUE);
			SetAmuletBackHTML();
			return;
	}
}
