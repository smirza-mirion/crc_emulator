#define PHASE_WELLTHYROIDUPTAKETEST_PRE_INIT	0
#define PHASE_WELLTHYROIDUPTAKETEST_WAIT		1
#define PHASE_WELLTHYROIDUPTAKETEST_RUN			2
#define PHASE_WELLTHYROIDUPTAKETEST_PRINT		3
#define PHASE_WELLTHYROIDUPTAKETEST_LINE1		17
#define PHASE_WELLTHYROIDUPTAKETEST_LINE2		18
#define PHASE_WELLTHYROIDUPTAKETEST_LINE3		19
#define PHASE_WELLTHYROIDUPTAKETEST_LINE4		20
#define PHASE_WELLTHYROIDUPTAKETEST_LINE5		21
#define PHASE_WELLTHYROIDUPTAKETEST_LINE6		22
#define PHASE_WELLTHYROIDUPTAKETEST_LINE7		23
#define PHASE_WELLTHYROIDUPTAKETEST_LINE8		24
#define PHASE_WELLTHYROIDUPTAKETEST_VIEW		25
#define PHASE_WELLTHYROIDUPTAKETEST_ACTIVATE	26

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "sqlite3.h"
#include "database.h"
#include "printer.h"

typedef struct tudrilldown TUDRILLDOWN;
struct tudrilldown{
	int type;	// 0=None, 1=Dose, 2=Patient
	long long int UID;
};

extern int m_iPhase;
extern unsigned char m_ucClear;
extern time_t clock_time;
extern CURRENT current;
extern long long int AmuletWellThyroidUptakeEnterAdministrationDate_ProbeTUTestID;
extern PROBETHYROIDUPTAKEPATMEAS AmuletWellThyroidUptakeMeasurePatient;
extern PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasureDose_total;
extern PROBETHYROIDUPTAKENORMAL ThyroidUptake_normal[4];
extern PROBETHYROIDUPTAKEPROTOCOL AmuletAddEditThyroidUptakeProtocol_protocol;
extern uchar AmuletGenericItems_config;
extern uchar AmuletInactivate_config;

PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeTest;

static PROBETHYROIDUPTAKEPROTOCOL protocol;
static PROBETHYROIDUPTAKEPATMEAS patient;
static PROBETHYROIDUPTAKEDOSETOTALMEAS dose;
static BACKGND background;
static TUDRILLDOWN drill[8];

float nucdecay(float act0, time_t time0,time_t time1, float hl, short unit);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
void AmuletWellThyroidUptakeTest_print_narrow(void);
void AmuletWellThyroidUptakeTest_print_wide(void);
int CalcAge(double julian_date);
bool ThyroidUptakeCheckNormal(int hours, float uptake, int *min, int *max, bool *abnormal);

int roundfloat(float value){
	char charvalue[11];

	sprintf(charvalue, "%.0f", value);
	return atoi(charvalue);
}

void AmuletWellThyroidUptakeTest_menu(void){
	bool abnormal;
	char message[76], timestring[40], unitstring[10], boldstring[2], normalstring[2];
	float admin_activity, cpm_cps;
	int index, intvalue, curline, hours, NextMeasurementNumber, Age, normal_min, normal_max;

	switch(m_iPhase){
		case PHASE_WELLTHYROIDUPTAKETEST_PRE_INIT:
			if(m_ucClear == 91){
				DB_RetrieveProbeThyroidUptakeTest(&AmuletWellThyroidUptakeTest);
				if(AmuletWellThyroidUptakeTest.ProbeTUTestID <= 0){
					Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
					return;
				}
				m_ucClear = 0;
			}

			SetAmuletString(100, AmuletWellThyroidUptakeTest.PatientID);

			message[0] = 0;
			strcat(message, AmuletWellThyroidUptakeTest.LastName);
			strcat(message, ", ");
			strcat(message, AmuletWellThyroidUptakeTest.FirstName);
			delayloop(2);
			SetAmuletString(101, message);

			dateout_julian(message, AmuletWellThyroidUptakeTest.DateOfBirth);
			delayloop(2);
			SetAmuletString(103, message);

			Age = CalcAge(AmuletWellThyroidUptakeTest.DateOfBirth);
			sprintf(message, "%d", Age);
			delayloop(2);
			SetAmuletString(104, message);

			delayloop(2);
			SetAmuletString(105, AmuletWellThyroidUptakeTest.Sex);
			delayloop(2);
			SetAmuletString(106, AmuletWellThyroidUptakeTest.TechID);
			delayloop(2);
			SetAmuletString(107, AmuletWellThyroidUptakeTest.Physician);

			protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeTest.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
			if(protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}
			delayloop(2);
			SetAmuletString(108, protocol.ProtocolName);

			delayloop(2);
			SetAmuletString(109, AmuletWellThyroidUptakeTest.NuclideName);

			sprintf(message, "%.1f - %.1f", AmuletWellThyroidUptakeTest.StartROI, AmuletWellThyroidUptakeTest.EndROI);
			delayloop(2);
			SetAmuletString(110, message);

			switch(protocol.DoseMeasurementMethod){
				case 1:
					strcpy(message, "1");
					SetAmuletByte(92, 0xFF);
					break;

				case 2:
				case 3:
					intvalue = AmuletWellThyroidUptakeTest.DoseMultiplier;
					sprintf(message, "%d", intvalue);
					SetAmuletByte(92, 0xFF);
					break;

				case 4:
					sprintf(message, "%.2f", AmuletWellThyroidUptakeTest.DoseMultiplier);
					SetAmuletByte(93, 0xFF);
					break;
			}
			delayloop(2);
			SetAmuletString(111, message);

			delayloop(2);
			SetAmuletString(112, AmuletWellThyroidUptakeTest.LotNum);

			sprintf(message, "%d", AmuletWellThyroidUptakeTest.CountingTime);
			delayloop(2);
			SetAmuletString(113, message);

			sprintf(message, "%d", AmuletWellThyroidUptakeTest.ProbeDistance);
			delayloop(2);
			SetAmuletString(114, message);

			if(current.system == CI) format_activity_system2(AmuletWellThyroidUptakeTest.CalibrationActivity, message);
			else format_activity_system_kbq(AmuletWellThyroidUptakeTest.CalibrationActivity, message);
			delayloop(2);
			SetAmuletString(115, message);

			GetExtendedTimeInfo(&AmuletWellThyroidUptakeTest.CalibrationDate, message);
			delayloop(2);
			SetAmuletString(116, message);

			if(AmuletWellThyroidUptakeTest.DoseAdministeredOn == (time_t) 0){
				SetAmuletByte(90, 0x00);
			}else{
				SetAmuletByte(90, 0xFF);
				admin_activity = nucdecay(AmuletWellThyroidUptakeTest.CalibrationActivity, AmuletWellThyroidUptakeTest.CalibrationDate, AmuletWellThyroidUptakeTest.DoseAdministeredOn, AmuletWellThyroidUptakeTest.HalfLife, AmuletWellThyroidUptakeTest.HalfLifeUnit);
				if(current.system == CI) format_activity_system2(admin_activity, message);
				else format_activity_system_kbq(admin_activity, message);
				delayloop(2);
				SetAmuletString(117, message);
				GetExtendedTimeInfo(&AmuletWellThyroidUptakeTest.DoseAdministeredOn, message);
				delayloop(2);
				SetAmuletString(118, message);
			}

			if(!AmuletWellThyroidUptakeTest.Inactive){
				if(AmuletWellThyroidUptakeTest.ProbeTUTestPhase < 0){
					SetAmuletByte(94, 0x00);
					SetAmuletByte(99, 0x00);
				}
				else if(AmuletWellThyroidUptakeTest.ProbeTUTestPhase == 0){
					SetAmuletByte(94, 0x00);
					if(protocol.PreDoseMeasurement){
						delayloop(2);
						SetAmuletString(98, "Measure Pre-Dose Patient");
					}else{
						if(protocol.DoseDecayCorrect){
							delayloop(2);
							SetAmuletString(98, "Measure Administered Dose");
						}else{
							delayloop(2);
							SetAmuletString(98, "Enter Administration Date/Time");
						}
					}
					SetAmuletByte(99, 0xFF);
				}else{
					SetAmuletByte(94, 0xFF);
					SetAmuletString(97, "Complete");
					switch(AmuletWellThyroidUptakeTest.ProbeTUTestPhase){
						case 1:
							delayloop(2);
							SetAmuletString(98, "Measure Pre-Dose Patient");
							break;

						case 2:
							delayloop(2);
							SetAmuletString(98, "Measure Administered Dose");
							break;

						case 3:
							delayloop(2);
							SetAmuletString(98, "Enter Administration Date/Time");
							break;

						case 4:
							delayloop(2);
							SetAmuletString(98, "Measure Residual Liquid");
							break;

						case 5:
							delayloop(2);
							SetAmuletString(98, "Measure Patient");
							break;
					}
					SetAmuletByte(99, 0xFF);
				}
				SetAmuletByte(98, 0x00);
			}else{
				SetAmuletByte(94, 0xFF);
				SetAmuletString(97, "ReActivate");
				SetAmuletByte(99, 0x00);
				if(strlen(AmuletWellThyroidUptakeTest.InactiveReason)>0){
					delayloop(2);
					SetAmuletString(98, AmuletWellThyroidUptakeTest.InactiveReason);
					SetAmuletByte(98, 0xFF);
				}else{
					SetAmuletByte(98, 0x00);
				}
			}

			if(current.system == CI){
				strcpy(unitstring, "cpm");
			}else{
				strcpy(unitstring, "cps");
			}

			for(index=0; index<8; index++){
				drill[index].type = 0;
				drill[index].UID = -1;
			}

			boldstring[0] = 5;
			boldstring[1] = 0;
			normalstring[0] = 3;
			normalstring[1] = 0;

			curline = 0;
			if((protocol.PreDoseMeasurement) && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 1){
				patient.ProbeTUPatientMeasurementID = -1;
				patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
				patient.MeasurementNumber = 0;
				DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
				if(patient.ProbeTUPatientMeasurementID != -1){
					drill[curline].type = 2;
					drill[curline].UID = patient.ProbeTUPatientMeasurementID;
					GetExtendedTimeInfo(&patient.CreatedOn, timestring);
					if(patient.Uptake == -1){
						sprintf(message, " %s %sPre-Dose:%s Skipped", timestring, boldstring, normalstring);
					}else{
						cpm_cps = patient.AdjustedPatientROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						sprintf(message, " %s %sPre-Dose Net (%s):%s %.0f", timestring, boldstring, unitstring, normalstring, cpm_cps);
					}
					delayloop(10);
					SetAmuletString((3*curline) + 51, message);
					SetAmuletByte(curline + 81, 0xFF);
					curline++;
				}
			}

			if((protocol.DoseDecayCorrect) && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 2){
				dose.ProbeTUTotalDoseMeasurementID = -1;
				dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
				dose.MeasurementNumber = 0;
				DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
				if(dose.ProbeTUTotalDoseMeasurementID != -1){
					drill[curline].type = 1;
					drill[curline].UID = dose.ProbeTUTotalDoseMeasurementID;
					GetExtendedTimeInfo(&dose.CreatedOn, timestring);
					cpm_cps = dose.DoseROINetCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					sprintf(message, " %s %sAdmin Dose Net (%s):%s %.0f", timestring, boldstring, unitstring, normalstring, cpm_cps);
					delayloop(10);
					SetAmuletString((3*curline) + 51, message);
					SetAmuletByte(curline + 81, 0xFF);
					curline++;
				}
			}

			if((protocol.ResidualMeasurement) && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 4){
				dose.ProbeTUTotalDoseMeasurementID = -1;
				dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
				dose.MeasurementNumber = -1;
				DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
				if(dose.ProbeTUTotalDoseMeasurementID != -1){
					drill[curline].type = 1;
					drill[curline].UID = dose.ProbeTUTotalDoseMeasurementID;
					GetExtendedTimeInfo(&dose.CreatedOn, timestring);
					cpm_cps = dose.DoseROINetCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					sprintf(message, " %s %sResidual Liquid Net (%s):%s %.0f", timestring, boldstring, unitstring, normalstring, cpm_cps);
					delayloop(10);
					SetAmuletString((3*curline) + 51, message);
					SetAmuletByte(curline + 81, 0xFF);
					curline++;
				}
			}

			if(AmuletWellThyroidUptakeTest.ProbeTUTestPhase >= 5){
				NextMeasurementNumber = DB_NextThyroidUptakeMeasurementNumber(AmuletWellThyroidUptakeTest.ProbeTUTestID);

				if(NextMeasurementNumber > 1){
					for(index=1; index<NextMeasurementNumber; index++){
						patient.ProbeTUPatientMeasurementID = -1;
						patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
						patient.MeasurementNumber = index;
						DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
						if(patient.ProbeTUPatientMeasurementID != -1){
							if((curline == 7) && index < NextMeasurementNumber - 1){
								drill[curline].type = 0;
								drill[curline].UID = -1;
								sprintf(message, " %sPlease print test to view additional uptake measurements%s", boldstring, normalstring);
								delayloop(10);
								SetAmuletString((3*curline) + 51, message);
								SetAmuletByte(curline + 81, 0xFF);
								curline++;
								break;
							}
							drill[curline].type = 2;
							drill[curline].UID = patient.ProbeTUPatientMeasurementID;
							GetExtendedTimeInfo(&patient.CreatedOn, timestring);
							hours = roundfloat(patient.ElapsedTime);
							cpm_cps = patient.PatientROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(ThyroidUptakeCheckNormal(hours, patient.Uptake, &normal_min, &normal_max, &abnormal)){
								if(abnormal){
									sprintf(message, " %s %sAt %d Hours) Uptake:%s %.1f%%*** %s(Normal %d%%-%d%%)%s", timestring, boldstring, hours, normalstring, patient.Uptake, boldstring, normal_min, normal_max, normalstring);
								}else{
									sprintf(message, " %s %sAt %d Hours) Uptake:%s %.1f%% %s(Normal %d%%-%d%%)%s", timestring, boldstring, hours, normalstring, patient.Uptake, boldstring, normal_min, normal_max, normalstring);
								}
							}else{
								sprintf(message, " %s %sAt %d Hours) Uptake:%s %.1f%%", timestring, boldstring, hours, normalstring, patient.Uptake);
							}
							delayloop(10);
							SetAmuletString((3*curline) + 51, message);
							SetAmuletByte(curline + 81, 0xFF);
							curline++;
						}
					}
				}
			}

			//if(current.printer != NONE_PRINTER) SetAmuletByte(91, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(91, 0xFF);

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLTHYROIDUPTAKETEST_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKETEST_WAIT:
			break;

		case PHASE_WELLTHYROIDUPTAKETEST_RUN:
			beep_amulet();
			protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeTest.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
			if(protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}
			if(AmuletWellThyroidUptakeTest.ProbeTUTestPhase == 0){
				if(protocol.PreDoseMeasurement){
					if(DB_ExistsThyroidUptakePreDose(AmuletWellThyroidUptakeTest.ProbeTUTestID)){
						Amulet_DisplayError("Thyroid Uptake", "Pre-Dose Measurement Exists", TRUE);
						return;
					}else{
						AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID = -1;
						AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
						AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber  = 0;
						m_ucClear = 94;
						SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
						PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
						return;
					}
				}else{
					if(protocol.DoseDecayCorrect){
						if(DB_ExistsThyroidUptakeAdministeredDose(AmuletWellThyroidUptakeTest.ProbeTUTestID)){
							Amulet_DisplayError("Thyroid Uptake", "Administered Dose Measurement Exists", TRUE);
							return;
						}else{
							AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID = -1;
							AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber = 0;
							m_ucClear = 93;
							SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
							PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
							return;
						}
					}else{
						if(AmuletWellThyroidUptakeTest.DoseAdministeredOn == (time_t) 0){
							AmuletWellThyroidUptakeEnterAdministrationDate_ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							m_ucClear = 92;
							SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM]);
							PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM]);
							return;
						}else{
							Amulet_DisplayError("Thyroid Uptake", "Administration Date Exists", TRUE);
							return;
						}
					}
				}
			}else{
				switch(AmuletWellThyroidUptakeTest.ProbeTUTestPhase){
					case 1:
						if(DB_ExistsThyroidUptakePreDose(AmuletWellThyroidUptakeTest.ProbeTUTestID)){
							Amulet_DisplayError("Thyroid Uptake", "Pre-Dose Measurement Exists", TRUE);
							return;
						}else{
							AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID = -1;
							AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber  = 0;
							m_ucClear = 94;
							SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
							PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
							return;
						}

					case 2:
						if(DB_ExistsThyroidUptakeAdministeredDose(AmuletWellThyroidUptakeTest.ProbeTUTestID)){
							Amulet_DisplayError("Thyroid Uptake", "Administered Dose Measurement Exists", TRUE);
							return;
						}else{
							AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID = -1;
							AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber = 0;
							m_ucClear = 93;
							SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
							PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
							return;
						}

					case 3:
						if(AmuletWellThyroidUptakeTest.DoseAdministeredOn == (time_t) 0){
							AmuletWellThyroidUptakeEnterAdministrationDate_ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							m_ucClear = 92;
							SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM]);
							PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEENTERADMINISTRATIONDATE_HTM]);
							return;
						}else{
							Amulet_DisplayError("Thyroid Uptake", "Administration Date Exists", TRUE);
							return;
						}

					case 4:
						if(DB_ExistsThyroidUptakeResidualDose(AmuletWellThyroidUptakeTest.ProbeTUTestID)){
							Amulet_DisplayError("Thyroid Uptake", "Residual Liquid Measurement Exists", TRUE);
							return;
						}else{
							AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID = -1;
							AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber = -1;
							m_ucClear = 93;
							SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
							PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
							return;
						}

					case 5:
						AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID = -1;
						AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
						AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber = DB_NextThyroidUptakeMeasurementNumber(AmuletWellThyroidUptakeTest.ProbeTUTestID);
						m_ucClear = 94;
						SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
						PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
						return;
				}
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKETEST_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKETEST_PRINT:
			beep_amulet();
			if(current.printer == USB_PRINTER)
				AmuletWellThyroidUptakeTest_print_wide();
			else
				AmuletWellThyroidUptakeTest_print_narrow();
			SetAmuletByte(101, 0xFF);
			m_iPhase = PHASE_WELLTHYROIDUPTAKETEST_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKETEST_LINE1:
		case PHASE_WELLTHYROIDUPTAKETEST_LINE2:
		case PHASE_WELLTHYROIDUPTAKETEST_LINE3:
		case PHASE_WELLTHYROIDUPTAKETEST_LINE4:
		case PHASE_WELLTHYROIDUPTAKETEST_LINE5:
		case PHASE_WELLTHYROIDUPTAKETEST_LINE6:
		case PHASE_WELLTHYROIDUPTAKETEST_LINE7:
		case PHASE_WELLTHYROIDUPTAKETEST_LINE8:
			beep_amulet();
			if(drill[m_iPhase - PHASE_WELLTHYROIDUPTAKETEST_LINE1].type == 1){
				AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID = drill[m_iPhase - PHASE_WELLTHYROIDUPTAKETEST_LINE1].UID;
				m_ucClear = 93;
				SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
				return;
			}else if(drill[m_iPhase - PHASE_WELLTHYROIDUPTAKETEST_LINE1].type == 2){
				if(m_iPhase == PHASE_WELLTHYROIDUPTAKETEST_LINE1){
					patient.ProbeTUPatientMeasurementID = drill[m_iPhase - PHASE_WELLTHYROIDUPTAKETEST_LINE1].UID;
					DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
					if((patient.MeasurementNumber == 0) && (patient.Uptake == -1)){
						SetAmuletByte(102, 0xFF);
						m_iPhase = PHASE_WELLTHYROIDUPTAKETEST_WAIT;
						return;
					}else{
						AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID = drill[m_iPhase - PHASE_WELLTHYROIDUPTAKETEST_LINE1].UID;
						m_ucClear = 94;
						SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
						PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
					}
				}else{
					AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID = drill[m_iPhase - PHASE_WELLTHYROIDUPTAKETEST_LINE1].UID;
					m_ucClear = 94;
					SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
					PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREPATIENT_HTM]);
				}
				return;
			}

			SetAmuletByte(102, 0xFF);
			m_iPhase = PHASE_WELLTHYROIDUPTAKETEST_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKETEST_VIEW:
			beep_amulet();
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeTest.ProbeTUProtocolID;
			AmuletAddEditThyroidUptakeProtocol_protocol.ProbeTUProtocolGroupID = -1;
			m_ucClear = 88;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKEPROTOCOL_HTM]);
			m_iPhase = PHASE_WELLTHYROIDUPTAKETEST_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKETEST_ACTIVATE:
			beep_amulet();
			if(!AmuletWellThyroidUptakeTest.Inactive){
				AmuletInactivate_config = 9;
				m_ucClear = 63;
				SetAmuletHTML(AmuletHTMLIndex[INACTIVATE_HTM]);
				PushPageStack(AmuletHTMLIndex[INACTIVATE_HTM]);
				return;
			}else{
				AmuletGenericItems_config = 15;
				SetAmuletHTML(AmuletHTMLIndex[GENERICITEMS_HTM]);
				PushPageStack(AmuletHTMLIndex[GENERICITEMS_HTM]);
				return;
			}
			break;
	}
}

