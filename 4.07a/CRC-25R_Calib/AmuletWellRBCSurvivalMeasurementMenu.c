#define PHASE_WELLRBCSURVIVALMEASUREMENT_PRE_INIT	0
#define PHASE_WELLRBCSURVIVALMEASUREMENT_WAIT		1
#define PHASE_WELLRBCSURVIVALMEASUREMENT_SAVE		2
#define PHASE_WELLRBCSURVIVALMEASUREMENT_BACKSPEC	3
#define PHASE_WELLRBCSURVIVALMEASUREMENT_S1			4
#define PHASE_WELLRBCSURVIVALMEASUREMENT_S2			5

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "sqlite3.h"
#include "database.h"
#include <math.h>

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
//extern PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasurePatient_reference;
//extern PROBETHYROIDUPTAKEPATMEAS AmuletWellThyroidUptakeMeasurePatient;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

WELLRBCSURVIVALMEAS AmuletWellRBCSurvivalMeasureMenu_measurement;
short AmuletWellRBCSurvivalMeasureMenu_startCh;
short AmuletWellRBCSurvivalMeasureMenu_endCh;
static BACKGND AmuletWellRBCSurvivalMeasurement_background;
static WELLRBCSURVIVALMEAS AmuletWellRBCSurvivalMeasureMenu_firstmeasurement;
WELLRBCSURVIVALTEST AmuletWellRBCSurvivalMeasureMenu_test;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void SetAmuletBackHTML(void);
void PushPageStack(unsigned char ucPage);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void SetAmuletBackHTML(void);

