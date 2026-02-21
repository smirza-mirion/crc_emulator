#define PHASE_WELLTHYROIDUPTAKEMEASPAT_PRE_INIT	0
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT		1
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_SAVE		2
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_SKIP		3
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_REF		4
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_REFS		5
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_BACKS1	6
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_BACKS2	7
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_NECKS1	8
#define PHASE_WELLTHYROIDUPTAKEMEASPAT_NECKS2	9

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "sqlite3.h"
#include "database.h"
#include "nuc.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern time_t clock_time;
extern PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasureDose_total;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

PROBETHYROIDUPTAKEPATMEAS AmuletWellThyroidUptakeMeasurePatient;
PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeMeasurePatient_test;
PROBETHYROIDUPTAKEPROTOCOL AmuletWellThyroidUptakeMeasurePatient_protocol;
PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasurePatient_reference;

static PROBETHYROIDUPTAKEDOSETOTALMEAS DoseAdmin, Residual;
static PROBETHYROIDUPTAKEPATMEAS PreDose;

void SetAmuletBackHTML(void);
unsigned char PopPageStack(void);
void PushPageStack(unsigned char ucPage);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);

void AmuletWellThyroidUptakeMeasurePatient_menu(void){
	int index;
	char message[51];
	short calcStart, calcEnd;
	long int maxtime, mintime, longtime;
	long long int long2time;
	double deltaseconds;
	float ResidualCPM, DecayPreDose;
	static float PeakEV;
	ENERGYPEAK energyPeak[10];

	switch(m_iPhase){
		case PHASE_WELLTHYROIDUPTAKEMEASPAT_PRE_INIT:
			if(m_ucClear == 94){
				AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTotalDoseMeasurementID = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.MeasurementNumber = AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber;
				AmuletWellThyroidUptakeMeasurePatient_reference.BackgroundROICounts = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.BackgroundROICPM = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.DoseROICPM = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.DoseROINetCPM = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.StartDateTime = (time_t) 0;
				AmuletWellThyroidUptakeMeasurePatient_reference.EndDateTime = (time_t) 0;
				AmuletWellThyroidUptakeMeasurePatient_reference.WellBackgroundID = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
				AmuletWellThyroidUptakeMeasurePatient_reference.CreatedOn = (time_t) 0;
				AmuletWellThyroidUptakeMeasurePatient_reference.Inactive = FALSE;
				for(index=0;index<5;index++){
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].DoseSubUnit = index + 1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1ROICounts = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1ROICPM = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2ROICounts = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2ROICPM = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].DoseROIAverageCPM = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1_SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2_SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].CreatedOn = (time_t) 0;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1_Spectrum.SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2_Spectrum.SpectraID = -1;
				}

				if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID == -1){
					AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = -1;
					AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = -1;
					AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg1ROICounts = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = -1;
					AmuletWellThyroidUptakeMeasurePatient.Uptake = -1;
					AmuletWellThyroidUptakeMeasurePatient.ElapsedTime = 0;
					AmuletWellThyroidUptakeMeasurePatient.comment[0] = 0;
					AmuletWellThyroidUptakeMeasurePatient.Neck1_SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient.Neck2_SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg1_SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg2_SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient.CreatedOn = (time_t) 0;
					AmuletWellThyroidUptakeMeasurePatient.Inactive = FALSE;
					AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.SpectraID = -1;
					AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.SpectraID = -1;
				}else{
					DB_RetrieveProbeThyroidUptakePatientMeasurement(&AmuletWellThyroidUptakeMeasurePatient);
				}
				PeakEV = -1;
				m_ucClear = 0;
			}
			if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID == -1) SetAmuletByte(89, 0xFF);
			else SetAmuletByte(88, 0xFF);

			AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
			DB_RetrieveProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasurePatient_test);
			if(AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
				return;
			}
			AmuletWellThyroidUptakeMeasurePatient_protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&AmuletWellThyroidUptakeMeasurePatient_protocol);
			if(AmuletWellThyroidUptakeMeasurePatient_protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}
			SetAmuletString(100, AmuletWellThyroidUptakeMeasurePatient_test.PatientID);
			strcpy(message, AmuletWellThyroidUptakeMeasurePatient_test.LastName);
			strcat(message, ", ");
			strcat(message, AmuletWellThyroidUptakeMeasurePatient_test.FirstName);
			SetAmuletString(101, message);

			if(AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber == 0){
				strcpy(message, "Pre-Dose Measurement");
				SetAmuletByte(90, 0x00); // Hide Reference
				SetAmuletByte(87, 0x00); // Hide Reference
				if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID == -1) SetAmuletByte(91, 0xFF); // Show Skip Button
			}else{
				sprintf(message, "Measurement #%d", AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber);
				SetAmuletByte(91, 0x00); // Hide Skip Button
				if(AmuletWellThyroidUptakeMeasurePatient_protocol.DoseDecayCorrect){
					SetAmuletByte(90, 0x00); // Hide Reference Button
					SetAmuletByte(87, 0x00); // Hide Reference Button
				}else{
					if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID == -1) SetAmuletByte(90, 0xFF); // Show Reference Button
					else SetAmuletByte(87, 0xFF); // Show Reference Button
				}
			}
			SetAmuletString(103, message);

			if(AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM == -1) message[0] = 0;
			else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM / 60.0);
			}
			SetAmuletString(104, message);

			sprintf(message, "%d", AmuletWellThyroidUptakeMeasurePatient_test.ProbeDistance);
			SetAmuletString(105, message);

			if(AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM < 0){
				strcpy(message, "");
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM / 60.0);
			}
			SetAmuletString(106, message);

			if(AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM < 0){
				strcpy(message, "");
			}else{
				if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID != -1) SetAmuletByte(85, 0xFF);
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM / 60.0);
			}
			SetAmuletString(107, message);

			if(AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM < 0){
				strcpy(message, "");
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM / 60.0);
			}
			SetAmuletString(108, message);