void AmuletWellThyroidUptakeTest_print_narrow(void){
	bool abnormal, flgDetectedValidNormal;
	char prtype;
	char strng[90];
	float cpm_cps;
	float admin_activity;
	char message[51], timestring[40];
	int index, jndex, NextMeasurementNumber, hours, normal_min, normal_max;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeTest.ProbeTUProtocolID;
		DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
		if(protocol.ProbeTUProtocolID <= 0){
			Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
			return;
		}

		rawheader(prtype, "Thyroid Uptake Report",  clock_time);
		pr_set_linecnt(5);

		/*lininit3(strng, TRUE, prtype);

		insertconst3(strng, 39, 1, 0, "@10123456789@8");
		pr_write3(strng, "Thyroid Uptake Report");

		insertconst3(strng, 0, 0, 0, "Italics ID:@8 patient ID");
		pr_write3(strng, "Thyroid Uptake Report");

		insertconst3(strng, 0, 0, 0, "@3BItalics ID:@8 patient ID");
		pr_write3(strng, "Thyroid Uptake Report");

		insertconst3(strng, 0, 0, 0, "@4Underline ID:@8 patient ID");
		pr_write3(strng, "Thyroid Uptake Report");

		insertconst3(strng, 0, 0, 0, "@5BUnderline ID:@8 patient ID");
		pr_write3(strng, "Thyroid Uptake Report");

		insertconst3(strng, 0, 0, 0, "@6IUnderline ID:@8 patient ID");
		pr_write3(strng, "Thyroid Uptake Report");

		insertconst3(strng, 0, 0, 0, "@7BIUnderline ID:@8 patient ID");
		pr_write3(strng, "Thyroid Uptake Report"); */

		lininit(strng, TRUE, prtype);

		insertstring(strng, 0, 0, 0, "   ID: %s", AmuletWellThyroidUptakeTest.PatientID);
		pr_write2(strng, "Thyroid Uptake Report");

		insertstring(strng, 0, 0, 0, "First: %s", AmuletWellThyroidUptakeTest.FirstName);
		pr_write2(strng, "Thyroid Uptake Report");

		insertstring(strng, 0, 0, 0, " Last: %s", AmuletWellThyroidUptakeTest.LastName);
		pr_write2(strng, "Thyroid Uptake Report");

		dateout_julian(message, AmuletWellThyroidUptakeTest.DateOfBirth);
		insertstring(strng, 0, 0, 0, "  DOB: %s", message);
		pr_write2(strng, "Thyroid Uptake Report");

		insertint(strng, 0, 0, 0, "  Age: %d", CalcAge(AmuletWellThyroidUptakeTest.DateOfBirth));
		pr_write2(strng, "Thyroid Uptake Report");

		// Sex
		insertstring(strng, 0, 0, 0, "  Sex: %s", AmuletWellThyroidUptakeTest.Sex);
		pr_write2(strng, "Thyroid Uptake Report");

		// Phys
		insertstring(strng, 0, 0, 0, "Physician: %s", AmuletWellThyroidUptakeTest.Physician);
		pr_write2(strng, "Thyroid Uptake Report");

		// Tech
		insertstring(strng, 0, 0, 0, " Tech: %s", AmuletWellThyroidUptakeTest.TechID);
		pr_write2(strng, "Thyroid Uptake Report");

		if(strlen(AmuletWellThyroidUptakeTest.InactiveReason)>0){
			insertstring(strng, 0, 0, 0, "INACTIVE: %s", AmuletWellThyroidUptakeTest.InactiveReason);
			pr_write2(strng, "Thyroid Uptake Report");
		}

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2(strng, "Thyroid Uptake Report");

		// Protocol
		insertstring(strng, 0, 0, 0, "Protocol: %s", protocol.ProtocolName);
		pr_write2(strng, "Thyroid Uptake Report");

		// Nuc
		insertstring(strng, 0, 0, 0, " Nuclide: %s", AmuletWellThyroidUptakeTest.NuclideName);
		pr_write2(strng, "Thyroid Uptake Report");

		// ROI
		sprintf(message, "%.1f - %.1f", AmuletWellThyroidUptakeTest.StartROI, AmuletWellThyroidUptakeTest.EndROI);
		insertstring(strng, 0, 0, 0, "ROI(keV): %s", message);
		pr_write2(strng, "Thyroid Uptake Report");

		// Halflife
		sprintf(message, "Halflife: %.2f", AmuletWellThyroidUptakeTest.HalfLife);
		switch(AmuletWellThyroidUptakeTest.HalfLifeUnit){
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
		pr_write2(strng, "Thyroid Uptake Report");
		pr_write2(strng, "Thyroid Uptake Report");

		// Dose Counting Method Decay Corrected or Reference
		if(protocol.DoseDecayCorrect){
			insertconst(strng, 0, 0, 0, "Counting Method: Decay Correct");
			pr_write2(strng, "Thyroid Uptake Report");
			insertconst(strng, 0, 0, 0, "Administered Dose");
			pr_write2(strng, "Thyroid Uptake Report");
		}else{
			insertconst(strng, 0, 0, 0, "Counting Method: Measure Same");
			pr_write2(strng, "Thyroid Uptake Report");
			insertconst(strng, 0, 0, 0, "Reference Dose Before Each");
			pr_write2(strng, "Thyroid Uptake Report");
			insertconst(strng, 0, 0, 0, "Uptake");
			pr_write2(strng, "Thyroid Uptake Report");
		}
		pr_write2(strng, "Thyroid Uptake Report");

		// Dose Measurment Method 1,2,3,4
		switch(protocol.DoseMeasurementMethod){
			case 1:
				insertconst(strng, 0, 0, 0, "Dose Measurement: Dose Measured");
				pr_write2(strng, "Thyroid Uptake Report");
				insertconst(strng, 0, 0, 0, "is Dose Administered");
				pr_write2(strng, "Thyroid Uptake Report");
				break;

			case 2:
				insertconst(strng, 0, 0, 0, "Dose Measurement: Measure Each");
				pr_write2(strng, "Thyroid Uptake Report");
				insertconst(strng, 0, 0, 0, "Dose and Add Activity");
				pr_write2(strng, "Thyroid Uptake Report");
				break;

			case 3:
				insertconst(strng, 0, 0, 0, "Dose Measurement: Measure One");
				pr_write2(strng, "Thyroid Uptake Report");
				insertconst(strng, 0, 0, 0, "Capsule and Multiply by Number");
				pr_write2(strng, "Thyroid Uptake Report");
				insertconst(strng, 0, 0, 0, "Administered");
				pr_write2(strng, "Thyroid Uptake Report");
				break;

			case 4:
				insertconst(strng, 0, 0, 0, "Dose Measurement: Measure Liquid");
				pr_write2(strng, "Thyroid Uptake Report");
				insertconst(strng, 0, 0, 0, "and Multiply by Factor");
				pr_write2(strng, "Thyroid Uptake Report");
				break;
		}
		pr_write2(strng, "Thyroid Uptake Report");

		// Dose Form
		if(protocol.DoseForm == 1) insertconst(strng, 0, 0, 0, "  Dose Form: Capsule");
		else insertconst(strng, 0, 0, 0, "  Dose Form: Liquid");
		pr_write2(strng, "Thyroid Uptake Report");

		// PreDose Measurement Y/N
		if(protocol.PreDoseMeasurement) insertconst(strng, 0, 0, 0, "   Pre-Dose: Yes");
		else insertconst(strng, 0, 0, 0, "   Pre-Dose: No");
		pr_write2(strng, "Thyroid Uptake Report");

		// Residual Measurement Y/N
		if(protocol.ResidualMeasurement) insertconst(strng, 0, 0, 0, "   Residual: Yes");
		else insertconst(strng, 0, 0, 0, "   Residual: No");
		pr_write2(strng, "Thyroid Uptake Report");

		// Lot Num
		insertstring(strng, 0, 0, 0, "    Lot Num: %s", AmuletWellThyroidUptakeTest.LotNum);
		pr_write2(strng, "Thyroid Uptake Report");

		// Units Given
		if(protocol.DoseMeasurementMethod == 1) insertconst(strng, 0, 0, 0, "Units Given: 1");
		else if(protocol.DoseMeasurementMethod == 4)insertfloat(strng, 0, 0, 0, "     Factor: %.2f", AmuletWellThyroidUptakeTest.DoseMultiplier);
		else insertfloat(strng, 0, 0, 0, "   Units Given: %.0f", AmuletWellThyroidUptakeTest.DoseMultiplier);
		pr_write2(strng, "Thyroid Uptake Report");

		// CountTime
	   	insertint(strng, 0, 0, 0, "    Count Time: %d sec", AmuletWellThyroidUptakeTest.CountingTime);
		pr_write2(strng, "Thyroid Uptake Report");

		// Probe Distance
		insertint(strng, 0, 0, 0, "Probe Distance: %d cm", AmuletWellThyroidUptakeTest.ProbeDistance);
		pr_write2(strng, "Thyroid Uptake Report");

		insertconst(strng, 0, 0, 0, "==================================");
		pr_write2(strng, "Thyroid Uptake Report");

		// Calibrated Activity
		insertconst(strng, 0, 0, 0, "Calibrated");
		pr_write2(strng, "Thyroid Uptake Report");
		if(current.system == CI) format_activity_system2(AmuletWellThyroidUptakeTest.CalibrationActivity, message);
		else format_activity_system_kbq(AmuletWellThyroidUptakeTest.CalibrationActivity, message);
		replace(message, '$', 'u');
		insertstring(strng, 0, 0, 0, " Activity: %s", message);
		pr_write2(strng, "Thyroid Uptake Report");

		// Calibrated Date
		GetExtendedTimeInfo(&AmuletWellThyroidUptakeTest.CalibrationDate, message);
		insertstring(strng, 0, 0, 0, "     Date: %s", message);
		pr_write2(strng, "Thyroid Uptake Report");

		if(AmuletWellThyroidUptakeTest.DoseAdministeredOn != (time_t) 0){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "Thyroid Uptake Report");

			// Administered Activity
			insertconst(strng, 0, 0, 0, "Administered");
			pr_write2(strng, "Thyroid Uptake Report");
			admin_activity = nucdecay(AmuletWellThyroidUptakeTest.CalibrationActivity, AmuletWellThyroidUptakeTest.CalibrationDate, AmuletWellThyroidUptakeTest.DoseAdministeredOn, AmuletWellThyroidUptakeTest.HalfLife, AmuletWellThyroidUptakeTest.HalfLifeUnit);
			if(current.system == CI) format_activity_system2(admin_activity, message);
			else format_activity_system_kbq(admin_activity, message);
			replace(message, '$', 'u');
			insertstring(strng, 0, 0, 0, " Activity: %s", message);
			pr_write2(strng, "Thyroid Uptake Report");

			// Administered Date
			GetExtendedTimeInfo(&AmuletWellThyroidUptakeTest.DoseAdministeredOn, message);
			insertstring(strng, 0, 0, 0, "     Date: %s", message);
			pr_write2(strng, "Thyroid Uptake Report");
		}

		// Print Summary
		NextMeasurementNumber = DB_NextThyroidUptakeMeasurementNumber(AmuletWellThyroidUptakeTest.ProbeTUTestID);
		if((AmuletWellThyroidUptakeTest.ProbeTUTestPhase >= 5) && (NextMeasurementNumber > 1)){
			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "Thyroid Uptake Report");

			insertconst(strng, 0, 0, 0, "SUMMARY");
			pr_write2(strng, "Thyroid Uptake Report");

			insertconst(strng, 0, 0, 0, "Hour Date/Time         %Uptake");
			pr_write2(strng, "Thyroid Uptake Report");

			for(index=1; index<NextMeasurementNumber; index++){
				patient.ProbeTUPatientMeasurementID = -1;
				patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
				patient.MeasurementNumber = index;
				DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
				if(patient.ProbeTUPatientMeasurementID != -1){
					hours = roundfloat(patient.ElapsedTime);
					GetExtendedTimeInfo(&patient.CreatedOn, timestring);
					insertint(strng, 2, 1, 3, "%d", hours);
					insertconst(strng, 5, 0, 0, timestring);
					if(ThyroidUptakeCheckNormal(hours, patient.Uptake, &normal_min, &normal_max, &abnormal)){
						if(abnormal){
							insertfloat(strng, 31, 1, 0, "%.1f%%***", patient.Uptake);
						}else{
							insertfloat(strng, 28, 1, 0, "%.1f%%", patient.Uptake);
						}
					}else{
						insertfloat(strng, 28, 1, 0, "%.1f%%", patient.Uptake);
					}
					pr_write2(strng, "Thyroid Uptake Report");
				}
			}

			flgDetectedValidNormal = FALSE;
			for(index=0; index<4; index++){
				if(ThyroidUptake_normal[index].hour != -1){
					flgDetectedValidNormal = TRUE;
					break;
				}
			}

			if(flgDetectedValidNormal){
				pr_write2(strng, "Thyroid Uptake Report");
				insertconst(strng, 0, 0, 0, "NORMAL VALUES");
				pr_write2(strng, "Thyroid Uptake Report");
				insertconst(strng, 0, 0, 0, "Hour     Min     Max");
				pr_write2(strng, "Thyroid Uptake Report");
				for(index=0; index<4; index++){
					if(ThyroidUptake_normal[index].hour != -1){
						insertint(strng, 2, 1, 0, "%d", ThyroidUptake_normal[index].hour);
						insertint(strng, 11, 1, 0, "%d%%", ThyroidUptake_normal[index].min);
						insertint(strng, 19, 1, 0, "%d%%", ThyroidUptake_normal[index].max);
						pr_write2(strng, "Thyroid Uptake Report");
					}
				}
			}
		}

		// Pre-Dose
		if(protocol.PreDoseMeasurement && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 1){
			patient.ProbeTUPatientMeasurementID = -1;
			patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
			patient.MeasurementNumber = 0;
			DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
			if(patient.ProbeTUPatientMeasurementID != -1){
				insertconst(strng, 0, 0, 0, "==================================");
				pr_write2(strng, "Thyroid Uptake Report");
				GetExtendedTimeInfo(&patient.CreatedOn, timestring);
				insertconst(strng, 0, 0, 0, "Pre-Dose");
				insertstring(strng, 32, 1, 0, "%s", timestring);
				pr_write2(strng, "Thyroid Uptake Report");
				if(patient.Uptake == -1){
					insertconst(strng, 0, 0, 0, "     SKIPPED");
					pr_write2(strng, "Thyroid Uptake Report");
				}else{
					cpm_cps = patient.LegROIAverageCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background Avg(cpm): %.0f", cpm_cps);
					else  insertfloat(strng, 0, 0, 0, "Background Avg(cps): %.0f", cpm_cps);
					pr_write2(strng, "Thyroid Uptake Report");

					cpm_cps = patient.Leg1ROICPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI ) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
					pr_write2(strng, "Thyroid Uptake Report");

					if(patient.Leg2_SpectraID != -1){
						cpm_cps = patient.Leg2ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");
					}
					pr_write2(strng, "Thyroid Uptake Report");

					cpm_cps = patient.NeckROIAverageCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "      Neck Avg(cpm): %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "      Neck Avg(cps): %.0f", cpm_cps);
					pr_write2(strng, "Thyroid Uptake Report");

					cpm_cps = patient.Neck1ROICPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
					pr_write2(strng, "Thyroid Uptake Report");

					if(patient.Neck2_SpectraID != -1){
						cpm_cps = patient.Neck2ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");
					}
					pr_write2(strng, "Thyroid Uptake Report");

					cpm_cps = patient.PatientROINetCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Net Counts(cpm): %.0f", cpm_cps);
					else insertfloat(strng, 0, 0, 0, "    Net Counts(cps): %.0f", cpm_cps);
					pr_write2(strng, "Thyroid Uptake Report");

					if(strlen(patient.comment) != 0){
						insertstring(strng, 0, 0, 0, "Comment: %s", patient.comment);
						pr_write2(strng, "Thyroid Uptake Report");
					}
				}
			}
		}


		// Admin Dose
		if(protocol.DoseDecayCorrect && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 2){
			dose.ProbeTUTotalDoseMeasurementID = -1;
			dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
			dose.MeasurementNumber = 0;
			DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
			if(dose.ProbeTUTotalDoseMeasurementID != -1){
				insertconst(strng, 0, 0, 0, "==================================");
				pr_write2(strng, "Thyroid Uptake Report");
				GetExtendedTimeInfo(&dose.CreatedOn, timestring);
				insertconst(strng, 0, 0, 0, "Admin Dose");
				insertstring(strng, 32, 1, 0, "%s", timestring);
				pr_write2(strng, "Thyroid Uptake Report");

				background.WellBackgroundID = dose.WellBackgroundID;
				DB_RetrieveBackgnd(&background);

				cpm_cps = dose.BackgroundROICPM;
				if(current.system != CI) cpm_cps /= 60.0;
				if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background(cpm): %.0f", cpm_cps);
				else insertfloat(strng, 0, 0, 0, "Background(cps): %.0f", cpm_cps);
				pr_write2(strng, "Thyroid Uptake Report");

				GetExtendedTimeInfo(&background.CreatedOn, timestring);
				insertstring(strng, 0, 0, 0, "     %s", timestring);
				pr_write2(strng, "Thyroid Uptake Report");

				pr_write2(strng, "Thyroid Uptake Report");

				switch(protocol.DoseMeasurementMethod){
					case 1:
						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
						dose.SingleMeasurement[0].DoseSubUnit = 1;
						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
							insertconst(strng, 0, 0, 0, "Dose #1");
							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
							insertstring(strng, 32, 1, 0, "%s", timestring);
							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");

							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");
							}

							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.DoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Dose(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "Net Dose(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");
						}
						break;

					case 2:
						for(index=0; index < 5; index++){
							dose.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
							dose.SingleMeasurement[index].DoseSubUnit = index + 1;
							DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[index]));

							if(dose.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID != -1){
								insertint(strng, 0, 0, 0, "Dose #%d", index + 1);
								GetExtendedTimeInfo(&(dose.SingleMeasurement[index].CreatedOn), timestring);
								insertstring(strng, 32, 1, 0, "%s", timestring);
								pr_write2(strng, "Thyroid Uptake Report");

								cpm_cps = dose.SingleMeasurement[index].DoseROIAverageCPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");

								cpm_cps = dose.SingleMeasurement[index].Dose1ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");

								if(dose.SingleMeasurement[index].Dose2_SpectraID != -1){
									cpm_cps = dose.SingleMeasurement[index].Dose2ROICPM;
									if(current.system != CI) cpm_cps /= 60.0;
									if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
									else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
									pr_write2(strng, "Thyroid Uptake Report");
								}

								pr_write2(strng, "Thyroid Uptake Report");
							}
						}

						cpm_cps = dose.DoseROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						cpm_cps = dose.DoseROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");
						break;

					case 3:
					case 4:
						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
						dose.SingleMeasurement[0].DoseSubUnit = 1;
						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
							insertconst(strng, 0, 0, 0, "Dose #1");
							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
							insertstring(strng, 32, 1, 0, "%s", timestring);
							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");

							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");
							}
							pr_write2(strng, "Thyroid Uptake Report");
						}

						cpm_cps = dose.DoseROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						cpm_cps = dose.DoseROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");
						break;
				}
			}
		}

		// Residual
		if((protocol.ResidualMeasurement) && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 4){
			dose.ProbeTUTotalDoseMeasurementID = -1;
			dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
			dose.MeasurementNumber = -1;
			DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
			if(dose.ProbeTUTotalDoseMeasurementID != -1){
				insertconst(strng, 0, 0, 0, "==================================");
				pr_write2(strng, "Thyroid Uptake Report");
				GetExtendedTimeInfo(&dose.CreatedOn, timestring);
				insertconst(strng, 0, 0, 0, "Residual");
				insertstring(strng, 32, 1, 0, "%s", timestring);
				pr_write2(strng, "Thyroid Uptake Report");

				background.WellBackgroundID = dose.WellBackgroundID;
				DB_RetrieveBackgnd(&background);

				cpm_cps = dose.BackgroundROICPM;
				if(current.system != CI) cpm_cps /= 60.0;
				if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background(cpm): %.0f", cpm_cps);
				else insertfloat(strng, 0, 0, 0, "Background(cps): %.0f", cpm_cps);
				pr_write2(strng, "Thyroid Uptake Report");

				GetExtendedTimeInfo(&background.CreatedOn, timestring);
				insertstring(strng, 0, 0, 0, "     %s", timestring);
				pr_write2(strng, "Thyroid Uptake Report");

				pr_write2(strng, "Thyroid Uptake Report");

				switch(protocol.DoseMeasurementMethod){
					case 1:
					case 3:
					case 4:
						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
						dose.SingleMeasurement[0].DoseSubUnit = 1;
						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));
						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
							insertconst(strng, 0, 0, 0, "Residual");
							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
							insertstring(strng, 32, 1, 0, "%s", timestring);
							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");

							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");
							}

							pr_write2(strng, "Thyroid Uptake Report");

							cpm_cps = dose.DoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Residual(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "Net Residual(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");
						}
						break;

					case 2:
						for(index=0; index < 5; index++){
							dose.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
							dose.SingleMeasurement[index].DoseSubUnit = index + 1;
							DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[index]));

							if(dose.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID != -1){
								insertint(strng, 0, 0, 0, "Residual #%d", index + 1);
								GetExtendedTimeInfo(&(dose.SingleMeasurement[index].CreatedOn), timestring);
								insertstring(strng, 32, 1, 0, "%s", timestring);
								pr_write2(strng, "Thyroid Uptake Report");

								cpm_cps = dose.SingleMeasurement[index].DoseROIAverageCPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");

								cpm_cps = dose.SingleMeasurement[index].Dose1ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");

								if(dose.SingleMeasurement[index].Dose2_SpectraID != -1){
									cpm_cps = dose.SingleMeasurement[index].Dose2ROICPM;
									if(current.system != CI) cpm_cps /= 60.0;
									if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
									else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
									pr_write2(strng, "Thyroid Uptake Report");
								}

								pr_write2(strng, "Thyroid Uptake Report");
							}
						}

						cpm_cps = dose.DoseROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Residual(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "Total Residual(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						cpm_cps = dose.DoseROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Residual(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "  Net Residual(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");
						break;
				}
			}
		}

		// Measurement
		if(AmuletWellThyroidUptakeTest.ProbeTUTestPhase >= 5){
			NextMeasurementNumber = DB_NextThyroidUptakeMeasurementNumber(AmuletWellThyroidUptakeTest.ProbeTUTestID);
			if(NextMeasurementNumber > 1){
				for(index=1; index<NextMeasurementNumber; index++){
					patient.ProbeTUPatientMeasurementID = -1;
					patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
					patient.MeasurementNumber = index;
					DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
					if(patient.ProbeTUPatientMeasurementID != -1){
						insertconst(strng, 0, 0, 0, "==================================");
						pr_write2(strng, "Thyroid Uptake Report");
						GetExtendedTimeInfo(&patient.CreatedOn, timestring);
						hours = roundfloat(patient.ElapsedTime);
						insertint(strng, 0, 0, 0, "Uptake: %d Hrs", hours);
						insertstring(strng, 32, 1, 0, "%s", timestring);
						pr_write2(strng, "Thyroid Uptake Report");
						pr_write2(strng, "Thyroid Uptake Report");

						if(protocol.DoseDecayCorrect){
							cpm_cps = patient.DoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Decayed Admin Dose(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "Decayed Admin Dose(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");
						}else{
							dose.ProbeTUTotalDoseMeasurementID = -1;
							dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							dose.MeasurementNumber = index;
							DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
							if(dose.ProbeTUTotalDoseMeasurementID != -1){
								background.WellBackgroundID = dose.WellBackgroundID;
								DB_RetrieveBackgnd(&background);

								cpm_cps = dose.BackgroundROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								if(current.system == CI) insertfloat(strng, 0, 0, 0, "Dose Background(cpm): %.0f", cpm_cps);
								else insertfloat(strng, 0, 0, 0, "Dose Background(cps): %.0f", cpm_cps);
								pr_write2(strng, "Thyroid Uptake Report");

								GetExtendedTimeInfo(&background.CreatedOn, timestring);
								insertstring(strng, 0, 0, 0, "     %s", timestring);
								pr_write2(strng, "Thyroid Uptake Report");

								switch(protocol.DoseMeasurementMethod){
									case 1:
										dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
										dose.SingleMeasurement[0].DoseSubUnit = 1;
										DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

										if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
											insertconst(strng, 0, 0, 0, "Dose #1");
											GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
											insertstring(strng, 32, 1, 0, "%s", timestring);
											pr_write2(strng, "Thyroid Uptake Report");

											cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
											if(current.system != CI) cpm_cps /= 60.0;
											if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
											else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
											pr_write2(strng, "Thyroid Uptake Report");

											cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
											if(current.system != CI) cpm_cps /= 60.0;
											if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
											else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
											pr_write2(strng, "Thyroid Uptake Report");

											if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
												cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
												if(current.system != CI) cpm_cps /= 60.0;
												if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
												else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
												pr_write2(strng, "Thyroid Uptake Report");
											}

											cpm_cps = patient.DoseROINetCPM;
											if(current.system != CI) cpm_cps /= 60.0;
											if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Dose(cpm): %.0f", cpm_cps);
											else insertfloat(strng, 0, 0, 0, "Net Dose(cps): %.0f", cpm_cps);
											pr_write2(strng, "Thyroid Uptake Report");
										}
										break;

									case 2:
										for(jndex=0; jndex < 5; jndex++){
											dose.SingleMeasurement[jndex].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
											dose.SingleMeasurement[jndex].DoseSubUnit = jndex + 1;
											DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[jndex]));

											if(dose.SingleMeasurement[jndex].ProbeTUSingleDoseMeasurementID != -1){
												insertint(strng, 0, 0, 0, "Dose #%d", jndex + 1);
												GetExtendedTimeInfo(&(dose.SingleMeasurement[jndex].CreatedOn), timestring);
												insertstring(strng, 32, 1, 0, "%s", timestring);
												pr_write2(strng, "Thyroid Uptake Report");

												cpm_cps = dose.SingleMeasurement[jndex].DoseROIAverageCPM;
												if(current.system != CI) cpm_cps /= 60.0;
												if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
												else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
												pr_write2(strng, "Thyroid Uptake Report");

												cpm_cps = dose.SingleMeasurement[jndex].Dose1ROICPM;
												if(current.system != CI) cpm_cps /= 60.0;
												if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
												else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
												pr_write2(strng, "Thyroid Uptake Report");

												if(dose.SingleMeasurement[jndex].Dose2_SpectraID != -1){
													cpm_cps = dose.SingleMeasurement[jndex].Dose2ROICPM;
													if(current.system != CI) cpm_cps /= 60.0;
													if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
													else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
													pr_write2(strng, "Thyroid Uptake Report");
												}
											}
										}

										cpm_cps = dose.DoseROICPM;
										if(current.system != CI) cpm_cps /= 60.0;
										if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
										else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
										pr_write2(strng, "Thyroid Uptake Report");

										cpm_cps = patient.DoseROINetCPM;
										if(current.system != CI) cpm_cps /= 60.0;
										if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
										else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
										pr_write2(strng, "Thyroid Uptake Report");
										break;

									case 3:
									case 4:
										dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
										dose.SingleMeasurement[0].DoseSubUnit = 1;
										DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

										if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
											insertconst(strng, 0, 0, 0, "Dose #1");
											GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
											insertstring(strng, 32, 1, 0, "%s", timestring);
											pr_write2(strng, "Thyroid Uptake Report");

											cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
											if(current.system != CI) cpm_cps /= 60.0;
											if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
											else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
											pr_write2(strng, "Thyroid Uptake Report");

											cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
											if(current.system != CI) cpm_cps /= 60.0;
											if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
											else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
											pr_write2(strng, "Thyroid Uptake Report");

											if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
												cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
												if(current.system != CI) cpm_cps /= 60.0;
												if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
												else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
												pr_write2(strng, "Thyroid Uptake Report");
											}
										}

										cpm_cps = dose.DoseROICPM;
										if(current.system != CI) cpm_cps /= 60.0;
										if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
										else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
										pr_write2(strng, "Thyroid Uptake Report");

										cpm_cps = patient.DoseROINetCPM;
										if(current.system != CI) cpm_cps /= 60.0;
										if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
										else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
										pr_write2(strng, "Thyroid Uptake Report");
										break;
								}
							}
						}

						if(protocol.ResidualMeasurement){
							cpm_cps = patient.AdjustedDoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Residual Adjusted(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "Residual Adjusted(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");
						}

						pr_write2(strng, "Thyroid Uptake Report");

						cpm_cps = patient.LegROIAverageCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Patient Backgnd Avg(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "Patient Backgnd Avg(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						cpm_cps = patient.Leg1ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI ) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						if(patient.Leg2_SpectraID != -1){
							cpm_cps = patient.Leg2ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");
						}

						cpm_cps = patient.NeckROIAverageCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Neck Avg(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "Neck Avg(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						cpm_cps = patient.Neck1ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						if(patient.Neck2_SpectraID != -1){
							cpm_cps = patient.Neck2ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");
						}

						cpm_cps = patient.PatientROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Counts(cpm): %.0f", cpm_cps);
						else insertfloat(strng, 0, 0, 0, "Net Counts(cps): %.0f", cpm_cps);
						pr_write2(strng, "Thyroid Uptake Report");

						if(protocol.PreDoseMeasurement){
							cpm_cps = patient.AdjustedPatientROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							if(current.system == CI) insertfloat(strng, 0, 0, 0, "PreDose Adjusted(cpm): %.0f", cpm_cps);
							else insertfloat(strng, 0, 0, 0, "PreDose Adjusted(cps): %.0f", cpm_cps);
							pr_write2(strng, "Thyroid Uptake Report");
						}

						pr_write2(strng, "Thyroid Uptake Report");

						if(strlen(patient.comment) != 0){
							insertstring(strng, 0, 0, 0, "Comment: %s", patient.comment);
							pr_write2(strng, "Thyroid Uptake Report");
						}

						if(ThyroidUptakeCheckNormal(hours, patient.Uptake, &normal_min, &normal_max, &abnormal)){
							if(abnormal){
								insertfloat(strng, 0, 0, 0, "%%Uptake: %.1f%% (Outside Normal)", patient.Uptake);
							}else{
								insertfloat(strng, 0, 0, 0, "%%Uptake: %.1f%%", patient.Uptake);
							}
						}else{
							insertfloat(strng, 0, 0, 0, "%%Uptake: %.1f%%", patient.Uptake);
						}
						pr_write2(strng, "Thyroid Uptake Report");
					}
				}
			}
		}
		formfeed(prtype);
	}
}

