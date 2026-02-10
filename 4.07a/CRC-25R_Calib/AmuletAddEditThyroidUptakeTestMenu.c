#define PHASE_ADDEDITTHYROIDUPTAKETEST_PRE_INIT			0
#define PHASE_ADDEDITTHYROIDUPTAKETEST_WAIT				1
#define PHASE_ADDEDITTHYROIDUPTAKETEST_VIEW_PROTOCOL	2
#define PHASE_ADDEDITTHYROIDUPTAKETEST_SAVE				3


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
extern bool AmuletWellSetupThyroidUptakeProtocol_setupMode;
extern PROBETHYROIDUPTAKEPROTOCOL AmuletAddEditThyroidUptakeProtocol_protocol;

PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
static PROBETHYROIDUPTAKEPROTOCOL protocol;
void SetAmuletBackHTML(void);
void PushPageStack(unsigned char ucPage);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void trim(char *acByte);
unsigned char PopPageStack(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);

void AmuletAddEditThyroidUptakeTest_menu(void){
	bool flgComplete;
	int intvalue;
	char message[26];

	switch(m_iPhase){
		case PHASE_ADDEDITTHYROIDUPTAKETEST_PRE_INIT:
			if(m_ucClear == 90){
				if(DB_GetActiveTUProtocol() == 0){
					PopPageStack();
					Amulet_DisplayError("Add Thyroid Uptake Test", "There are no Thyroid Uptake Protocols\nPlease setup a protocol", TRUE);
					return;
				}

				m_ucClear = 0;
				if(AmuletAddEditThyroidUptakeTest_test.ProbeTUTestID == -1){
					AmuletAddEditThyroidUptakeTest_test.ProbeTUTestPhase = -1;
					AmuletAddEditThyroidUptakeTest_test.PatientID[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.FirstName[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.LastName[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.DateOfBirth = 0;
					AmuletAddEditThyroidUptakeTest_test.Sex[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.Physician[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.TechID[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.NuclideName[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.HalfLife = -999;
					AmuletAddEditThyroidUptakeTest_test.HalfLifeUnit = -1;
					AmuletAddEditThyroidUptakeTest_test.StartROI = 0;
					AmuletAddEditThyroidUptakeTest_test.EndROI = 0;
					AmuletAddEditThyroidUptakeTest_test.PeakEV = -1;
					AmuletAddEditThyroidUptakeTest_test.CountingTime = -1;
					AmuletAddEditThyroidUptakeTest_test.DoseMultiplier = -1;
					AmuletAddEditThyroidUptakeTest_test.DoseAdministeredOn = (time_t) 0;
					AmuletAddEditThyroidUptakeTest_test.CalibrationActivity = -1;
					AmuletAddEditThyroidUptakeTest_test.CalibrationDate = (time_t) 0;
					AmuletAddEditThyroidUptakeTest_test.LotNum[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.ProbeDistance = -1;
					AmuletAddEditThyroidUptakeTest_test.AccessionNumber[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID = -1;
					AmuletAddEditThyroidUptakeTest_test.CreatedOn = (time_t) 0;
					AmuletAddEditThyroidUptakeTest_test.LastUpdated = (time_t) 0;
					AmuletAddEditThyroidUptakeTest_test.InactiveReason[0] = 0;
					AmuletAddEditThyroidUptakeTest_test.Inactive = FALSE;

					AmuletWellSetupThyroidUptakeProtocol_setupMode = FALSE;
					SetAmuletHTML(AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM]);
					PushPageStack(AmuletHTMLIndex[WELLSETUPTHYROIDUPTAKEPROTOCOL_HTM]);
					return;
				}else{
					DB_RetrieveProbeThyroidUptakeTest(&AmuletAddEditThyroidUptakeTest_test);
					if(AmuletAddEditThyroidUptakeTest_test.ProbeTUTestID <= 0){
						Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
						return;
					}
				}
			}

			if(AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID == -1){
				SetAmuletBackHTML();
				return;
			}

			if(AmuletAddEditThyroidUptakeTest_test.ProbeTUTestID == -1){
				SetAmuletString(100, "Add Thyroid Uptake Test");
			}else{
				SetAmuletString(100, "Edit Thyroid Uptake Test");
			}

			trim(AmuletAddEditThyroidUptakeTest_test.FirstName);
			SetAmuletString(102, AmuletAddEditThyroidUptakeTest_test.FirstName); //FirstName
			trim(AmuletAddEditThyroidUptakeTest_test.LastName);
			SetAmuletString(103, AmuletAddEditThyroidUptakeTest_test.LastName); //LastName
			trim(AmuletAddEditThyroidUptakeTest_test.PatientID);
			SetAmuletString(104, AmuletAddEditThyroidUptakeTest_test.PatientID); //PatientID
			if(AmuletAddEditThyroidUptakeTest_test.DateOfBirth != 0.0){
				dateout_julian(message, AmuletAddEditThyroidUptakeTest_test.DateOfBirth);
				SetAmuletString(105, message); //DOB
			}else{
				SetAmuletString(105, ""); //DOB
			}

			SetAmuletString(106, AmuletAddEditThyroidUptakeTest_test.Sex); //Sex
			trim(AmuletAddEditThyroidUptakeTest_test.Physician);
			SetAmuletString(107, AmuletAddEditThyroidUptakeTest_test.Physician); //Physician
			trim(AmuletAddEditThyroidUptakeTest_test.TechID);
			SetAmuletString(108, AmuletAddEditThyroidUptakeTest_test.TechID); //TechID
			if(AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID == -1){
				SetAmuletString(109, ""); //Protocol Name
			}else{
				protocol.ProbeTUProtocolID = AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID;
				DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
				if(protocol.ProbeTUProtocolID <= 0){
					Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
					return;
				}
				SetAmuletString(109, protocol.ProtocolName); //Protocol Name
			}

			if(AmuletAddEditThyroidUptakeTest_test.CountingTime == -1){
				SetAmuletString(110, ""); //CountTime
			}else{
				sprintf(message, "%d sec", AmuletAddEditThyroidUptakeTest_test.CountingTime);
				SetAmuletString(110, message); //CountTime
			}

			if(AmuletAddEditThyroidUptakeTest_test.ProbeDistance == -1){
				SetAmuletString(111, ""); //Probe Distance
			}else{
				sprintf(message, "%d cm", AmuletAddEditThyroidUptakeTest_test.ProbeDistance);
				SetAmuletString(111, message); //Probe Distance
			}

			if(AmuletAddEditThyroidUptakeTest_test.CalibrationActivity < 0){
				SetAmuletString(112, ""); //Activity
			}else{
				if(current.system == CI) format_activity_system2(AmuletAddEditThyroidUptakeTest_test.CalibrationActivity, message);
				else format_activity_system_kbq(AmuletAddEditThyroidUptakeTest_test.CalibrationActivity, message);
				SetAmuletString(112, message); //Activity
			}

			if(AmuletAddEditThyroidUptakeTest_test.CalibrationDate == (time_t) 0){
				SetAmuletString(113, ""); //Cal Date
			}else{
				GetExtendedTimeInfo(&AmuletAddEditThyroidUptakeTest_test.CalibrationDate, message);
				SetAmuletString(113, message); //Cal Date
			}
			trim(AmuletAddEditThyroidUptakeTest_test.LotNum);
			SetAmuletString(114, AmuletAddEditThyroidUptakeTest_test.LotNum); // Lot Num

			if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier < 0){
				SetAmuletString(115, ""); // Multiplier
			}else{
				if((protocol.DoseMeasurementMethod == 2) || (protocol.DoseMeasurementMethod == 3)){
					intvalue = AmuletAddEditThyroidUptakeTest_test.DoseMultiplier;
					AmuletAddEditThyroidUptakeTest_test.DoseMultiplier = intvalue;
					sprintf(message, "%d", intvalue);
				}else if(protocol.DoseMeasurementMethod == 4){
					sprintf(message, "%.2f", AmuletAddEditThyroidUptakeTest_test.DoseMultiplier);
				}else{
					message[0] = 0;
				}
				SetAmuletString(115, message); // Multiplier
			}

			if((protocol.DoseMeasurementMethod == 2) || (protocol.DoseMeasurementMethod == 3)){
				SetAmuletString(116, "# of Doses:");
			}else if(protocol.DoseMeasurementMethod == 4){
				SetAmuletString(116, "Factor:");
			}else{
				SetAmuletString(116, "");
			}

			if(AmuletAddEditThyroidUptakeTest_test.ProbeTUTestPhase < 1){
				if(AmuletAddEditThyroidUptakeTest_test.ProbeTUTestID != -1) SetAmuletByte(88, 0xFF);
				SetAmuletByte(90, 0xFF);
				SetAmuletByte(92, 0xFF);
				SetAmuletByte(94, 0xFF);
				if((protocol.DoseMeasurementMethod == 2) || (protocol.DoseMeasurementMethod == 3)){
					SetAmuletByte(99, 0xFF);
					SetAmuletByte(96, 0xFF);
				}
				else if(protocol.DoseMeasurementMethod == 4){
					SetAmuletByte(99, 0xFF);
					SetAmuletByte(97, 0xFF);
				}
			}else{
				//if(AmuletAddEditThyroidUptakeTest_test.Inactive){
				//	SetAmuletByte(87, 0xFF);
				//}else{
				//	SetAmuletByte(89, 0xFF);
				//}
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(93, 0xFF);
				SetAmuletByte(95, 0xFF);
				if((protocol.DoseMeasurementMethod == 2) || (protocol.DoseMeasurementMethod == 3) || (protocol.DoseMeasurementMethod == 4)){
					SetAmuletByte(99, 0xFF);
					SetAmuletByte(98, 0xFF);
				}
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_ADDEDITTHYROIDUPTAKETEST_WAIT;
			break;

		case PHASE_ADDEDITTHYROIDUPTAKETEST_WAIT:
			break;

		case PHASE_ADDEDITTHYROIDUPTAKETEST_VIEW_PROTOCOL:
			beep_amulet();
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID = AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID;
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID = -1;
			m_ucClear = 88;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			m_iPhase = PHASE_ADDEDITTHYROIDUPTAKETEST_WAIT;
			break;

		case PHASE_ADDEDITTHYROIDUPTAKETEST_SAVE:
			beep_amulet();
			if(AmuletAddEditThyroidUptakeTest_test.ProbeTUTestPhase != -1){
				if(AmuletAddEditThyroidUptakeTest_test.FirstName[0] == 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing First Name", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.LastName[0] == 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Last Name", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.PatientID[0] == 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Patient ID", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.DateOfBirth == 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Date of Birth", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID == -1){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Thyroid Uptake Protocol", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.CountingTime == -1){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Count Time", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.ProbeDistance == -1){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Probe Distance", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.CalibrationActivity == -1){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Calibration Activity", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.CalibrationDate == (time_t) 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Calibration Date", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.LotNum[0] == 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Lot Number", TRUE);
					return;
				}

				protocol.ProbeTUProtocolID = AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID;
				DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
				if(protocol.ProbeTUProtocolID <= 0){
					Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
					return;
				}
				if(protocol.DoseMeasurementMethod == 2 || protocol.DoseMeasurementMethod == 3 || protocol.DoseMeasurementMethod == 4){
					if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier == -1){
						if(protocol.DoseMeasurementMethod == 2 || protocol.DoseMeasurementMethod == 3){
							Amulet_DisplayError("Thyroid Uptake Test", "Missing # of Doses", TRUE);
							return;
						}else if(protocol.DoseMeasurementMethod == 4){
							Amulet_DisplayError("Thyroid Uptake Test", "Missing Factor", TRUE);
							return;
						}
					}else{
						if(protocol.DoseMeasurementMethod == 2 || protocol.DoseMeasurementMethod == 3){
							if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier > 5){
								Amulet_DisplayError("Thyroid Uptake Test", "# of Doses is greater than 5", TRUE);
								return;
							}
							if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier < 2){
								Amulet_DisplayError("Thyroid Uptake Test", "# of Doses is less than 2", TRUE);
								return;
							}
						}

						if(protocol.DoseMeasurementMethod == 4){
							if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier > 99.99){
								Amulet_DisplayError("Thyroid Uptake Test", "Factor is greater than 99.99", TRUE);
								return;
							}

							if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier < 1.0){
								Amulet_DisplayError("Thyroid Uptake Test", "Factor is less than 1", TRUE);
								return;
							}
						}
					}
				}
			}else{
				if(AmuletAddEditThyroidUptakeTest_test.FirstName[0] == 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing First Name", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.LastName[0] == 0){
					Amulet_DisplayError("Thyroid Uptake Test", "Missing Last Name", TRUE);
					return;
				}

				if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier != -1){
					if(protocol.DoseMeasurementMethod == 2 || protocol.DoseMeasurementMethod == 3){
						if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier > 5){
							Amulet_DisplayError("Thyroid Uptake Test", "# of Doses is greater than 5", TRUE);
							return;
						}
						if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier < 2){
							Amulet_DisplayError("Thyroid Uptake Test", "# of Doses is less than 2", TRUE);
							return;
						}
					}

					if(protocol.DoseMeasurementMethod == 4){
						if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier > 99.99){
							Amulet_DisplayError("Thyroid Uptake Test", "Factor is greater than 99.99", TRUE);
							return;
						}

						if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier < 1.0){
							Amulet_DisplayError("Thyroid Uptake Test", "Factor is less than 1", TRUE);
							return;
						}
					}
				}

				flgComplete = TRUE;
				if(AmuletAddEditThyroidUptakeTest_test.PatientID[0] == 0) flgComplete = FALSE;
				if(AmuletAddEditThyroidUptakeTest_test.DateOfBirth == 0) flgComplete = FALSE;
				if(AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID == -1) flgComplete = FALSE;
				if(AmuletAddEditThyroidUptakeTest_test.CountingTime == -1) flgComplete = FALSE;
				if(AmuletAddEditThyroidUptakeTest_test.ProbeDistance == -1) flgComplete = FALSE;
				if(AmuletAddEditThyroidUptakeTest_test.CalibrationActivity == -1) flgComplete = FALSE;
				if(AmuletAddEditThyroidUptakeTest_test.CalibrationDate == (time_t) 0) flgComplete = FALSE;
				if(AmuletAddEditThyroidUptakeTest_test.LotNum[0] == 0) flgComplete = FALSE;
				protocol.ProbeTUProtocolID = AmuletAddEditThyroidUptakeTest_test.ProbeTUProtocolID;
				DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
				if(protocol.ProbeTUProtocolID <= 0){
					Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
					return;
				}
				if(protocol.DoseMeasurementMethod == 2 || protocol.DoseMeasurementMethod == 3 || protocol.DoseMeasurementMethod == 4){
					if(AmuletAddEditThyroidUptakeTest_test.DoseMultiplier == -1) flgComplete = FALSE;
				}

				if(flgComplete) AmuletAddEditThyroidUptakeTest_test.ProbeTUTestPhase = 0;
			}

			DB_WriteProbeThyroidUptakeTest(&AmuletAddEditThyroidUptakeTest_test, TRUE);
			SetAmuletBackHTML();
			return;

	}
}