#ifdef DISABLE_TU_PREDOSE_PEAK
			SetAmuletByte(93, 0x00);
			SetAmuletByte(94, 0x00);
#else
			if(AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber == 0){
				PeakEV = NuclideData_getEffectivePrimary(AmuletWellThyroidUptakeMeasurePatient_protocol.NuclideID);

				if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID == -1){
					for(index=0; index<10; index++){
						energyPeak[index].peakindex = -1;
						energyPeak[index].startCh = -1;
						energyPeak[index].endCh = -1;
						energyPeak[index].found = FALSE;
					}
					energyPeak[0].energy = PeakEV;

					if(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.SpectraID != -1){
						if(Mca_installedDetector == DET_DRILLEDPROBE700) Mca_findPeak(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.DecompressedSpectra, energyPeak, TRUE, NULL);
						else Mca_findPeak(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.DecompressedSpectra, energyPeak, FALSE, NULL);

						if(energyPeak[0].found){
							Mca_getROIChannels(AmuletWellThyroidUptakeMeasurePatient_test.StartROI, AmuletWellThyroidUptakeMeasurePatient_test.EndROI, &calcStart, &calcEnd, NULL);
							AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
							AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts;
							AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM /= AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.LiveTime;
							AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM *= 60.0;
							SetAmuletByte(93, 0x00);
						}else{
							AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = 0;
							AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = 0;
							SetAmuletByte(93, 0xFF);
						}
					}else{
						AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = -1;
						AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = -1;
						SetAmuletByte(93, 0x00);
					}

					for(index=0; index<10; index++){
						energyPeak[index].peakindex = -1;
						energyPeak[index].startCh = -1;
						energyPeak[index].endCh = -1;
						energyPeak[index].found = FALSE;
					}
					energyPeak[0].energy = PeakEV;


					if(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.SpectraID != -1){
						if(Mca_installedDetector == DET_DRILLEDPROBE700) Mca_findPeak(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.DecompressedSpectra, energyPeak, TRUE, NULL);
						else Mca_findPeak(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.DecompressedSpectra, energyPeak, FALSE, NULL);

						if(energyPeak[0].found){
							Mca_getROIChannels(AmuletWellThyroidUptakeMeasurePatient_test.StartROI, AmuletWellThyroidUptakeMeasurePatient_test.EndROI, &calcStart, &calcEnd, NULL);
							AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
							AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts;
							AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM /= AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.LiveTime;
							AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM *= 60.0;
							SetAmuletByte(94, 0x00);
						}else{
							AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = 0;
							AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = 0;
							SetAmuletByte(94, 0xFF);
						}
					}else{
						AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = -1;
						AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = -1;
						SetAmuletByte(94, 0x00);
					}

					if(AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM == -1 && AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM == -1){
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = -1;
					}else if(AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM != -1 && AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM == -1){
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
					}else if(AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM == -1 && AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM != -1){
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
					}else{
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM /= 2.0;
					}
				}
			}else{
				SetAmuletByte(93, 0x00);
				SetAmuletByte(94, 0x00);
			}