extern short diag_page_num;
void print_page_num(void);
void AmuletWellThyroidUptakeTest_print_wide(void){
	bool abnormal, flgDetectedValidNormal;
	char prtype;
	char strng[200];
	float cpm_cps;
	float admin_activity;
	char message[51], timestring[40];
	int index, jndex, NextMeasurementNumber, hours, normal_min, normal_max;
	char name[40];
	char ustr[4];

	prtype = current.printer;
	diag_page_num = 0;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeTest.ProbeTUProtocolID;
		DB_RetrieveProbeThyroidUptakeProtocol(&protocol);
		if(protocol.ProbeTUProtocolID <= 0){
			Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
			return;
		}

	if(current.system == CI)
		strcpy(ustr,"cpm");
	else
		strcpy(ustr,"cps");

		rawheader(prtype, "Thyroid Uptake Report",  clock_time);
		pr_set_linecnt(5);

		//set to 12cpi
		pcl_cpi("12",TRUE);

		
		lininit3(strng, TRUE, prtype);

		//Patient Demographics     Radiopharmaceutical Information
		insertconst3(strng,0,0,0,"@5Patient Demographics@8");
		insertconst3(strng,47,0,0,"@5Radiopharmaceutical Information@8");
		//insertconst3(strng,48,0,0,"@5Radiopharmaceutical Information@8");
		pr_write3(strng, "Thyroid Uptake Report");
		feed(1,prtype);
		
		//patient name, nuclide, halflife
		strcpy(name,AmuletWellThyroidUptakeTest.LastName);
		strcat(name,",");
		strcat(name,AmuletWellThyroidUptakeTest.FirstName);
		insertstring3(strng,0,0,47,"@1Patient Name:@8 %s",name);
		insertstring3(strng, 48, 0, 0, "@1Nuclide:@8 %s", AmuletWellThyroidUptakeTest.NuclideName);
		sprintf(message, "%.2f", AmuletWellThyroidUptakeTest.HalfLife);
		switch(AmuletWellThyroidUptakeTest.HalfLifeUnit){
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
		insertstring3(strng, 66, 0, 0, "%s", message);
		pr_write3(strng, "Thyroid Uptake Report");
		
		insertstring3(strng, 0, 0, 0, "@1Patient ID  :@8 %s", AmuletWellThyroidUptakeTest.PatientID);
		// Administered Activity
		if(AmuletWellThyroidUptakeTest.DoseAdministeredOn != (time_t) 0){
			admin_activity = nucdecay(AmuletWellThyroidUptakeTest.CalibrationActivity, AmuletWellThyroidUptakeTest.CalibrationDate, AmuletWellThyroidUptakeTest.DoseAdministeredOn, AmuletWellThyroidUptakeTest.HalfLife, AmuletWellThyroidUptakeTest.HalfLifeUnit);
			if(current.system == CI) format_activity_system2(admin_activity, message);
			else format_activity_system_kbq(admin_activity, message);
			replace(message, '$', MU_PRT);
			insertstring3(strng,48, 0,0, "@1Administered:@8 %s", message);
		}
		else
			insertconst3(strng,48,0,0,"@1Administered:@8");
		pr_write3(strng, "Thyroid Uptake Report");
		
		dateout_julian(message, AmuletWellThyroidUptakeTest.DateOfBirth);
		insertstring3(strng, 0, 0, 0, "@1DOB         :@8 %s", message);
		if(AmuletWellThyroidUptakeTest.DoseAdministeredOn != (time_t) 0){
			GetExtendedTimeInfo(&AmuletWellThyroidUptakeTest.DoseAdministeredOn, message);
			insertstring3(strng, 48, 0, 0, "@1At:@8 %s", message);
		}
		else
			insertconst3(strng,48,0,0,"@1At:@8");
		pr_write3(strng, "Thyroid Uptake Report");

		insertint3(strng, 0, 0, 0, "@1Age         :@8 %d", CalcAge(AmuletWellThyroidUptakeTest.DateOfBirth));
		if(current.system == CI) format_activity_system2(AmuletWellThyroidUptakeTest.CalibrationActivity, message);
		else format_activity_system_kbq(AmuletWellThyroidUptakeTest.CalibrationActivity, message);
		replace(message, '$', MU_PRT);
		insertstring3(strng, 48, 0, 0, "@1Calibrated@8: %s", message);
		pr_write3(strng, "Thyroid Uptake Report");
		
		insertstring3(strng, 0, 0, 0, "@1Sex         :@8 %s", AmuletWellThyroidUptakeTest.Sex);
		GetExtendedTimeInfo(&AmuletWellThyroidUptakeTest.CalibrationDate, message);
		insertstring3(strng, 48, 0, 0, "@1At:@8 %s", message);
		pr_write3(strng, "Thyroid Uptake Report");

		insertstring3(strng, 0, 0, 0, "@1Physician   :@8 %s", AmuletWellThyroidUptakeTest.Physician);
		insertstring3(strng, 48, 0, 0, "@1Lot #:@8 %s", AmuletWellThyroidUptakeTest.LotNum);
		pr_write3(strng, "Thyroid Uptake Report");
	
		insertstring3(strng, 0, 0, 0, "@1Technologist@8: %s", AmuletWellThyroidUptakeTest.TechID);
	   	insertint3(strng, 48, 0, 0, "@1Count Time:@8 %d sec", AmuletWellThyroidUptakeTest.CountingTime);
		pr_write3(strng, "Thyroid Uptake Report");
		
		insertstring3(strng, 0, 0, 0, "@1Protocol    :@8 %s", protocol.ProtocolName);
		insertint3(strng, 48, 0, 0, "@1Probe Distance:@8 %d cm", AmuletWellThyroidUptakeTest.ProbeDistance);
		pr_write3(strng, "Thyroid Uptake Report");

		sprintf(message, "%.1f - %.1f", AmuletWellThyroidUptakeTest.StartROI, AmuletWellThyroidUptakeTest.EndROI);
		insertstring3(strng, 0, 0, 0, "@1ROI(keV)    :@8 %s", message);
		if(protocol.DoseMeasurementMethod == 1)
			insertconst3(strng, 48, 0, 0, "@1Units Given:@8 1");
		else if(protocol.DoseMeasurementMethod == 4)
			insertfloat3(strng, 48, 0, 0, "@1Factor:@8 %.2f", AmuletWellThyroidUptakeTest.DoseMultiplier);
		else
			insertfloat3(strng, 48, 0, 0, "@1Units Given:@8 %.0f", AmuletWellThyroidUptakeTest.DoseMultiplier);
		pr_write3(strng, "Thyroid Uptake Report");
		
		// Dose Counting Method Decay Corrected or Reference
		if(protocol.DoseDecayCorrect){
			insertconst3(strng, 0, 0, 0, "@1Counting Method :@8 Decay Correct Administered Dose");
		}else{
			insertconst3(strng, 0, 0, 0, "@1Counting Method :@8 Measure Same Reference Dose Before Each Uptake");
		}
		pr_write3(strng, "Thyroid Uptake Report");

		// Dose Measurment Method 1,2,3,4
		switch(protocol.DoseMeasurementMethod){
			case 1:
				insertconst3(strng, 0, 0, 0, "@1Dose Measurement:@8 Dose Measured is Dose Administered");
				break;

			case 2:
				insertconst3(strng, 0, 0, 0, "@1Dose Measurement:@8 Measure Each Dose and Add Activity");
				break;

			case 3:
				insertconst3(strng, 0, 0, 0, "@1Dose Measurement:@8 Measure One Capsule and Multiply by Number Administered");
				break;

			case 4:
				insertconst3(strng, 0, 0, 0, "@1Dose Measurement:@8 Measure Liquid and Multiply by Factor");
				break;
		}
		pr_write3(strng, "Thyroid Uptake Report");

		// Dose Form
		if(protocol.DoseForm == 1)
			insertconst3(strng, 0, 0, 0, "@1Dose Form:@8 Capsule");
		else
			insertconst3(strng, 0, 0, 0, "@1Dose Form:@8 Liquid");

		// PreDose Measurement Y/N
		if(protocol.PreDoseMeasurement)
			insertconst3(strng, 22, 0, 0, "@1Pre-Dose:@8 Yes");
		else
			insertconst3(strng, 22, 0, 0, "@1Pre-Dose:@8 No");

		// Residual Measurement Y/N
		if(protocol.ResidualMeasurement)
			insertconst3(strng, 48, 0, 0, "@1Residual:@8 Yes");
		else
			insertconst3(strng, 48, 0, 0, "@1Residual:@8 No");
		pr_write3(strng, "Thyroid Uptake Report");
		


		if(strlen(AmuletWellThyroidUptakeTest.InactiveReason)>0){
			insertstring3(strng, 0, 0, 0, "@1INACTIVE:@8 %s", AmuletWellThyroidUptakeTest.InactiveReason);
			pr_write3(strng, "Thyroid Uptake Report");
		}

		feed(1,prtype);

		//Summary
		NextMeasurementNumber = DB_NextThyroidUptakeMeasurementNumber(AmuletWellThyroidUptakeTest.ProbeTUTestID);
		if((AmuletWellThyroidUptakeTest.ProbeTUTestPhase >= 5) && (NextMeasurementNumber > 1)){
			insertconst3(strng,0,0,0,"@5Summary@8");
			pr_write3(strng, "Thyroid Uptake Report");
			insertconst3(strng,0,0,0,"@4Hour@8");
			insertconst3(strng,5,0,0,"@4Date/Time@8");
			insertconst3(strng,23,0,0,"@4%Uptake@8");
			insertconst3(strng,35,0,0,"@4Normal Range - %@8");
			pr_write3(strng, "Thyroid Uptake Report");

			for(index=1; index<NextMeasurementNumber; index++){
				patient.ProbeTUPatientMeasurementID = -1;
				patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
				patient.MeasurementNumber = index;
				DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
				if(patient.ProbeTUPatientMeasurementID != -1){
					hours = roundfloat(patient.ElapsedTime);
					GetExtendedTimeInfo(&patient.CreatedOn, timestring);
					insertint3(strng, 2, 1, 3, "%d", hours);
					insertconst3(strng, 5, 0, 0, timestring);
					if(ThyroidUptakeCheckNormal(hours, patient.Uptake, &normal_min, &normal_max, &abnormal)){
						if(abnormal){
							insertfloat3(strng, 31, 1, 0, "%.1f***", patient.Uptake);
						}else{
							insertfloat3(strng, 28, 1, 0, "%.1f", patient.Uptake);
						}
						insertint3(strng,35,0,0,"%d",normal_min);
						insertint3(strng,38,0,0,"- %d",normal_max);
					}else{
						insertfloat3(strng, 28, 1, 0, "%.1f", patient.Uptake);
					}
					pr_write3(strng, "Thyroid Uptake Report");
				}
			}

			flgDetectedValidNormal = FALSE;
			for(index=0; index<4; index++){
				if(ThyroidUptake_normal[index].hour != -1){
					flgDetectedValidNormal = TRUE;
					break;
				}
			}

		}

		feed(1,prtype);

		//switch from 12 point to 8 point font, line spacing to 8lpi
		pcl_height("8",TRUE);
		pcl_line_spacing("8",TRUE);

		strcpy(message,"Avg ");
		strcat(message,ustr);
		insertstring3(strng,46,0,0,"@5%s@8",message);
		insertconst3(strng,58,0,0,"@5Count1@8");
		insertconst3(strng,70,0,0,"@5Count2@8");
		pr_write3(strng,"Thyroid Uptake report");

		// Pre-Dose
		if(protocol.PreDoseMeasurement && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 1){
			patient.ProbeTUPatientMeasurementID = -1;
			patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
			patient.MeasurementNumber = 0;
			DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
			if(patient.ProbeTUPatientMeasurementID != -1){
				GetExtendedTimeInfo(&patient.CreatedOn, timestring);
				insertconst3(strng, 0, 0, 0, "@5Pre-Dose@8");
				insertstring3(strng, 20, 0, 0, "%s", timestring);
				pr_write3(strng, "Thyroid Uptake Report");
				if(patient.Uptake == -1){
					insertconst3(strng, 3, 0, 0, "SKIPPED");
					pr_write3(strng, "Thyroid Uptake Report");
				}else{
					cpm_cps = patient.LegROIAverageCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					insertconst3(strng, 3, 0, 0, "Patient Background:");
					insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

					cpm_cps = patient.Leg1ROICPM;
					if(current.system != CI) cpm_cps /= 60.0;
					insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

					if(patient.Leg2_SpectraID != -1){
						cpm_cps = patient.Leg2ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
					}
					pr_write3(strng, "Thyroid Uptake Report");

					cpm_cps = patient.NeckROIAverageCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					insertconst3(strng, 3, 0, 0, "Patient Thyroid:");
					insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

					cpm_cps = patient.Neck1ROICPM;
					if(current.system != CI) cpm_cps /= 60.0;
					insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

					if(patient.Neck2_SpectraID != -1){
						cpm_cps = patient.Neck2ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
					}
					pr_write3(strng, "Thyroid Uptake Report");

					cpm_cps = patient.PatientROINetCPM;
					if(current.system != CI) cpm_cps /= 60.0;
					insertconst3(strng, 3, 0, 0, "Patient Net Counts:");
					insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
					pr_write3(strng, "Thyroid Uptake Report");

					if(strlen(patient.comment) != 0){
						insertstring3(strng, 3, 0, 0, "Comment: %s", patient.comment);
						pr_write3(strng, "Thyroid Uptake Report");
					}
				}
			}
			feed(1,prtype);
		}
		

		// Admin Dose
		if(protocol.DoseDecayCorrect && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 2){
			dose.ProbeTUTotalDoseMeasurementID = -1;
			dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
			dose.MeasurementNumber = 0;
			DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
			if(dose.ProbeTUTotalDoseMeasurementID != -1){
				GetExtendedTimeInfo(&dose.CreatedOn, timestring);
				insertconst3(strng, 0, 0, 0, "@5Administered Dose@8");
				insertstring3(strng, 20, 0, 0, "%s", timestring);
				pr_write3(strng, "Thyroid Uptake Report");
				

				background.WellBackgroundID = dose.WellBackgroundID;
				DB_RetrieveBackgnd(&background);
				GetExtendedTimeInfo(&background.CreatedOn, timestring);
				insertconst3(strng, 3, 0, 0, "Room Background:");
				insertstring3(strng, 20, 0, 0, "%s", timestring);

				cpm_cps = dose.BackgroundROICPM;
				if(current.system != CI) cpm_cps /= 60.0;
				insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
				pr_write3(strng, "Thyroid Uptake Report");

				
				switch(protocol.DoseMeasurementMethod){
					case 1:
						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
						dose.SingleMeasurement[0].DoseSubUnit = 1;
						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
							insertconst3(strng, 3, 0, 0, "Dose #1:");
							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);

							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
							}

							pr_write3(strng, "Thyroid Uptake Report");

							cpm_cps = dose.DoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertconst3(strng, 3, 0, 0, "Net Dose:");
							insertstring3(strng, 20, 0, 0, "%s", timestring);
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
							pr_write3(strng, "Thyroid Uptake Report");
						}
						break;

					case 2:
						for(index=0; index < 5; index++){
							dose.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
							dose.SingleMeasurement[index].DoseSubUnit = index + 1;
							DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[index]));

							if(dose.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID != -1){
								insertint3(strng, 3, 0, 0, "Dose #%d", index + 1);
								GetExtendedTimeInfo(&(dose.SingleMeasurement[index].CreatedOn), timestring);
								insertstring3(strng, 20, 0, 0, "%s", timestring);
								cpm_cps = dose.SingleMeasurement[index].DoseROIAverageCPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

								cpm_cps = dose.SingleMeasurement[index].Dose1ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

								if(dose.SingleMeasurement[index].Dose2_SpectraID != -1){
									cpm_cps = dose.SingleMeasurement[index].Dose2ROICPM;
									if(current.system != CI) cpm_cps /= 60.0;
									insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
								}

								pr_write3(strng, "Thyroid Uptake Report");
							}
						}

						cpm_cps = dose.DoseROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Total Dose");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
						pr_write3(strng, "Thyroid Uptake Report");

						cpm_cps = dose.DoseROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Net Dose");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
						pr_write3(strng, "Thyroid Uptake Report");
						break;

					case 3:
					case 4:
						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
						dose.SingleMeasurement[0].DoseSubUnit = 1;
						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
							insertconst3(strng, 3, 0, 0, "Dose #1");
							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
							insertstring3(strng, 20, 0, 0, "%s", timestring);

							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
							}
							pr_write3(strng, "Thyroid Uptake Report");
						}

						cpm_cps = dose.DoseROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Total Dose");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
						pr_write3(strng, "Thyroid Uptake Report");

						cpm_cps = dose.DoseROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Net Dose");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
						pr_write3(strng, "Thyroid Uptake Report");
						break;
				}
			}
		}

		// Residual
		if((protocol.ResidualMeasurement) && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 4){
			dose.ProbeTUTotalDoseMeasurementID = -1;
			dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
			dose.MeasurementNumber = -1;
			DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
			if(dose.ProbeTUTotalDoseMeasurementID != -1){
				feed(1,prtype);
				GetExtendedTimeInfo(&dose.CreatedOn, timestring);
				insertconst3(strng, 0, 0, 0, "@5Residual@8");
				insertstring3(strng, 20, 0, 0, "%s", timestring);
				pr_write3(strng, "Thyroid Uptake Report");

				background.WellBackgroundID = dose.WellBackgroundID;
				DB_RetrieveBackgnd(&background);

				cpm_cps = dose.BackgroundROICPM;
				if(current.system != CI) cpm_cps /= 60.0;
				insertconst3(strng, 3, 0, 0, "Room Background:");
				GetExtendedTimeInfo(&background.CreatedOn, timestring);
				insertstring3(strng, 20, 0, 0, "%s", timestring);
				insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
				pr_write3(strng, "Thyroid Uptake Report");


				switch(protocol.DoseMeasurementMethod){
					case 1:
					case 3:
					case 4:
						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
						dose.SingleMeasurement[0].DoseSubUnit = 1;
						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));
						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
							insertconst3(strng, 3, 0, 0, "Residual:");
							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
							insertstring3(strng, 20, 0, 0, "%s", timestring);

							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
							}

							pr_write3(strng, "Thyroid Uptake Report");

							cpm_cps = dose.DoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertconst3(strng, 3, 0, 0, "Net Residual:");
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
							pr_write3(strng, "Thyroid Uptake Report");
						}
						break;

					case 2:
						for(index=0; index < 5; index++){
							dose.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
							dose.SingleMeasurement[index].DoseSubUnit = index + 1;
							DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[index]));

							if(dose.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID != -1){
								insertint3(strng, 3, 0, 0, "Residual #%d", index + 1);
								GetExtendedTimeInfo(&(dose.SingleMeasurement[index].CreatedOn), timestring);
								insertstring3(strng, 20, 0, 0, "%s", timestring);

								cpm_cps = dose.SingleMeasurement[index].DoseROIAverageCPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

								cpm_cps = dose.SingleMeasurement[index].Dose1ROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

								if(dose.SingleMeasurement[index].Dose2_SpectraID != -1){
									cpm_cps = dose.SingleMeasurement[index].Dose2ROICPM;
									if(current.system != CI) cpm_cps /= 60.0;
									insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
								}

								pr_write3(strng, "Thyroid Uptake Report");
							}
						}

						cpm_cps = dose.DoseROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Total Residual");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
						pr_write3(strng, "Thyroid Uptake Report");

						cpm_cps = dose.DoseROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Net Residual");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
						pr_write3(strng, "Thyroid Uptake Report");
						break;
				}
			}
		}
		
		// Measurement
		if(AmuletWellThyroidUptakeTest.ProbeTUTestPhase >= 5){
			NextMeasurementNumber = DB_NextThyroidUptakeMeasurementNumber(AmuletWellThyroidUptakeTest.ProbeTUTestID);
			if(NextMeasurementNumber > 1){
				for(index=1; index<NextMeasurementNumber; index++){
					patient.ProbeTUPatientMeasurementID = -1;
					patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
					patient.MeasurementNumber = index;
					DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
					if(patient.ProbeTUPatientMeasurementID != -1){
						feed(1,prtype);
						GetExtendedTimeInfo(&patient.CreatedOn, timestring);
						hours = roundfloat(patient.ElapsedTime);
						insertint3(strng, 0, 0, 0, "@5Uptake At %d Hours:@8", hours);
						insertstring3(strng, 20, 0, 0, "%s", timestring);
						pr_write3(strng, "Thyroid Uptake Report");

						if(protocol.DoseDecayCorrect){
							cpm_cps = patient.DoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertconst3(strng, 3, 0, 0, "Decayed Administered Dose:");
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
							pr_write3(strng, "Thyroid Uptake Report");
						}else{
							dose.ProbeTUTotalDoseMeasurementID = -1;
							dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
							dose.MeasurementNumber = index;
							DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
							if(dose.ProbeTUTotalDoseMeasurementID != -1){
								background.WellBackgroundID = dose.WellBackgroundID;
								DB_RetrieveBackgnd(&background);

								cpm_cps = dose.BackgroundROICPM;
								if(current.system != CI) cpm_cps /= 60.0;
								insertconst3(strng, 3, 0, 0, "Dose Background");
								insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

								GetExtendedTimeInfo(&background.CreatedOn, timestring);
								insertstring3(strng, 20, 0, 0, "%s", timestring);
								pr_write3(strng, "Thyroid Uptake Report");

								switch(protocol.DoseMeasurementMethod){
									case 1:
										dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
										dose.SingleMeasurement[0].DoseSubUnit = 1;
										DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

										if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
											insertconst3(strng, 3, 0, 0, "Dose #1");
											GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
											insertstring(strng, 20, 0, 0, "%s", timestring);

											cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
											if(current.system != CI) cpm_cps /= 60.0;
											insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

											cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
											if(current.system != CI) cpm_cps /= 60.0;
											insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

											if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
												cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
												if(current.system != CI) cpm_cps /= 60.0;
												insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
											}
											pr_write3(strng, "Thyroid Uptake Report");

											cpm_cps = patient.DoseROINetCPM;
											if(current.system != CI) cpm_cps /= 60.0;
											insertconst3(strng, 3, 0, 0, "Net Dose");
											insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
											pr_write3(strng, "Thyroid Uptake Report");
										}
										break;

									case 2:
										for(jndex=0; jndex < 5; jndex++){
											dose.SingleMeasurement[jndex].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
											dose.SingleMeasurement[jndex].DoseSubUnit = jndex + 1;
											DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[jndex]));

											if(dose.SingleMeasurement[jndex].ProbeTUSingleDoseMeasurementID != -1){
												insertint3(strng, 3, 0, 0, "Dose #%d", jndex + 1);
												GetExtendedTimeInfo(&(dose.SingleMeasurement[jndex].CreatedOn), timestring);
												insertstring3(strng, 20, 0, 0, "%s", timestring);

												cpm_cps = dose.SingleMeasurement[jndex].DoseROIAverageCPM;
												if(current.system != CI) cpm_cps /= 60.0;
												insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

												cpm_cps = dose.SingleMeasurement[jndex].Dose1ROICPM;
												if(current.system != CI) cpm_cps /= 60.0;
												insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

												if(dose.SingleMeasurement[jndex].Dose2_SpectraID != -1){
													cpm_cps = dose.SingleMeasurement[jndex].Dose2ROICPM;
													if(current.system != CI) cpm_cps /= 60.0;
													insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
												}
												pr_write3(strng, "Thyroid Uptake Report");
											}
										}

										cpm_cps = dose.DoseROICPM;
										if(current.system != CI) cpm_cps /= 60.0;
										insertconst3(strng, 3, 0, 0, "Total Dose");
										insertfloat3(strng,46, 0, 0, "%.0f", cpm_cps);
										pr_write3(strng, "Thyroid Uptake Report");

										cpm_cps = patient.DoseROINetCPM;
										if(current.system != CI) cpm_cps /= 60.0;
										insertconst3(strng, 3, 0, 0, "  Net Dose");
										insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
										pr_write3(strng, "Thyroid Uptake Report");
										break;

									case 3:
									case 4:
										dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
										dose.SingleMeasurement[0].DoseSubUnit = 1;
										DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));

										if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
											insertconst3(strng, 3, 0, 0, "Dose #1");
											GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
											insertstring3(strng, 20, 0, 0, "%s", timestring);

											cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
											if(current.system != CI) cpm_cps /= 60.0;
											insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

											cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
											if(current.system != CI) cpm_cps /= 60.0;
											insertfloat(strng, 58, 0, 0, "%.0f", cpm_cps);
											
											if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
												cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
												if(current.system != CI) cpm_cps /= 60.0;
												insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
											}
											pr_write3(strng, "Thyroid Uptake Report");
										}
										
										cpm_cps = dose.DoseROICPM;
										if(current.system != CI) cpm_cps /= 60.0;
										insertconst3(strng, 3, 0, 0, "Total Dose");
										insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
										pr_write3(strng, "Thyroid Uptake Report");

										cpm_cps = patient.DoseROINetCPM;
										if(current.system != CI) cpm_cps /= 60.0;
										insertconst3(strng, 3, 0, 0, "Net Dose");
										insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
										pr_write3(strng, "Thyroid Uptake Report");
										break;
								}
							}
						}

						if(protocol.ResidualMeasurement){
							cpm_cps = patient.AdjustedDoseROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertconst3(strng, 3, 0, 0, "Residual Adjusted:");
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
							pr_write3(strng, "Thyroid Uptake Report");
						}


						cpm_cps = patient.LegROIAverageCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Patient Background:");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

						cpm_cps = patient.Leg1ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertfloat(strng, 58, 0, 0, "%.0f", cpm_cps);

						if(patient.Leg2_SpectraID != -1){
							cpm_cps = patient.Leg2ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
						}
						pr_write3(strng, "Thyroid Uptake Report");

						cpm_cps = patient.NeckROIAverageCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Patient Thyroid:");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);

						cpm_cps = patient.Neck1ROICPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertfloat3(strng, 58, 0, 0, "%.0f", cpm_cps);

						if(patient.Neck2_SpectraID != -1){
							cpm_cps = patient.Neck2ROICPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertfloat3(strng, 70, 0, 0, "%.0f", cpm_cps);
						}
						pr_write3(strng, "Thyroid Uptake Report");

						cpm_cps = patient.PatientROINetCPM;
						if(current.system != CI) cpm_cps /= 60.0;
						insertconst3(strng, 3, 0, 0, "Patient Net Counts:");
						insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
						pr_write3(strng, "Thyroid Uptake Report");

						if(protocol.PreDoseMeasurement){
							cpm_cps = patient.AdjustedPatientROINetCPM;
							if(current.system != CI) cpm_cps /= 60.0;
							insertconst3(strng, 3, 0, 0, "PreDose Adjusted:");
							insertfloat3(strng, 46, 0, 0, "%.0f", cpm_cps);
							pr_write3(strng, "Thyroid Uptake Report");
						}


						if(strlen(patient.comment) != 0){
							insertstring3(strng, 3, 0, 0, "Comment: %s", patient.comment);
							pr_write3(strng, "Thyroid Uptake Report");
						}

						if(ThyroidUptakeCheckNormal(hours, patient.Uptake, &normal_min, &normal_max, &abnormal)){
							if(abnormal){
								insertfloat3(strng, 3, 0, 0, "@1Uptake = %.1f%% (Outside Normal)@8", patient.Uptake);
							}else{
								insertfloat3(strng, 3, 0, 0, "@1Uptake = %.1f%%@8", patient.Uptake);
							}
						}else{
							insertfloat3(strng, 3, 0, 0, "@1Uptake = %.1f %%@8", patient.Uptake);
						}
						pr_write3(strng, "Thyroid Uptake Report");
					}
				}
			}

			if(flgDetectedValidNormal)
			{
				feed(2,prtype);
				insertconst3(strng,0,0,0,"@1Normal Range@8");
				pr_write3(strng,"Thyroid Uptake Report");

				insertconst3(strng, 2, 0, 0, "@4 Hour      Min %      Max %@8");
				pr_write3(strng,"Thyroid Uptake Report");

				for(index=0; index<4; index++){
					if(ThyroidUptake_normal[index].hour != -1){
						insertint3(strng, 3, 1, 0, "%d",  ThyroidUptake_normal[index].hour);
						insertint3(strng, 13, 1, 0, "%d",  ThyroidUptake_normal[index].min);
						insertint3(strng, 24, 1, 0, "%d",  ThyroidUptake_normal[index].max);
						pr_write3(strng,"Thyroid Uptake Report");
						
					}
				}
			}	
		}
		



