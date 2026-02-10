#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_PRE_INIT	0
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT		1
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_SAVE		2
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_BACKSPEC	3
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S11			4
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S12			5
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S21			6
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S22			7
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S31			8
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S32			9
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S41			10
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S42			11
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S51			12
#define PHASE_WELLTHYROIDUPTAKEMEASDOSE_S52			13

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "sqlite3.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasurePatient_reference;
extern PROBETHYROIDUPTAKEPATMEAS AmuletWellThyroidUptakeMeasurePatient;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasureDose_total;
PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeMeasureDose_test;
PROBETHYROIDUPTAKEPROTOCOL AmuletWellThyroidUptakeMeasureDose_protocol;
short AmuletWellThyroidUptakeMeasureDose_startCh;
short AmuletWellThyroidUptakeMeasureDose_endCh;

static BACKGND AmuletWellThyroidUptakeMeasureDose_background;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void SetAmuletBackHTML(void);
void PushPageStack(unsigned char ucPage);
unsigned char PopPageStack(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletWellThyroidUptakeMeasureDose_menu(void){
	bool flgComplete;
	float fNumOfDoses;
	char message[104], backgroundStampText[51];
	int index, NumOfDoses;
	time_t backgroundStamp;
	long maxtime, mintime, longtime;
	long long int long2time;

	switch(m_iPhase){
		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_PRE_INIT:
			if(m_ucClear == 93){
				for(index=0; index<5; index++){
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].DoseSubUnit = index + 1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1ROICounts = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1ROICPM = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICounts = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICPM = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].DoseROIAverageCPM = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_SpectraID = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_SpectraID = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].CreatedOn = (time_t) 0;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.SpectraID = -1;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.SpectraID = -1;
				}

				if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1){
					AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICounts = -1;
					AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM = -1;
					AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM = -1;
					AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = -1;
					AmuletWellThyroidUptakeMeasureDose_total.StartDateTime = (time_t) 0;
					AmuletWellThyroidUptakeMeasureDose_total.EndDateTime = (time_t) 0;
					AmuletWellThyroidUptakeMeasureDose_total.WellBackgroundID = -1;
					AmuletWellThyroidUptakeMeasureDose_total.CreatedOn = (time_t) 0;
					AmuletWellThyroidUptakeMeasureDose_total.Inactive = FALSE;
				}else{
					DB_RetrieveProbeThyroidUptakeDoseMeasurement(&AmuletWellThyroidUptakeMeasureDose_total);
					if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID != -1){
						for(index=0; index<5; index++){
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].DoseSubUnit = index + 1;
							DB_RetrieveProbeThyroidUptakeSingleDoseMeasurement(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index]));
						}
					}
				}
				m_ucClear = 0;
			}

			if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1) SetAmuletByte(69, 0xFF);
			else SetAmuletByte(70, 0xFF);

			if(current.system == CI) SetAmuletString(117, "cpm");
			else SetAmuletString(117, "cps");

			if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber == -1) SetAmuletString(118, "Thyroid Uptake Measure Residual");
			else SetAmuletString(118, "Thyroid Uptake Measure Dose");

			AmuletWellThyroidUptakeMeasureDose_test.ProbeTUTestID = AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTestID;
			DB_RetrieveProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasureDose_test);
			if(AmuletWellThyroidUptakeMeasureDose_test.ProbeTUTestID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
				return;
			}
			AmuletWellThyroidUptakeMeasureDose_protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeMeasureDose_test.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&AmuletWellThyroidUptakeMeasureDose_protocol);
			if(AmuletWellThyroidUptakeMeasureDose_protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}

			Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &AmuletWellThyroidUptakeMeasureDose_startCh, &AmuletWellThyroidUptakeMeasureDose_endCh, NULL);

			SetAmuletString(80, AmuletWellThyroidUptakeMeasureDose_test.PatientID);
			strcpy(message, AmuletWellThyroidUptakeMeasureDose_test.LastName);
			strcat(message, ", ");
			strcat(message, AmuletWellThyroidUptakeMeasureDose_test.FirstName);
			SetAmuletString(81, message);

			switch(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber){
				case -1:
					SetAmuletString(83, "Measure Residual Liquid");
					break;

				case 0:
					SetAmuletString(83, "Measure Administered Dose");
					break;

				default:
					SetAmuletString(83, "Measure Reference Dose");
					break;
			}

			if(AmuletWellThyroidUptakeMeasureDose_total.WellBackgroundID == -1){
				if(!Mca_getBackgroundStatus() || !Mca_getBackgroundToday()){
					AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICounts = -1;
					AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM = -1;
					message[0] = 0;
				}else{
					AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICounts = Mca_getROIBackgroundCounts(AmuletWellThyroidUptakeMeasureDose_startCh, AmuletWellThyroidUptakeMeasureDose_endCh, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM = Mca_getROIBackgroundCPM(AmuletWellThyroidUptakeMeasureDose_startCh, AmuletWellThyroidUptakeMeasureDose_endCh, NULL);
					backgroundStamp = Mca_getBackgroundStamp();
					GetExtendedTimeInfo(&backgroundStamp, backgroundStampText);
					sprintf(message, "(%s)", backgroundStampText);
				}
			}else{
				AmuletWellThyroidUptakeMeasureDose_background.WellBackgroundID = AmuletWellThyroidUptakeMeasureDose_total.WellBackgroundID;
				DB_RetrieveBackgnd(&AmuletWellThyroidUptakeMeasureDose_background);
				backgroundStamp = AmuletWellThyroidUptakeMeasureDose_background.Spectrum.MeasuredOn;
				GetExtendedTimeInfo(&backgroundStamp, backgroundStampText);
				sprintf(message, "(%s)", backgroundStampText);
			}
			SetAmuletString(84, message);

			if(AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM == -1){
				message[0] = 0;
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM / 60.0);
			}
			SetAmuletString(86, message);

			if(AmuletWellThyroidUptakeMeasureDose_protocol.DoseMeasurementMethod == 2){
				flgComplete = TRUE;
				NumOfDoses = AmuletWellThyroidUptakeMeasureDose_test.DoseMultiplier;
				for(index=0; index<NumOfDoses; index++){
					if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber == -1){
						if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1) SetAmuletByte(81 + index, 0xFF);
						sprintf(message, "Residual #%d in Phantom at %d cm:", index + 1, AmuletWellThyroidUptakeMeasureDose_test.ProbeDistance);
					}else{
						if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1) SetAmuletByte(91 + index, 0xFF);
						if(AmuletWellThyroidUptakeMeasureDose_protocol.DoseForm == 1){
							sprintf(message, "Capsule #%d in Phantom at %d cm:", index + 1, AmuletWellThyroidUptakeMeasureDose_test.ProbeDistance);
						}else{
							sprintf(message, "Dose #%d in Phantom at %d cm:", index + 1, AmuletWellThyroidUptakeMeasureDose_test.ProbeDistance);
						}
					}
					if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID != -1) SetAmuletByte(71 + index, 0xFF);
					SetAmuletString(5*index + 87, message);

					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1ROICPM == -1) message[0] = 0;
					else{
						if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1ROICPM);
						else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1ROICPM / 60.0);
					}
					SetAmuletString(5*index + 89, message);

					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICPM == -1) message[0] = 0;
					else{
						if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICPM);
						else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICPM / 60.0);
						if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID != -1) SetAmuletByte(76 + index, 0xFF);
					}
					SetAmuletString(5*index + 90, message);

					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].DoseROIAverageCPM == -1){
						message[0] = 0;
						flgComplete = FALSE;
					}else{
						if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].DoseROIAverageCPM);
						else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].DoseROIAverageCPM / 60.0);
					}
					SetAmuletString(5*index + 91, message);
				}

				if(flgComplete){
					if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1){
						AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM = 0;
						for(index=0; index<NumOfDoses; index++){
							AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].DoseROIAverageCPM;
						}
					}
					if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM);
					else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM / 60.0);
					SetAmuletString(112, message);
					SetAmuletByte(96, 0xFF);

					fNumOfDoses = NumOfDoses;
					if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1){
						AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM - (fNumOfDoses * AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM);
						if(AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM < 0.0) AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = 0.0;
					}
					if(current.system == CI) sprintf(message, "Net Counts = Total - (Num_of_Doses * Background) = %.0f cpm", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM);
					else sprintf(message, "Net Counts = Total - (Num_of_Doses * Background) = %.0f cps", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM / 60.0);
					delayloop(20);
					SetAmuletString(113, message);
					SetAmuletByte(98, 0xFF);
				}
			}else{
				if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber == -1){
					if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1) SetAmuletByte(81, 0xFF);
					sprintf(message, "Residual in Phantom at %d cm:", AmuletWellThyroidUptakeMeasureDose_test.ProbeDistance);
				}else{
					if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1) SetAmuletByte(91, 0xFF);
					if(AmuletWellThyroidUptakeMeasureDose_protocol.DoseForm == 1){
						sprintf(message, "Capsule in Phantom at %d cm:", AmuletWellThyroidUptakeMeasureDose_test.ProbeDistance);
					}else{
						sprintf(message, "Dose in Phantom at %d cm:", AmuletWellThyroidUptakeMeasureDose_test.ProbeDistance);
					}
				}
				if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID != -1) SetAmuletByte(71, 0xFF);
				SetAmuletString(87, message);

				if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM == -1) message[0] = 0;
				else{
					if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM);
					else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM / 60.0);
				}
				SetAmuletString(89, message);

				if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM == -1) message[0] = 0;
				else{
					if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID != -1) SetAmuletByte(76, 0xFF);
					if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM);
					else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM / 60.0);
				}
				SetAmuletString(90, message);

				if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM == -1) message[0] = 0;
				else{
					if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM);
					else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM / 60.0);
				}
				SetAmuletString(91, message);

				if((AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM != -1) && (AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM != -1)){
					if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber == -1){ // Residual Measurement
						if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1){
							AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM;
							AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM - AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM;
							if(AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM < 0.0) AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = 0.0;
						}
						if(current.system == CI) sprintf(message, "Net Counts = Average - Background = %.0f cpm", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM);
						else sprintf(message, "Net Counts = Average - Background = %.0f cps", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM / 60.0);
						delayloop(20);
						SetAmuletString(113, message);
						SetAmuletByte(97, 0xFF);
					}else{
						switch(AmuletWellThyroidUptakeMeasureDose_protocol.DoseMeasurementMethod){
							case 1: // Dose Administered
								if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1){
									AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM;
									AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM - AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM;
									if(AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM < 0.0) AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = 0.0;
								}
								if(current.system == CI) sprintf(message, "Net Counts = Average - Background = %.0f cpm", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM);
								else sprintf(message, "Net Counts = Average - Background = %.0f cps", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM / 60.0);
								delayloop(20);
								SetAmuletString(113, message);
								SetAmuletByte(97, 0xFF);
								break;

							case 3:	// Multiply Measurement by Num of Doses
								NumOfDoses = AmuletWellThyroidUptakeMeasureDose_test.DoseMultiplier;
								fNumOfDoses = NumOfDoses;
								if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1){
									AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM = fNumOfDoses * AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM;
									AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM - (fNumOfDoses * AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM);
									if(AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM < 0.0) AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = 0.0;
								}
								if(current.system == CI) sprintf(message, "Net Counts = Num_Of_Doses * (Average - Background) = %.0f cpm\nNum_Of_Doses = %d", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM, NumOfDoses);
								else sprintf(message, "Net Counts = Num_Of_Doses * (Average - Background) = %.0f cpm\nNum_Of_Doses = %d", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM / 60.0, NumOfDoses);
								delayloop(20);
								SetAmuletString(113, message);
								SetAmuletByte(97, 0xFF);
								break;

							case 4: // Multiply Measurement by Factor
								if(AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID == -1){
									AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM = AmuletWellThyroidUptakeMeasureDose_test.DoseMultiplier * AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM;
									AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = AmuletWellThyroidUptakeMeasureDose_total.DoseROICPM - (AmuletWellThyroidUptakeMeasureDose_test.DoseMultiplier * AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM);
									if(AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM < 0.0) AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM = 0.0;
								}
								if(current.system == CI) sprintf(message, "Net Counts = Factor * (Average - Background) = %.0f cpm\nFactor = %.2f", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM, AmuletWellThyroidUptakeMeasureDose_test.DoseMultiplier);
								else sprintf(message, "Net Counts = Factor * (Average - Background) = %.0f cpm\nFactor = %.2f", AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM / 60.0, AmuletWellThyroidUptakeMeasureDose_test.DoseMultiplier);
								delayloop(20);
								SetAmuletString(113, message);
								SetAmuletByte(97, 0xFF);
								break;
						}
					}
				}
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT:
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_SAVE:
			if(AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM == -1.0){
				if(AmuletWellThyroidUptakeMeasureDose_total.BackgroundROICPM == -1.0){
					beep_amulet();
					Amulet_DisplayError("Thyroid Uptake", "Missing Background Measurement", TRUE);
					return;
				}else{
					beep_amulet();
					Amulet_DisplayError("Thyroid Uptake", "Missing Dose Measurement", TRUE);
					return;
				}
			}else if((AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM == 0.0) && (AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber != -1)){
				beep_amulet();
				if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber == 0){
					Amulet_DisplayError("Thyroid Uptake", "Administered Dose Net Counts can not be zero", TRUE);
				}else{
					Amulet_DisplayError("Thyroid Uptake", "Reference Dose Net Counts can not be zero", TRUE);
				}
				return;
			}else{
				AmuletWellThyroidUptakeMeasureDose_test.ProbeTUTestID = AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTestID;
				DB_RetrieveProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasureDose_test);
				if(AmuletWellThyroidUptakeMeasureDose_test.ProbeTUTestID <= 0){
					Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
					return;
				}
				AmuletWellThyroidUptakeMeasureDose_protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeMeasureDose_test.ProbeTUProtocolID;
				DB_RetrieveProbeThyroidUptakeProtocol(&AmuletWellThyroidUptakeMeasureDose_protocol);
				if(AmuletWellThyroidUptakeMeasureDose_protocol.ProbeTUProtocolID <= 0){
					Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
					return;
				}

				AmuletWellThyroidUptakeMeasureDose_total.WellBackgroundID = ((BACKGND *)Mca_getBackgroundMirror())->WellBackgroundID;
				AmuletWellThyroidUptakeMeasureDose_total.Inactive = FALSE;
				if(AmuletWellThyroidUptakeMeasureDose_protocol.DoseMeasurementMethod == 2){
					NumOfDoses = AmuletWellThyroidUptakeMeasureDose_test.DoseMultiplier;
					for(index=0;index<NumOfDoses;index++){
						if((AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.SpectraID == -1) && (AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.SpectraID != -1)){
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1ROICounts = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICounts;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICounts = -1;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICPM;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2ROICPM = -1.0;
							memcpy(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum), &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum), sizeof(DB_SPEC));
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.SpectraID = -1;
						}
					}

					maxtime = 0;
					for(index=0;index<NumOfDoses;index++){
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.SpectraID != -1){
							longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.MeasuredOn;
							if(longtime > maxtime) maxtime = longtime;
						}

						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.SpectraID != -1){
							longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.MeasuredOn;
							if(longtime > maxtime) maxtime = longtime;
						}
					}
					AmuletWellThyroidUptakeMeasureDose_total.EndDateTime = (time_t) maxtime;

					mintime = maxtime;
					for(index=0;index<NumOfDoses;index++){
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.SpectraID != -1){
							longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.MeasuredOn;
							if(longtime < mintime) mintime = longtime;
						}

						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.SpectraID != -1){
							longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.MeasuredOn;
							if(longtime < mintime) mintime = longtime;
						}
					}
					AmuletWellThyroidUptakeMeasureDose_total.StartDateTime = (time_t) mintime;

					if(maxtime != mintime){
						long2time = mintime;
						long2time = long2time + maxtime;
						long2time /= 2;
						longtime = long2time;
						AmuletWellThyroidUptakeMeasureDose_total.CreatedOn = (time_t) longtime;
					}else{
						AmuletWellThyroidUptakeMeasureDose_total.CreatedOn = (time_t) maxtime;
					}

					for(index=0;index<NumOfDoses;index++){
						if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.SpectraID != -1){
							longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.MeasuredOn;
							long2time = longtime;
							longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose2_Spectrum.MeasuredOn;
							long2time += longtime;
							long2time /= 2;
							longtime = long2time;
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].CreatedOn = (time_t) longtime;
						}else{
							AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].CreatedOn = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[index].Dose1_Spectrum.MeasuredOn;
						}
					}
				}else{
					if((AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.SpectraID == -1) && (AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.SpectraID != -1)){
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICounts = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICounts;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICounts = -1;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM = -1.0;
						memcpy(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum), &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum), sizeof(DB_SPEC));
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.SpectraID = -1;
					}

					maxtime = 0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.SpectraID != -1){
						longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.MeasuredOn;
						if(longtime > maxtime) maxtime = longtime;
					}
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.SpectraID != -1){
						longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.MeasuredOn;
						if(longtime > maxtime) maxtime = longtime;
					}
					AmuletWellThyroidUptakeMeasureDose_total.EndDateTime = (time_t) maxtime;

					mintime = maxtime;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.SpectraID != -1){
						longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.MeasuredOn;
						if(longtime < mintime) mintime = longtime;
					}
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.SpectraID != -1){
						longtime = (long) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.MeasuredOn;
						if(longtime < mintime) mintime = longtime;
					}
					AmuletWellThyroidUptakeMeasureDose_total.StartDateTime = (time_t) mintime;

					if(maxtime != mintime){
						long2time = mintime;
						long2time = long2time + maxtime;
						long2time /= 2;
						longtime = long2time;
						AmuletWellThyroidUptakeMeasureDose_total.CreatedOn = (time_t) longtime;
					}else{
						AmuletWellThyroidUptakeMeasureDose_total.CreatedOn = (time_t) maxtime;
					}

					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].CreatedOn = AmuletWellThyroidUptakeMeasureDose_total.CreatedOn;
				}

				if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber < 1){
					DB_WriteProbeThyroidUptakeDoseMeasurement(&AmuletWellThyroidUptakeMeasureDose_total, TRUE);
					if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber == -1) AmuletWellThyroidUptakeMeasureDose_test.ProbeTUTestPhase = 5;
					else if(AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber == 0) AmuletWellThyroidUptakeMeasureDose_test.ProbeTUTestPhase = 3;
					DB_WriteProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasureDose_test, TRUE);
					PopPageStack();
					SetAmuletBackHTML();
					return;
				}else{
					memcpy(&(AmuletWellThyroidUptakeMeasurePatient_reference), &(AmuletWellThyroidUptakeMeasureDose_total), sizeof(PROBETHYROIDUPTAKEDOSETOTALMEAS));
					AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM = AmuletWellThyroidUptakeMeasureDose_total.DoseROINetCPM;
					SetAmuletBackHTML();
					return;
				}
			}
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_BACKSPEC:
			beep_amulet();
			AmuletWellThyroidUptakeMeasureDose_background.WellBackgroundID = AmuletWellThyroidUptakeMeasureDose_total.WellBackgroundID;
			DB_RetrieveBackgnd(&AmuletWellThyroidUptakeMeasureDose_background);
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S11:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S12:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S21:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S22:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S31:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S32:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S41:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S42:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S51:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASDOSE_S52:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_SpectraID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Dose");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASDOSE_WAIT;
			break;
	}
}