void AmuletWellRBCSurvivalMeasurement_menu(void){
	float denomin, numer;
	double dY, dX, dSumX, dSumY, dSumXX, dSumXY, dN, dK, dTau, dLnA0, dLogA0, dX0, dHalfDays, dX10, dY60;
	char message[104], message2[104], backgroundStampText[51];
	int index, iN;
	time_t backgroundStamp;
	long time1, time2, longtime;
	long long int long2time;
	WELLRBCSURVIVALRESULT result;

	switch(m_iPhase){
		case PHASE_WELLRBCSURVIVALMEASUREMENT_PRE_INIT:
			if(m_ucClear == 102){
				AmuletWellRBCSurvivalMeasureMenu_test.WellRBCSurvivalTestID = AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalTestID;
				DB_RetrieveWellRBCSurvivalTest(&AmuletWellRBCSurvivalMeasureMenu_test);
				if(AmuletWellRBCSurvivalMeasureMenu_test.WellRBCSurvivalTestID <= 0){
					Amulet_DisplayError("RBC Survival", "Unable to retrieve Test", TRUE);
					return;
				}

				if(AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID == -1){
					AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICounts = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICounts = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICounts = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.comment[0] = 0;
					AmuletWellRBCSurvivalMeasureMenu_measurement.WellBackgroundID = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_SpectraID = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_SpectraID = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.SpectraID = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.SpectraID = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn = (time_t) 0;
				}else{
					DB_RetrieveWellRBCSurvivalMeasurement(&AmuletWellRBCSurvivalMeasureMenu_measurement);
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID == -1){
						Amulet_DisplayError("RBC Survival", "Unable to retrieve Measurement", TRUE);
						return;
					}
				}
				m_ucClear = 0;
			}

			if(current.system == CI) SetAmuletString(100, "cpm");
			else SetAmuletString(100, "cps");

			SetAmuletString(101, AmuletWellRBCSurvivalMeasureMenu_test.PatientID);
			strcpy(message, AmuletWellRBCSurvivalMeasureMenu_test.LastName);
			strcat(message, ", ");
			strcat(message, AmuletWellRBCSurvivalMeasureMenu_test.FirstName);
			SetAmuletString(102, message);

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit == -1){
				message[0] = 0;
			}else{
				sprintf(message, "%.1f %%", AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit);
			}
			SetAmuletString(104, message);

			Mca_getROIChannels(AmuletWellRBCSurvivalMeasureMenu_test.StartROI, AmuletWellRBCSurvivalMeasureMenu_test.EndROI, &AmuletWellRBCSurvivalMeasureMenu_startCh, &AmuletWellRBCSurvivalMeasureMenu_endCh, NULL);

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.WellBackgroundID == -1){
				if(!Mca_getBackgroundStatus() || !Mca_getBackgroundToday()){
					AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICounts = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM = -1;
					message[0] = 0;
				}else{
					AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICounts = Mca_getROIBackgroundCounts(AmuletWellRBCSurvivalMeasureMenu_startCh, AmuletWellRBCSurvivalMeasureMenu_endCh, NULL);
					AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM = Mca_getROIBackgroundCPM(AmuletWellRBCSurvivalMeasureMenu_startCh, AmuletWellRBCSurvivalMeasureMenu_endCh, NULL);
					backgroundStamp = Mca_getBackgroundStamp();
					GetExtendedTimeInfo(&backgroundStamp, backgroundStampText);
					sprintf(message, "(%s)", backgroundStampText);
				}
			}else{
				AmuletWellRBCSurvivalMeasurement_background.WellBackgroundID = AmuletWellRBCSurvivalMeasureMenu_measurement.WellBackgroundID;
				DB_RetrieveBackgnd(&AmuletWellRBCSurvivalMeasurement_background);
				backgroundStamp = AmuletWellRBCSurvivalMeasurement_background.Spectrum.MeasuredOn;
				GetExtendedTimeInfo(&backgroundStamp, backgroundStampText);
				sprintf(message, "(%s)", backgroundStampText);
			}
			SetAmuletString(105, message);

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM == -1){
				message[0] = 0;
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM);
				else sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM / 60.0);
			}
			SetAmuletString(107, message);

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM == -1){
				message[0] = 0;
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM);
				else sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM / 60.0);
			}
			SetAmuletString(108, message);

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM == -1){
				message[0] = 0;
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM);
				else sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM / 60.0);
			}
			SetAmuletString(109, message);

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM == -1){
				message[0] = 0;
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM);
				else sprintf(message, "%.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM / 60.0);
			}
			SetAmuletString(110, message);

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID == -1){
				SetAmuletByte(90, 0xFF);
				SetAmuletByte(94, 0xFF);

				// Calculate CreatedOn
				if((AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM != -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM == -1)){
					AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.MeasuredOn;
				}else if((AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM == -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM != -1)){
					AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.MeasuredOn;
				}else if((AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM != -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM != -1)){
					time1 = (long) AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.MeasuredOn;
					time2 = (long) AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.MeasuredOn;
					long2time = time1;
					long2time = long2time + time2;
					long2time /= 2;
					longtime = long2time;
					AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn = (time_t) longtime;
				}else{
					AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn = (time_t) 0;
				}

				// Calculate ElapsedDays
				if(AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn == (time_t) 0){
					AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays = -1;
				}else{
					if(DB_RBCSurvivalMeasurementCount(&AmuletWellRBCSurvivalMeasureMenu_test) == 0){
						AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays = 0.0;
					}else{
						AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.WellRBCSurvivalMeasurementID = -1;
						AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.MeasurementNumber = 1;
						AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.WellRBCSurvivalTestID = AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalTestID;
						DB_RetrieveWellRBCSurvivalMeasurement(&AmuletWellRBCSurvivalMeasureMenu_firstmeasurement);
						AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays = difftime(AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn, AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.CreatedOn);
						AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays /= 86400.0;
					}
				}

				// Calculate Net CPM
				if((AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM != -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM != -1)){
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM > AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM){
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM = -1;
					}else{
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM - AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM;
					}
				}else{
					AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM = -1;
				}

				// Calculate Decay Corrected
				if((AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays != -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM != -1)){
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays == 0.0){
						AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM;
					}else{
						AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM = nucdecay(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM, AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn, AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.CreatedOn, AmuletWellRBCSurvivalMeasureMenu_test.HalfLife, AmuletWellRBCSurvivalMeasureMenu_test.HalfLifeUnit);
					}
				}else{
					AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM = -1;
				}

				// Calculate Remaining
				if((AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM != -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit != -1)){
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays != -1){
						if(AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays == 0.0){
							AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining = 100.0;
						}else{
							denomin = AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.DecayCorrectedSampleROINetCPM;
							denomin *= 100.0;
							denomin /= AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.SampleHematocrit;
							numer = AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM;
							numer *= 100.0;
							numer /= AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit;
							AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining = numer / denomin * 100.0;
						}
					}else{
						AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining = -1;
					}
				}else{
					AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining = -1;
				}
			}else{
				SetAmuletByte(91, 0xFF);
				SetAmuletByte(92, 0xFF);
				if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM != -1){
					SetAmuletByte(93, 0xFF);
				}
			}

			delayloop(50);
			message[0] = 0;
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays != -1){
				sprintf(message, "Elapsed Days: %.1f     ", AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays);
			}
			message2[0] = 0;
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM != -1){
				if(current.system == CI) sprintf(message2, "Net(cpm): %.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM);
				else sprintf(message2, "Net(cps): %.0f", AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM / 60.0);
			}
			strcat(message, message2);
			SetAmuletString(112, message);

			delayloop(50);
			message[0] = 0;
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM != -1){
				if(current.system == CI) sprintf(message, "Decay Corrected(cpm): %.0f     ", AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM);
				else sprintf(message, "Decay Corrected(cps): %.0f     ", AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM / 60.0);
			}
			message2[0] = 0;
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining != -1){
				sprintf(message2, "Remaining: %.1f %%", AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining);
			}
			strcat(message, message2);
			SetAmuletString(116, message);

			SetAmuletString(111, AmuletWellRBCSurvivalMeasureMenu_measurement.comment);

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLRBCSURVIVALMEASUREMENT_WAIT;
			break;

		case PHASE_WELLRBCSURVIVALMEASUREMENT_WAIT:
			break;

		case PHASE_WELLRBCSURVIVALMEASUREMENT_SAVE:
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit == -1){
				beep_amulet();
				Amulet_DisplayError("RBC Survival", "Missing Hematocrit", TRUE);
				return;
			}

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM == -1){
				beep_amulet();
				Amulet_DisplayError("RBC Survival", "Missing Background Measurement", TRUE);
				return;
			}

			if((AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM == -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM == -1)){
				beep_amulet();
				Amulet_DisplayError("RBC Survival", "Missing Measurement", TRUE);
				return;
			}

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM < AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM){
				beep_amulet();
				Amulet_DisplayError("RBC Survival", "Background is larger than Measurement", TRUE);
				return;
			}

			if((AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM == -1) && (AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM != -1)){
				AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM;
				AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICounts = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICounts;
				memcpy(&(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum), &(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum), sizeof(DB_SPEC));
				AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM = -1;
				AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICounts = -1;
				AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.SpectraID = -1;
			}

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM == -1){
				AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.MeasuredOn;
			}else{
				time1 = (long) AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.MeasuredOn;
				time2 = (long) AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.MeasuredOn;
				long2time = time1;
				long2time = long2time + time2;
				long2time /= 2;
				longtime = long2time;
				AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn = (time_t) longtime;
			}

			AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber = DB_RBCSurvivalMeasurementCount(&AmuletWellRBCSurvivalMeasureMenu_test) + 1;
			AmuletWellRBCSurvivalMeasureMenu_measurement.WellBackgroundID = ((BACKGND *)Mca_getBackgroundMirror())->WellBackgroundID;
			AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM - AmuletWellRBCSurvivalMeasureMenu_measurement.BackgroundROICPM;
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM < 0.0) AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM = 0;

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber > 1){
				AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.WellRBCSurvivalMeasurementID = -1;
				AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.MeasurementNumber = 1;
				AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.WellRBCSurvivalTestID = AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalTestID;
				DB_RetrieveWellRBCSurvivalMeasurement(&AmuletWellRBCSurvivalMeasureMenu_firstmeasurement);
				AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM = nucdecay(AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM, AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn, AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.CreatedOn, AmuletWellRBCSurvivalMeasureMenu_test.HalfLife, AmuletWellRBCSurvivalMeasureMenu_test.HalfLifeUnit);
				denomin = AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.DecayCorrectedSampleROINetCPM;
				denomin *= 100.0;
				denomin /= AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.SampleHematocrit;
				numer = AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM;
				numer *= 100.0;
				numer /= AmuletWellRBCSurvivalMeasureMenu_measurement.SampleHematocrit;
				AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining = numer / denomin * 100.0;
				AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays = difftime(AmuletWellRBCSurvivalMeasureMenu_measurement.CreatedOn, AmuletWellRBCSurvivalMeasureMenu_firstmeasurement.CreatedOn);
				AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays /= 86400.0;
			}else{
				AmuletWellRBCSurvivalMeasureMenu_measurement.DecayCorrectedSampleROINetCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROINetCPM;
				AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining = 100.0;
				AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays = 0.0;
			}
			DB_WriteRBCSurvivalMeasurement(&AmuletWellRBCSurvivalMeasureMenu_measurement, TRUE);

			if(AmuletWellRBCSurvivalMeasureMenu_test.WellRBCSurvivalTestPhase < 1){
				AmuletWellRBCSurvivalMeasureMenu_test.WellRBCSurvivalTestPhase = 1;
				DB_WriteRBCSurvivalTest(&AmuletWellRBCSurvivalMeasureMenu_test, TRUE);
			}

			if(AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber >= 4){
				iN = AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber;
				dN = iN;
				dX = 0.0;
				dY = log(100.0);
				dSumX = 0.0;
				dSumY = dY;
				dSumXX = 0.0;
				dSumXY = 0.0;
				for(index=2; index<=iN; index++){
					AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID = -1;
					AmuletWellRBCSurvivalMeasureMenu_measurement.MeasurementNumber = index;
					DB_RetrieveWellRBCSurvivalMeasurement(&AmuletWellRBCSurvivalMeasureMenu_measurement);
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalMeasurementID != -1){
						dX = AmuletWellRBCSurvivalMeasureMenu_measurement.ElapsedDays;
						dY = log(AmuletWellRBCSurvivalMeasureMenu_measurement.Remaining);
						dSumX += dX;
						dSumY += dY;
						dSumXX += (dX * dX);
						dSumXY += (dX * dY);
					}else{
						Amulet_DisplayError("RBC Survival", "Unable to retrieve Measurement", TRUE);
						return;
					}
				}

				dK = ((dSumX * dSumY) - (dN * dSumXY)) / ((dN * dSumXX) - (dSumX * dSumX));
				dTau = log(2) / dK;
				dLnA0 = (dSumY + (dK * dSumX)) / dN;
				dLogA0 = dLnA0 / log(10);
				dX0 = (dLnA0 - log(100)) / dK;
				dHalfDays = dTau + dX0;
				dX10 = (dLogA0 - 1) * log(10) / dK;
				dY60 = dLogA0 - dK * 60 / log(10);

				result.WellRBCSurvivalResultID = -1;
				result.WellRBCSurvivalTestID = AmuletWellRBCSurvivalMeasureMenu_measurement.WellRBCSurvivalTestID;
				result.HalfDays = dHalfDays;
				result.X10 = dX10;
				result.Y0 = dLogA0;
				result.Y60 = dY60;
				DB_WriteRBCSurvivalResult(&result, TRUE);
			}
			SetAmuletBackHTML();
			return;

		case PHASE_WELLRBCSURVIVALMEASUREMENT_BACKSPEC:
			beep_amulet();
			AmuletWellRBCSurvivalMeasurement_background.WellBackgroundID = AmuletWellRBCSurvivalMeasureMenu_measurement.WellBackgroundID;
			DB_RetrieveBackgnd(&AmuletWellRBCSurvivalMeasurement_background);
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCSurvivalMeasurement_background.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "RBC Survival Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			AmuletWellSpectrumMenu_inactive = FALSE;
			AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLRBCSURVIVALMEASUREMENT_S1:
			beep_amulet();
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_SpectraID != -1){
				AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.SpectraID = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "RBC Survival Measurement");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLRBCSURVIVALMEASUREMENT_WAIT;
			break;

		case PHASE_WELLRBCSURVIVALMEASUREMENT_S2:
			beep_amulet();
			if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_SpectraID != -1){
				AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.SpectraID = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "RBC Survival Measurement");
				AmuletWellSpectrumMenu_comment2[0] = 0;
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLRBCSURVIVALMEASUREMENT_WAIT;
			break;
	}
}