/*		// Pre-Dose
 - 		if(protocol.PreDoseMeasurement && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 1){
 - 			patient.ProbeTUPatientMeasurementID = -1;
 - 			patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
 - 			patient.MeasurementNumber = 0;
 - 			DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
 - 			if(patient.ProbeTUPatientMeasurementID != -1){
 - 				insertconst(strng, 0, 0, 0, "==================================");
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 				GetExtendedTimeInfo(&patient.CreatedOn, timestring);
 - 				insertconst(strng, 0, 0, 0, "Pre-Dose");
 - 				insertstring(strng, 32, 1, 0, "%s", timestring);
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 				if(patient.Uptake == -1){
 - 					insertconst(strng, 0, 0, 0, "     SKIPPED");
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 				}else{
 - 					cpm_cps = patient.LegROIAverageCPM;
 - 					if(current.system != CI) cpm_cps /= 60.0;
 - 					if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background Avg(cpm): %.0f", cpm_cps);
 - 					else  insertfloat(strng, 0, 0, 0, "Background Avg(cps): %.0f", cpm_cps);
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 					cpm_cps = patient.Leg1ROICPM;
 - 					if(current.system != CI) cpm_cps /= 60.0;
 - 					if(current.system == CI ) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
 - 					else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 					if(patient.Leg2_SpectraID != -1){
 - 						cpm_cps = patient.Leg2ROICPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 					}
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 					cpm_cps = patient.NeckROIAverageCPM;
 - 					if(current.system != CI) cpm_cps /= 60.0;
 - 					if(current.system == CI) insertfloat(strng, 0, 0, 0, "      Neck Avg(cpm): %.0f", cpm_cps);
 - 					else insertfloat(strng, 0, 0, 0, "      Neck Avg(cps): %.0f", cpm_cps);
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 					cpm_cps = patient.Neck1ROICPM;
 - 					if(current.system != CI) cpm_cps /= 60.0;
 - 					if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
 - 					else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 					if(patient.Neck2_SpectraID != -1){
 - 						cpm_cps = patient.Neck2ROICPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 					}
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 					cpm_cps = patient.PatientROINetCPM;
 - 					if(current.system != CI) cpm_cps /= 60.0;
 - 					if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Net Counts(cpm): %.0f", cpm_cps);
 - 					else insertfloat(strng, 0, 0, 0, "    Net Counts(cps): %.0f", cpm_cps);
 - 					pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 					if(strlen(patient.comment) != 0){
 - 						insertstring(strng, 0, 0, 0, "Comment: %s", patient.comment);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 					}
 - 				}
 - 			}
 - 		}*/