#endif

			if(AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM < 0){
				strcpy(message, "");
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM / 60.0);
			}
			SetAmuletString(109, message);

			if(AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM < 0){
				strcpy(message, "");
			}else{
				if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID != -1) SetAmuletByte(86, 0xFF);
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM / 60.0);
			}
			SetAmuletString(110, message);

			if(AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM < 0){
				strcpy(message, "");
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM / 60.0);
			}
			SetAmuletString(111, message);

			if(current.system == CI) SetAmuletString(112, "cpm");
			else SetAmuletString(112, "cps");

			SetAmuletString(113, AmuletWellThyroidUptakeMeasurePatient.comment);

			if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID == -1){
				if((AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM >= 0) && (AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM >= 0)){
					AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM - AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM;
					if(AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM < 0.0) AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM = 0.0;
				}else{
					AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM = -1.0;
				}

				// Calculate CreatedOn
				maxtime = 0;
				if(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.MeasuredOn;
					if(longtime > maxtime) maxtime = longtime;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.MeasuredOn;
					if(longtime > maxtime) maxtime = longtime;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.MeasuredOn;
					if(longtime > maxtime) maxtime = longtime;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.MeasuredOn;
					if(longtime > maxtime) maxtime = longtime;
				}

				mintime = maxtime;
				if(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.MeasuredOn;
					if(longtime < mintime) mintime = longtime;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.MeasuredOn;
					if(longtime < mintime) mintime = longtime;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.MeasuredOn;
					if(longtime < mintime) mintime = longtime;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.SpectraID != -1){
					longtime = (long) AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.MeasuredOn;
					if(longtime < mintime) mintime = longtime;
				}

				if(maxtime != mintime){
					long2time = mintime;
					long2time = long2time + maxtime;
					long2time /= 2;
					longtime = long2time;
					AmuletWellThyroidUptakeMeasurePatient.CreatedOn = (time_t) longtime;
				}else{
					AmuletWellThyroidUptakeMeasurePatient.CreatedOn = (time_t) maxtime;
				}
			}

			if(AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM < 0){
				strcpy(message, "");
				SetAmuletString(114, message);
				SetAmuletByte(92, 0x00);
			}else{
				if(current.system == CI) sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM);
				else sprintf(message, "%.0f", AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM / 60.0);
				SetAmuletString(114, message);
				SetAmuletByte(92, 0xFF);

				if(AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber > 0){
					if(AmuletWellThyroidUptakeMeasurePatient.ProbeTUPatientMeasurementID == -1){
						// Calculate ElapsedTime
						deltaseconds = difftime(AmuletWellThyroidUptakeMeasurePatient.CreatedOn, AmuletWellThyroidUptakeMeasurePatient_test.DoseAdministeredOn);
						AmuletWellThyroidUptakeMeasurePatient.ElapsedTime = deltaseconds / 3600.0; // In Hours

						if(AmuletWellThyroidUptakeMeasurePatient_protocol.DoseDecayCorrect){
							// Calculate Admin Dose Decay
							DoseAdmin.ProbeTUTotalDoseMeasurementID = -1;
							DoseAdmin.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
							DoseAdmin.MeasurementNumber = 0;
							DB_RetrieveProbeThyroidUptakeDoseMeasurement(&DoseAdmin);
							if(DoseAdmin.ProbeTUTotalDoseMeasurementID != -1){
								AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM = nucdecay(DoseAdmin.DoseROINetCPM, DoseAdmin.CreatedOn, AmuletWellThyroidUptakeMeasurePatient.CreatedOn, AmuletWellThyroidUptakeMeasurePatient_test.HalfLife, AmuletWellThyroidUptakeMeasurePatient_test.HalfLifeUnit);
							}
						}

						// Calculate Uptake if Dose Measurement exists
						if(AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM != -1){

							// Adjust Patient Measurement for Pre-Dose
							if(AmuletWellThyroidUptakeMeasurePatient_protocol.PreDoseMeasurement){
								PreDose.ProbeTUPatientMeasurementID = -1;
								PreDose.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
								PreDose.MeasurementNumber = 0;
								DB_RetrieveProbeThyroidUptakePatientMeasurement(&PreDose);

								if(PreDose.ProbeTUPatientMeasurementID == -1){
									AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = -1;
								}else{
									if(PreDose.Uptake == -1){ // PreDose Skipped
										AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM;
									}else{
										// Adjust Patient ROI Net CPM with the decayed value of the PreDose
										DecayPreDose = nucdecay(PreDose.AdjustedPatientROINetCPM, PreDose.CreatedOn, AmuletWellThyroidUptakeMeasurePatient.CreatedOn, AmuletWellThyroidUptakeMeasurePatient_test.HalfLife, AmuletWellThyroidUptakeMeasurePatient_test.HalfLifeUnit);
										AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM - DecayPreDose;
										if(AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM < 0) AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = 0;
									}
								}
							}else{
								AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM;
							}

							if(AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM != -1){
								// Adjust Dose Measurement for Residual
								if(AmuletWellThyroidUptakeMeasurePatient_protocol.ResidualMeasurement){
									Residual.ProbeTUTotalDoseMeasurementID = -1;
									Residual.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
									Residual.MeasurementNumber = -1;
									DB_RetrieveProbeThyroidUptakeDoseMeasurement(&Residual);

									if(Residual.ProbeTUTotalDoseMeasurementID == -1){
										AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = -1;
									}else{
										ResidualCPM = nucdecay(Residual.DoseROINetCPM, Residual.CreatedOn, AmuletWellThyroidUptakeMeasurePatient.CreatedOn, AmuletWellThyroidUptakeMeasurePatient_test.HalfLife, AmuletWellThyroidUptakeMeasurePatient_test.HalfLifeUnit);
										AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM - ResidualCPM;
										if(AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM < 0) AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = 0;
									}
								}else{
									AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM;
								}

								if(AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM != -1){
									AmuletWellThyroidUptakeMeasurePatient.Uptake = AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM / AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM;
									AmuletWellThyroidUptakeMeasurePatient.Uptake *= 100.0;
									sprintf(message, "%.1f %%", AmuletWellThyroidUptakeMeasurePatient.Uptake);
									SetAmuletString(115, message);
									SetAmuletByte(95, 0xFF);
								}else{
									SetAmuletString(115, "");
									SetAmuletByte(95, 0x00);
								}
							}else{
								SetAmuletString(115, "");
								SetAmuletByte(95, 0x00);
							}
						}else{
							SetAmuletString(115, "");
							SetAmuletByte(95, 0x00);
						}
					}else{
						sprintf(message, "%.1f %%", AmuletWellThyroidUptakeMeasurePatient.Uptake);
						SetAmuletString(115, message);
						SetAmuletByte(95, 0xFF);
					}
				}else{
					SetAmuletString(115, "");
					SetAmuletByte(95, 0x00);
				}
			}

			SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT:
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_SAVE:
			beep_amulet();
			AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
			DB_RetrieveProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasurePatient_test);
			if(AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
				return;
			}
			AmuletWellThyroidUptakeMeasurePatient_protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&AmuletWellThyroidUptakeMeasurePatient_protocol);
			if(AmuletWellThyroidUptakeMeasurePatient_protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}
			if(PeakEV >= 0) AmuletWellThyroidUptakeMeasurePatient_test.PeakEV = PeakEV;

			// Check for Leg and Neck Measurements
			if((!AmuletWellThyroidUptakeMeasurePatient_protocol.DoseDecayCorrect) && (AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber > 0) && (AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM < 0)){
				Amulet_DisplayError("Thyroid Uptake", "Missing Reference Dose Measurement", TRUE);
				return;
			}

			if(AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM < 0){
				Amulet_DisplayError("Thyroid Uptake", "Missing Patient Background Measurement", TRUE);
				return;
			}
			if(AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM < 0){
				Amulet_DisplayError("Thyroid Uptake", "Missing Neck Measurement", TRUE);
				return;
			}

			// Swap Count1 and Count2
			if((AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM == -1) && (AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM != -1)){
				AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM = AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM;
				AmuletWellThyroidUptakeMeasurePatient.Leg1ROICounts = AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts;
				memcpy(&(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum), &(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum), sizeof(DB_SPEC));
				AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts = -1;
				AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM = -1;
				AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.SpectraID = -1;
			}

			if((AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM == -1) && (AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM != -1)){
				AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
				AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts;
				memcpy(&(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum), &(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum), sizeof(DB_SPEC));
				AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = -1;
				AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = -1;
				AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.SpectraID = -1;
			}

			if(AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber == 0){
				if(AmuletWellThyroidUptakeMeasurePatient_protocol.DoseDecayCorrect){
					AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestPhase = 2; // Measure Admin Dose
				}else{
					AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestPhase = 3; // Enter Admin Date
				}

				AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM = 0;
				AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM;
				AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM;
				AmuletWellThyroidUptakeMeasurePatient.Uptake = 0;
				AmuletWellThyroidUptakeMeasurePatient.ElapsedTime = 0;
				AmuletWellThyroidUptakeMeasurePatient.Inactive = FALSE;
			}else{
				AmuletWellThyroidUptakeMeasurePatient.Inactive = FALSE;

				// Calculate DoseROINetCPM
				if(!AmuletWellThyroidUptakeMeasurePatient_protocol.DoseDecayCorrect){
					if(AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM == -1){
						Amulet_DisplayError("Thyroid Uptake", "Missing Reference Dose Measurement", TRUE);
						return;
					}
				}else{
					// Calculate Admin Dose Decay
					DoseAdmin.ProbeTUTotalDoseMeasurementID = -1;
					DoseAdmin.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
					DoseAdmin.MeasurementNumber = 0;
					DB_RetrieveProbeThyroidUptakeDoseMeasurement(&DoseAdmin);
					if(DoseAdmin.ProbeTUTotalDoseMeasurementID == -1){
						Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve Administered Dose Measurement", TRUE);
						return;
					}
					AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM = nucdecay(DoseAdmin.DoseROINetCPM, DoseAdmin.CreatedOn, AmuletWellThyroidUptakeMeasurePatient.CreatedOn, AmuletWellThyroidUptakeMeasurePatient_test.HalfLife, AmuletWellThyroidUptakeMeasurePatient_test.HalfLifeUnit);
				}

				// Adjust Patient Measurement for Pre-Dose
				if(AmuletWellThyroidUptakeMeasurePatient_protocol.PreDoseMeasurement){
					PreDose.ProbeTUPatientMeasurementID = -1;
					PreDose.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
					PreDose.MeasurementNumber = 0;
					DB_RetrieveProbeThyroidUptakePatientMeasurement(&PreDose);
					if(PreDose.ProbeTUPatientMeasurementID == -1){
						Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve Pre-Dose Measurement", TRUE);
						return;
					}
					if(PreDose.Uptake == -1){
						AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM;
					}else{
						DecayPreDose = nucdecay(PreDose.AdjustedPatientROINetCPM, PreDose.CreatedOn, AmuletWellThyroidUptakeMeasurePatient.CreatedOn, AmuletWellThyroidUptakeMeasurePatient_test.HalfLife, AmuletWellThyroidUptakeMeasurePatient_test.HalfLifeUnit);
						AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM - DecayPreDose;
					}
				}else{
					AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM < 0) AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = 0;

				// Adjust Dose Measurement for Residual
				if(AmuletWellThyroidUptakeMeasurePatient_protocol.ResidualMeasurement){
					Residual.ProbeTUTotalDoseMeasurementID = -1;
					Residual.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
					Residual.MeasurementNumber = -1;
					DB_RetrieveProbeThyroidUptakeDoseMeasurement(&Residual);
					if(Residual.ProbeTUTotalDoseMeasurementID == -1){
						Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve Residual Liquid Measurement", TRUE);
						return;
					}

					ResidualCPM = nucdecay(Residual.DoseROINetCPM, Residual.CreatedOn, AmuletWellThyroidUptakeMeasurePatient.CreatedOn, AmuletWellThyroidUptakeMeasurePatient_test.HalfLife, AmuletWellThyroidUptakeMeasurePatient_test.HalfLifeUnit);
					AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM - ResidualCPM;
				}else{
					AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM;
				}
				if(AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM < 0) AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = 0;

				// Calculate Uptake
				AmuletWellThyroidUptakeMeasurePatient.Uptake = AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM / AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM;
				AmuletWellThyroidUptakeMeasurePatient.Uptake *= 100.0;

				AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestPhase = 5;
			}

			DB_WriteProbeThyroidUptakePatientMeasurement(&AmuletWellThyroidUptakeMeasurePatient, TRUE);
			DB_WriteProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasurePatient_test, TRUE);

			if((AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber != 0) && (!AmuletWellThyroidUptakeMeasurePatient_protocol.DoseDecayCorrect)){
				DB_WriteProbeThyroidUptakeDoseMeasurement(&AmuletWellThyroidUptakeMeasurePatient_reference, TRUE);
			}
			PopPageStack();
			SetAmuletBackHTML();
			return;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_SKIP:
			beep_amulet();
			AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
			DB_RetrieveProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasurePatient_test);
			if(AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
				return;
			}
			AmuletWellThyroidUptakeMeasurePatient_protocol.ProbeTUProtocolID = AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUProtocolID;
			DB_RetrieveProbeThyroidUptakeProtocol(&AmuletWellThyroidUptakeMeasurePatient_protocol);
			if(AmuletWellThyroidUptakeMeasurePatient_protocol.ProbeTUProtocolID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Protocol", TRUE);
				return;
			}

			if(AmuletWellThyroidUptakeMeasurePatient_protocol.DoseDecayCorrect){
				AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestPhase = 2;
			}else{
				AmuletWellThyroidUptakeMeasurePatient_test.ProbeTUTestPhase = 3;
			}
			DB_WriteProbeThyroidUptakeTest(&AmuletWellThyroidUptakeMeasurePatient_test, TRUE);

			AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = 0;
			AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = 0;
			AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.Leg1ROICounts = 0;
			AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts = 0;
			AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.PatientROINetCPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.AdjustedPatientROINetCPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.AdjustedDoseROINetCPM = 0;
			AmuletWellThyroidUptakeMeasurePatient.Uptake = -1;
			AmuletWellThyroidUptakeMeasurePatient.ElapsedTime = 0;
			AmuletWellThyroidUptakeMeasurePatient.comment[0] = 0;
			AmuletWellThyroidUptakeMeasurePatient.Neck1_SpectraID = -1;
			AmuletWellThyroidUptakeMeasurePatient.Neck2_SpectraID = -1;
			AmuletWellThyroidUptakeMeasurePatient.Leg1_SpectraID = -1;
			AmuletWellThyroidUptakeMeasurePatient.Leg2_SpectraID = -1;
			AmuletWellThyroidUptakeMeasurePatient.CreatedOn = clock_time;
			AmuletWellThyroidUptakeMeasurePatient.Inactive = FALSE;
			AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.SpectraID = -1;
			AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.SpectraID = -1;
			AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.SpectraID = -1;
			AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.SpectraID = -1;
			DB_WriteProbeThyroidUptakePatientMeasurement(&AmuletWellThyroidUptakeMeasurePatient, TRUE);

			PopPageStack();
			SetAmuletBackHTML();
			return;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_REF:
			beep_amulet();
			AmuletWellThyroidUptakeMeasurePatient.DoseROINetCPM = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTotalDoseMeasurementID = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.MeasurementNumber = AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber;
			AmuletWellThyroidUptakeMeasurePatient_reference.BackgroundROICounts = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.BackgroundROICPM = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.DoseROICPM = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.DoseROINetCPM = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.StartDateTime = (time_t) 0;
			AmuletWellThyroidUptakeMeasurePatient_reference.EndDateTime = (time_t) 0;
			AmuletWellThyroidUptakeMeasurePatient_reference.WellBackgroundID = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
			AmuletWellThyroidUptakeMeasurePatient_reference.CreatedOn = (time_t) 0;
			AmuletWellThyroidUptakeMeasurePatient_reference.Inactive = FALSE;
			for(index=0;index<5;index++){
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].ProbeTUSingleDoseMeasurementID = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].DoseSubUnit = index + 1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1ROICounts = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1ROICPM = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2ROICounts = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2ROICPM = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].DoseROIAverageCPM = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1_SpectraID = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2_SpectraID = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].ProbeTUTotalDoseMeasurementID = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].CreatedOn = (time_t) 0;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose1_Spectrum.SpectraID = -1;
				AmuletWellThyroidUptakeMeasurePatient_reference.SingleMeasurement[index].Dose2_Spectrum.SpectraID = -1;
			}
			AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID = -1;
			AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
			AmuletWellThyroidUptakeMeasureDose_total.MeasurementNumber = AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber;
			m_ucClear = 93;
			SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
			return;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_REFS:
			beep_amulet();
			AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTotalDoseMeasurementID = -1;
			AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTestID = AmuletWellThyroidUptakeMeasurePatient.ProbeTUTestID;
			AmuletWellThyroidUptakeMeasurePatient_reference.MeasurementNumber = AmuletWellThyroidUptakeMeasurePatient.MeasurementNumber;
			DB_RetrieveProbeThyroidUptakeDoseMeasurement(&AmuletWellThyroidUptakeMeasurePatient_reference);
			if(AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTotalDoseMeasurementID != -1){
				AmuletWellThyroidUptakeMeasureDose_total.ProbeTUTotalDoseMeasurementID = AmuletWellThyroidUptakeMeasurePatient_reference.ProbeTUTotalDoseMeasurementID;
				m_ucClear = 93;
				SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKEMEASUREDOSE_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_BACKS1:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasurePatient.Leg1_SpectraID != -1){
				AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasurePatient.Leg1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Patient");
				strcpy(AmuletWellSpectrumMenu_comment2, "Background");
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_BACKS2:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasurePatient.Leg2_SpectraID != -1){
				AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasurePatient.Leg2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Patient");
				strcpy(AmuletWellSpectrumMenu_comment2, "Background");
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKEMEASPAT_NECKS1:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasurePatient.Neck1_SpectraID != -1){
				AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasurePatient.Neck1_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Patient");
				strcpy(AmuletWellSpectrumMenu_comment2, "Neck");
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT;
			break;


		case PHASE_WELLTHYROIDUPTAKEMEASPAT_NECKS2:
			beep_amulet();
			if(AmuletWellThyroidUptakeMeasurePatient.Neck2_SpectraID != -1){
				AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.SpectraID = AmuletWellThyroidUptakeMeasurePatient.Neck2_SpectraID;
				DB_ReadSpectrum(&(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum));
				memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum), sizeof(DB_SPEC));
				strcpy(AmuletWellSpectrumMenu_comment1, "Thyroid Uptake Patient");
				strcpy(AmuletWellSpectrumMenu_comment2, "Neck");
				AmuletWellSpectrumMenu_comment3[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKEMEASPAT_WAIT;
			break;
	}
}