/*		// Admin Dose
 - 		if(protocol.DoseDecayCorrect && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 2){
 - 			dose.ProbeTUTotalDoseMeasurementID = -1;
 - 			dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
 - 			dose.MeasurementNumber = 0;
 - 			DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
 - 			if(dose.ProbeTUTotalDoseMeasurementID != -1){
 - 				insertconst(strng, 0, 0, 0, "==================================");
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 				GetExtendedTimeInfo(&dose.CreatedOn, timestring);
 - 				insertconst(strng, 0, 0, 0, "Admin Dose");
 - 				insertstring(strng, 32, 1, 0, "%s", timestring);
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				background.WellBackgroundID = dose.WellBackgroundID;
 - 				DB_RetrieveBackgnd(&background);
 - 
 - 				cpm_cps = dose.BackgroundROICPM;
 - 				if(current.system != CI) cpm_cps /= 60.0;
 - 				if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background(cpm): %.0f", cpm_cps);
 - 				else insertfloat(strng, 0, 0, 0, "Background(cps): %.0f", cpm_cps);
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				GetExtendedTimeInfo(&background.CreatedOn, timestring);
 - 				insertstring(strng, 0, 0, 0, "     %s", timestring);
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				switch(protocol.DoseMeasurementMethod){
 - 					case 1:
 - 						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 						dose.SingleMeasurement[0].DoseSubUnit = 1;
 - 						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));
 - 
 - 						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
 - 							insertconst(strng, 0, 0, 0, "Dose #1");
 - 							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
 - 							insertstring(strng, 32, 1, 0, "%s", timestring);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
 - 								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 							}
 - 
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.DoseROINetCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Dose(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "Net Dose(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 						break;
 - 
 - 					case 2:
 - 						for(index=0; index < 5; index++){
 - 							dose.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 							dose.SingleMeasurement[index].DoseSubUnit = index + 1;
 - 							DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[index]));
 - 
 - 							if(dose.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID != -1){
 - 								insertint(strng, 0, 0, 0, "Dose #%d", index + 1);
 - 								GetExtendedTimeInfo(&(dose.SingleMeasurement[index].CreatedOn), timestring);
 - 								insertstring(strng, 32, 1, 0, "%s", timestring);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								cpm_cps = dose.SingleMeasurement[index].DoseROIAverageCPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								cpm_cps = dose.SingleMeasurement[index].Dose1ROICPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								if(dose.SingleMeasurement[index].Dose2_SpectraID != -1){
 - 									cpm_cps = dose.SingleMeasurement[index].Dose2ROICPM;
 - 									if(current.system != CI) cpm_cps /= 60.0;
 - 									if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 									else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 									pr_write2(strng, "Thyroid Uptake Report");
 - 								}
 - 
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 							}
 - 						}
 - 
 - 						cpm_cps = dose.DoseROICPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						cpm_cps = dose.DoseROINetCPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 						break;
 - 
 - 					case 3:
 - 					case 4:
 - 						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 						dose.SingleMeasurement[0].DoseSubUnit = 1;
 - 						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));
 - 
 - 						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
 - 							insertconst(strng, 0, 0, 0, "Dose #1");
 - 							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
 - 							insertstring(strng, 32, 1, 0, "%s", timestring);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
 - 								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 							}
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 
 - 						cpm_cps = dose.DoseROICPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						cpm_cps = dose.DoseROINetCPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 						break;
 - 				}
 - 			}
 - 		}*/

/*		// Residual
 - 		if((protocol.ResidualMeasurement) && AmuletWellThyroidUptakeTest.ProbeTUTestPhase > 4){
 - 			dose.ProbeTUTotalDoseMeasurementID = -1;
 - 			dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
 - 			dose.MeasurementNumber = -1;
 - 			DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
 - 			if(dose.ProbeTUTotalDoseMeasurementID != -1){
 - 				insertconst(strng, 0, 0, 0, "==================================");
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 				GetExtendedTimeInfo(&dose.CreatedOn, timestring);
 - 				insertconst(strng, 0, 0, 0, "Residual");
 - 				insertstring(strng, 32, 1, 0, "%s", timestring);
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				background.WellBackgroundID = dose.WellBackgroundID;
 - 				DB_RetrieveBackgnd(&background);
 - 
 - 				cpm_cps = dose.BackgroundROICPM;
 - 				if(current.system != CI) cpm_cps /= 60.0;
 - 				if(current.system == CI) insertfloat(strng, 0, 0, 0, "Background(cpm): %.0f", cpm_cps);
 - 				else insertfloat(strng, 0, 0, 0, "Background(cps): %.0f", cpm_cps);
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				GetExtendedTimeInfo(&background.CreatedOn, timestring);
 - 				insertstring(strng, 0, 0, 0, "     %s", timestring);
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 				switch(protocol.DoseMeasurementMethod){
 - 					case 1:
 - 					case 3:
 - 					case 4:
 - 						dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 						dose.SingleMeasurement[0].DoseSubUnit = 1;
 - 						DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));
 - 						if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
 - 							insertconst(strng, 0, 0, 0, "Residual");
 - 							GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
 - 							insertstring(strng, 32, 1, 0, "%s", timestring);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
 - 								cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 							}
 - 
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 							cpm_cps = dose.DoseROINetCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Residual(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "Net Residual(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 						break;
 - 
 - 					case 2:
 - 						for(index=0; index < 5; index++){
 - 							dose.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 							dose.SingleMeasurement[index].DoseSubUnit = index + 1;
 - 							DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[index]));
 - 
 - 							if(dose.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID != -1){
 - 								insertint(strng, 0, 0, 0, "Residual #%d", index + 1);
 - 								GetExtendedTimeInfo(&(dose.SingleMeasurement[index].CreatedOn), timestring);
 - 								insertstring(strng, 32, 1, 0, "%s", timestring);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								cpm_cps = dose.SingleMeasurement[index].DoseROIAverageCPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								cpm_cps = dose.SingleMeasurement[index].Dose1ROICPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								if(dose.SingleMeasurement[index].Dose2_SpectraID != -1){
 - 									cpm_cps = dose.SingleMeasurement[index].Dose2ROICPM;
 - 									if(current.system != CI) cpm_cps /= 60.0;
 - 									if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 									else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 									pr_write2(strng, "Thyroid Uptake Report");
 - 								}
 - 
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 							}
 - 						}
 - 
 - 						cpm_cps = dose.DoseROICPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Residual(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "Total Residual(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						cpm_cps = dose.DoseROINetCPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Residual(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "  Net Residual(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 						break;
 - 				}
 - 			}
 - 		}*/

/*		// Measurement
 - 		if(AmuletWellThyroidUptakeTest.ProbeTUTestPhase >= 5){
 - 			NextMeasurementNumber = DB_NextThyroidUptakeMeasurementNumber(AmuletWellThyroidUptakeTest.ProbeTUTestID);
 - 			if(NextMeasurementNumber > 1){
 - 				for(index=1; index<NextMeasurementNumber; index++){
 - 					patient.ProbeTUPatientMeasurementID = -1;
 - 					patient.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
 - 					patient.MeasurementNumber = index;
 - 					DB_RetrieveProbeThyroidUptakePatientMeasurement(&patient);
 - 					if(patient.ProbeTUPatientMeasurementID != -1){
 - 						insertconst(strng, 0, 0, 0, "==================================");
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 						GetExtendedTimeInfo(&patient.CreatedOn, timestring);
 - 						hours = roundfloat(patient.ElapsedTime);
 - 						insertint(strng, 0, 0, 0, "Uptake: %d Hrs", hours);
 - 						insertstring(strng, 32, 1, 0, "%s", timestring);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						if(protocol.DoseDecayCorrect){
 - 							cpm_cps = patient.DoseROINetCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Decayed Admin Dose(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "Decayed Admin Dose(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}else{
 - 							dose.ProbeTUTotalDoseMeasurementID = -1;
 - 							dose.ProbeTUTestID = AmuletWellThyroidUptakeTest.ProbeTUTestID;
 - 							dose.MeasurementNumber = index;
 - 							DB_RetrieveProbeThyroidUptakeDoseMeasurement(&dose);
 - 							if(dose.ProbeTUTotalDoseMeasurementID != -1){
 - 								background.WellBackgroundID = dose.WellBackgroundID;
 - 								DB_RetrieveBackgnd(&background);
 - 
 - 								cpm_cps = dose.BackgroundROICPM;
 - 								if(current.system != CI) cpm_cps /= 60.0;
 - 								if(current.system == CI) insertfloat(strng, 0, 0, 0, "Dose Background(cpm): %.0f", cpm_cps);
 - 								else insertfloat(strng, 0, 0, 0, "Dose Background(cps): %.0f", cpm_cps);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								GetExtendedTimeInfo(&background.CreatedOn, timestring);
 - 								insertstring(strng, 0, 0, 0, "     %s", timestring);
 - 								pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 								switch(protocol.DoseMeasurementMethod){
 - 									case 1:
 - 										dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 										dose.SingleMeasurement[0].DoseSubUnit = 1;
 - 										DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));
 - 
 - 										if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
 - 											insertconst(strng, 0, 0, 0, "Dose #1");
 - 											GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
 - 											insertstring(strng, 32, 1, 0, "%s", timestring);
 - 											pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 											cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
 - 											if(current.system != CI) cpm_cps /= 60.0;
 - 											if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 											else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 											pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 											cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
 - 											if(current.system != CI) cpm_cps /= 60.0;
 - 											if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 											else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 											pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 											if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
 - 												cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
 - 												if(current.system != CI) cpm_cps /= 60.0;
 - 												if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 												else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 												pr_write2(strng, "Thyroid Uptake Report");
 - 											}
 - 
 - 											cpm_cps = patient.DoseROINetCPM;
 - 											if(current.system != CI) cpm_cps /= 60.0;
 - 											if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Dose(cpm): %.0f", cpm_cps);
 - 											else insertfloat(strng, 0, 0, 0, "Net Dose(cps): %.0f", cpm_cps);
 - 											pr_write2(strng, "Thyroid Uptake Report");
 - 										}
 - 										break;
 - 
 - 									case 2:
 - 										for(jndex=0; jndex < 5; jndex++){
 - 											dose.SingleMeasurement[jndex].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 											dose.SingleMeasurement[jndex].DoseSubUnit = jndex + 1;
 - 											DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[jndex]));
 - 
 - 											if(dose.SingleMeasurement[jndex].ProbeTUSingleDoseMeasurementID != -1){
 - 												insertint(strng, 0, 0, 0, "Dose #%d", jndex + 1);
 - 												GetExtendedTimeInfo(&(dose.SingleMeasurement[jndex].CreatedOn), timestring);
 - 												insertstring(strng, 32, 1, 0, "%s", timestring);
 - 												pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 												cpm_cps = dose.SingleMeasurement[jndex].DoseROIAverageCPM;
 - 												if(current.system != CI) cpm_cps /= 60.0;
 - 												if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 												else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 												pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 												cpm_cps = dose.SingleMeasurement[jndex].Dose1ROICPM;
 - 												if(current.system != CI) cpm_cps /= 60.0;
 - 												if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 												else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 												pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 												if(dose.SingleMeasurement[jndex].Dose2_SpectraID != -1){
 - 													cpm_cps = dose.SingleMeasurement[jndex].Dose2ROICPM;
 - 													if(current.system != CI) cpm_cps /= 60.0;
 - 													if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 													else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 													pr_write2(strng, "Thyroid Uptake Report");
 - 												}
 - 											}
 - 										}
 - 
 - 										cpm_cps = dose.DoseROICPM;
 - 										if(current.system != CI) cpm_cps /= 60.0;
 - 										if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
 - 										else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
 - 										pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 										cpm_cps = patient.DoseROINetCPM;
 - 										if(current.system != CI) cpm_cps /= 60.0;
 - 										if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
 - 										else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
 - 										pr_write2(strng, "Thyroid Uptake Report");
 - 										break;
 - 
 - 									case 3:
 - 									case 4:
 - 										dose.SingleMeasurement[0].ProbeTUTotalDoseMeasurementID = dose.ProbeTUTotalDoseMeasurementID;
 - 										dose.SingleMeasurement[0].DoseSubUnit = 1;
 - 										DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(dose.SingleMeasurement[0]));
 - 
 - 										if(dose.SingleMeasurement[0].ProbeTUSingleDoseMeasurementID != -1){
 - 											insertconst(strng, 0, 0, 0, "Dose #1");
 - 											GetExtendedTimeInfo(&(dose.SingleMeasurement[0].CreatedOn), timestring);
 - 											insertstring(strng, 32, 1, 0, "%s", timestring);
 - 											pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 											cpm_cps = dose.SingleMeasurement[0].DoseROIAverageCPM;
 - 											if(current.system != CI) cpm_cps /= 60.0;
 - 											if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Average(cpm): %.0f", cpm_cps);
 - 											else insertfloat(strng, 0, 0, 0, "   Average(cps): %.0f", cpm_cps);
 - 											pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 											cpm_cps = dose.SingleMeasurement[0].Dose1ROICPM;
 - 											if(current.system != CI) cpm_cps /= 60.0;
 - 											if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count1(cpm)= %.0f", cpm_cps);
 - 											else insertfloat(strng, 0, 0, 0, "    Count1(cps)= %.0f", cpm_cps);
 - 											pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 											if(dose.SingleMeasurement[0].Dose2_SpectraID != -1){
 - 												cpm_cps = dose.SingleMeasurement[0].Dose2ROICPM;
 - 												if(current.system != CI) cpm_cps /= 60.0;
 - 												if(current.system == CI) insertfloat(strng, 0, 0, 0, "    Count2(cpm)= %.0f", cpm_cps);
 - 												else insertfloat(strng, 0, 0, 0, "    Count2(cps)= %.0f", cpm_cps);
 - 												pr_write2(strng, "Thyroid Uptake Report");
 - 											}
 - 										}
 - 
 - 										cpm_cps = dose.DoseROICPM;
 - 										if(current.system != CI) cpm_cps /= 60.0;
 - 										if(current.system == CI) insertfloat(strng, 0, 0, 0, "Total Dose(cpm): %.0f", cpm_cps);
 - 										else insertfloat(strng, 0, 0, 0, "Total Dose(cps): %.0f", cpm_cps);
 - 										pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 										cpm_cps = patient.DoseROINetCPM;
 - 										if(current.system != CI) cpm_cps /= 60.0;
 - 										if(current.system == CI) insertfloat(strng, 0, 0, 0, "  Net Dose(cpm): %.0f", cpm_cps);
 - 										else insertfloat(strng, 0, 0, 0, "  Net Dose(cps): %.0f", cpm_cps);
 - 										pr_write2(strng, "Thyroid Uptake Report");
 - 										break;
 - 								}
 - 							}
 - 						}
 - 
 - 						if(protocol.ResidualMeasurement){
 - 							cpm_cps = patient.AdjustedDoseROINetCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "Residual Adjusted(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "Residual Adjusted(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						cpm_cps = patient.LegROIAverageCPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Patient Backgnd Avg(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "Patient Backgnd Avg(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						cpm_cps = patient.Leg1ROICPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI ) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						if(patient.Leg2_SpectraID != -1){
 - 							cpm_cps = patient.Leg2ROICPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 
 - 						cpm_cps = patient.NeckROIAverageCPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Neck Avg(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "Neck Avg(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						cpm_cps = patient.Neck1ROICPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count1(cpm)= %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "   Count1(cps)= %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						if(patient.Neck2_SpectraID != -1){
 - 							cpm_cps = patient.Neck2ROICPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "   Count2(cpm)= %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "   Count2(cps)= %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 
 - 						cpm_cps = patient.PatientROINetCPM;
 - 						if(current.system != CI) cpm_cps /= 60.0;
 - 						if(current.system == CI) insertfloat(strng, 0, 0, 0, "Net Counts(cpm): %.0f", cpm_cps);
 - 						else insertfloat(strng, 0, 0, 0, "Net Counts(cps): %.0f", cpm_cps);
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						if(protocol.PreDoseMeasurement){
 - 							cpm_cps = patient.AdjustedPatientROINetCPM;
 - 							if(current.system != CI) cpm_cps /= 60.0;
 - 							if(current.system == CI) insertfloat(strng, 0, 0, 0, "PreDose Adjusted(cpm): %.0f", cpm_cps);
 - 							else insertfloat(strng, 0, 0, 0, "PreDose Adjusted(cps): %.0f", cpm_cps);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 
 - 						if(strlen(patient.comment) != 0){
 - 							insertstring(strng, 0, 0, 0, "Comment: %s", patient.comment);
 - 							pr_write2(strng, "Thyroid Uptake Report");
 - 						}
 - 
 - 						if(ThyroidUptakeCheckNormal(hours, patient.Uptake, &normal_min, &normal_max, &abnormal)){
 - 							if(abnormal){
 - 								insertfloat(strng, 0, 0, 0, "%%Uptake: %.1f%% (Outside Normal)", patient.Uptake);
 - 							}else{
 - 								insertfloat(strng, 0, 0, 0, "%%Uptake: %.1f%%", patient.Uptake);
 - 							}
 - 						}else{
 - 							insertfloat(strng, 0, 0, 0, "%%Uptake: %.1f%%", patient.Uptake);
 - 						}
 - 						pr_write2(strng, "Thyroid Uptake Report");
 - 					}
 - 				}
 - 			}
 - 		}*/

		print_page_num();

	}
}

int CalcAge(double julian_date){
	double julian;
	DateTime datetime;
	struct tm tmt;
	int DOBMonth, DOBDay, DOBYear, CURMonth, CURDay, CURYear, Age;

	julian = julian_date;
	julian *= 86400000.0;
	datetime.iJD = julian;
	datetime.Y = 0;
	datetime.M = 0;
	datetime.D = 0;
	datetime.h = 0;
	datetime.m = 0;
	datetime.s = 0;
	datetime.validJD = 1;
	datetime.validYMD = 0;
	datetime.validHMS = 0;
	datetime.validTZ = 0;
	computeYMD_HMS(&datetime);

	DOBMonth = datetime.M;
	DOBDay = datetime.D;
	DOBYear = datetime.Y;

	memcpy(&tmt,gmtime(&clock_time),sizeof(tmt));
	CURMonth = tmt.tm_mon;
	CURMonth++;
	CURDay = tmt.tm_mday;
	CURYear = tmt.tm_year;
	CURYear += 1900;

	Age = CURYear - DOBYear;
	if(CURMonth < DOBMonth) Age--;
	else if(CURMonth == DOBMonth){
		if(CURDay < DOBDay) Age--;
	}

	return Age;
}
