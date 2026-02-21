#define PHASE_WELLMEASURE_PRE_INIT	0
#define PHASE_WELLMEASURE_WAIT		1
#define PHASE_WELLMEASURE_START		2
#define PHASE_WELLMEASURE_CREATE512GRAPH	3
#define PHASE_WELLMEASURE_DRAWGRAPH		4
#define PHASE_WELLMEASURE_DONE			5
#define PHASE_WELLMEASURE_STOP		6
#define PHASE_WELLMEASURE_SAVE_BKG	7
#define PHASE_WELLMEASURE_WAITFOREVER	8
#define PHASE_WELLMEASURE_ACCEPT	9
#define PHASE_WELLMEASURE_WIPESKIP	10
#define PHASE_WELLMEASURE_WIPESAVE	11
#define PHASE_WELLMEASURE_WIPEVIEW	12
#define PHASE_WELLMEASURE_WIPEEND	13
#define PHASE_WELLMEASURE_BACK10	14
#define PHASE_WELLMEASURE_BACK1		15
#define PHASE_WELLMEASURE_FORWARD1	16
#define PHASE_WELLMEASURE_FORWARD10	17
#define PHASE_WELLMEASURE_CURSOR	18
#define PHASE_WELLMEASURE_REP_START 19

#define PHASE_WELLMEASURE_WRITEGRAPH_PHASE0			0
#define PHASE_WELLMEASURE_WRITEGRAPH_PHASE42		1
#define PHASE_WELLMEASURE_WRITEGRAPH_PHASE85		2
#define PHASE_WELLMEASURE_WRITEGRAPH_PHASE127		3
#define PHASE_WELLMEASURE_WRITEGRAPH_PHASE170		4
#define PHASE_WELLMEASURE_WRITEGRAPH_PHASE212		5
#define PHASE_WELLMEASURE_WRITEGRAPH_SENDREFRESH	6
#define PHASE_WELLMEASURE_WRITEGRAPH_FINISH			7

#define PHASE_WELLMEASURE_DRAWGRAPH_INIT			0
#define PHASE_WELLMEASURE_DRAWGRAPH_GRAPH1			1
#define PHASE_WELLMEASURE_DRAWGRAPH_GRAPH2			2
#define PHASE_WELLMEASURE_DRAWGRAPH_GRAPH3			3

#define DELAY_TO_MEM_CLEAR		1
#define DELAY_TO_BIG_GRAPH_DRAW	4
#define DELAY_TO_SM_GRAPH_DRAW	1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "amulet.h"
#include "qspi.h"
#include "counter.h"
#include "ff.h"
#include "wipes.h"
#include "database.h"
#include "bioassay.h"

enum peak_type{
	NOT_FOUND,
	PRIMARY,
	SECONDARY,
	TERTIARY,
	SUMMED
};

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletWord(unsigned char ucIndex, short unsigned int uiValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
unsigned char PopPageStack(void);
void AmuletError_setNewPage(uint value);
void PushPageStack(unsigned char ucPage);
void ClearNextPhase(void);
void trim(char *acByte);
static void LoadWipeLocationData(char *title);
void Mca_calcPeaks(DB_SPEC *spectrum, WELLWIPEPEAK wipepeak[]);
void Mca_matchWipePeaks(WELLWIPEPEAK wipepeak[], float Threshold, WELLWIPENUCLIDE WellWipeNuclide[10], int NumOfChannels);
static void UpdateSpectrum(unsigned char *display);
static void UpdateSpectrumText(void);
void AmuletWellMainScreenMenu_getWellStandardSN(char *SN);
float AmuletWellMainScreenMenu_getWellStandardActivity(void);
time_t AmuletWellMainScreenMenu_getWellStandardDate(void);
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);

unsigned char m_ucSetWellMeasure;
long AmuletWellMeasurementMenu_reps;
long AmuletWellMeasurementMenu_partialreps;
bool AmuletWellMeasurementMenu_WipeBackgroundThreshold;
int AmuletWellMeasurementMenu_cursor;
char generalNuclideID;
ENERGYPEAK energyPeak[10];
float startEV[10];
float endEV[10];
WELLWIPE AmuletWellMeasureMenu_wellWipe;
short int AmuletWellMeasureMenu_countTime[8];
bool AmuletWellMeasurementMenu_unlockDetector;
bool AmuletWellMeasurementMenu_autoMeasure;
char AmuletWellMeasurementMenu_testID[11];
char AmuletWellMeasurementMenu_testIDPCComm[11];
WELLWIPE AmuletWellMeasureMenu_wellWipePCComm;
int AmuletWellMeasurementMenu_generalDetectorTest;

extern CURRENT current;
extern int m_iPhase;
extern unsigned char m_ucClear;
extern volatile char m_acTitle[51];
extern DB_SPEC AmuletWellMeasureEfficiency_spectrum;
extern float AmuletWellMeasureEfficiency_effectiveEnergy;
extern WELLSCHILLINGTEST AmuletWellSchillingMenu_test;
extern WELLPLASMATEST AmuletWellPlasmaMenu_test;
extern WELLRBCTEST AmuletWellRBCMenu_test;
extern bool AmuletWellSchillingMenu_backgroundMeasured;
extern bool AmuletWellPlasmaMenu_backgroundMeasured;
extern bool AmuletWellRBCMenu_backgroundMeasured;
extern WELLSYSTEMTEST AmuletWellSystemTestAnalysisMenu_systemTest;
extern WELLMDATEST AmuletWellMDATestMenu_mda;
extern bool AmuletWellChiTestMenu_finished;
extern WELLCHITEST AmuletWellChiTestMenu_test;
extern DB_SPEC AmuletWellGeneralAnalysis_spectrum;
extern char AmuletWellGeneralAnalysis_generalNuclideID;
extern float AmuletWellGeneralAnalysis_startEV;
extern float AmuletWellGeneralAnalysis_endEV;
extern bool AmuletWellStabilityTestMenu_finished;
extern WELLWIPE AmuletWellWipeReportMenu_wellWipe;
extern short AmuletWellMeasureEfficiency_mode;
extern PROBEBIOASSAYEFFICIENCY AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff;
extern bool AmuletWellSetupBioAssayMeasureEfficiencyMenu_backgroundMeasured;
extern bool AmuletWellBioAssay_backgroundMeasured;
extern int AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime;
extern float I131Lower_keV, I131Upper_keV, I125Lower_keV, I125Upper_keV, I123Lower_keV, I123Upper_keV;
extern PROBEBIOASSAYTEST AmuletWellBioAssay_test;
extern PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeMeasurePatient_test;
extern PROBETHYROIDUPTAKEPATMEAS AmuletWellThyroidUptakeMeasurePatient;
extern PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeMeasureDose_test;
extern PROBETHYROIDUPTAKEDOSETOTALMEAS AmuletWellThyroidUptakeMeasureDose_total;
extern WELLRBCSURVIVALTEST AmuletWellRBCSurvivalMeasureMenu_test;
extern WELLRBCSURVIVALMEAS AmuletWellRBCSurvivalMeasureMenu_measurement;

static bool AmuletWellMeasurementMenu_write255Bytes(unsigned char *display, short int offset, bool reset){
	static unsigned long int time_threshold;
	static unsigned char ucPhase;
	unsigned char index;
	bool returnvalue;

	if(reset){
		ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_PHASE0;
		time_threshold = g_csec_tstamp + 0;
		returnvalue = FALSE;
	}else{
		switch(ucPhase){
			case PHASE_WELLMEASURE_WRITEGRAPH_PHASE0:
				if(g_csec_tstamp > time_threshold){
					if((512 - offset) >= 255){
						for(index=0; index<42; index++) SetAmuletByte(index, display[index + offset]);
						time_threshold = g_csec_tstamp + DELAY_TO_MEM_CLEAR;
						ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_PHASE42;
					}else{
						index = 0;
						while((index + offset) < 512){
							SetAmuletByte(index, display[index+offset]);
							index++;
						}
						time_threshold = g_csec_tstamp + 0;
						ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_SENDREFRESH;
					}
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_WRITEGRAPH_PHASE42:
				if(g_csec_tstamp > time_threshold){
					for(index=42; index<85; index++) SetAmuletByte(index, display[index + offset]);
					time_threshold = g_csec_tstamp + DELAY_TO_MEM_CLEAR;
					ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_PHASE85;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_WRITEGRAPH_PHASE85:
				if(g_csec_tstamp > time_threshold){
					for(index=85; index<127; index++) SetAmuletByte(index, display[index + offset]);
					time_threshold = g_csec_tstamp + DELAY_TO_MEM_CLEAR;
					ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_PHASE127;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_WRITEGRAPH_PHASE127:
				if(g_csec_tstamp > time_threshold){
					for(index=127; index<170; index++) SetAmuletByte(index, display[index + offset]);
					time_threshold = g_csec_tstamp + DELAY_TO_MEM_CLEAR;
					ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_PHASE170;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_WRITEGRAPH_PHASE170:
				if(g_csec_tstamp > time_threshold){
					for(index=170; index<212; index++) SetAmuletByte(index, display[index + offset]);
					time_threshold = g_csec_tstamp + DELAY_TO_MEM_CLEAR;
					ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_PHASE212;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_WRITEGRAPH_PHASE212:
				if(g_csec_tstamp > time_threshold){
					for(index=212; index<255; index++) SetAmuletByte(index, display[index + offset]);
					time_threshold = g_csec_tstamp + DELAY_TO_MEM_CLEAR;
					ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_SENDREFRESH;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_WRITEGRAPH_SENDREFRESH:
				if(g_csec_tstamp > time_threshold){
					switch(offset){
						case 0:
							SetAmuletWord(122, 0xFFFF);
							time_threshold = g_csec_tstamp + DELAY_TO_BIG_GRAPH_DRAW;
							break;

						case 254:
							SetAmuletWord(123, 0xFFFF);
							time_threshold = g_csec_tstamp + DELAY_TO_BIG_GRAPH_DRAW;
							break;

						case 508:
							SetAmuletLine(542, 329 - display[253], 543, 329 - display[254], 0, 1);
							SetAmuletWord(124, 0xFFFF);
							time_threshold = g_csec_tstamp + DELAY_TO_SM_GRAPH_DRAW;
							break;
					}
					ucPhase = PHASE_WELLMEASURE_WRITEGRAPH_FINISH;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_WRITEGRAPH_FINISH:
				if(g_csec_tstamp > time_threshold){
					returnvalue = TRUE;
				}else{
					returnvalue = FALSE;
				}
				break;
		}
	}
	return returnvalue;
}

bool AmuletWellMeasurementMenu_drawGraph(unsigned char *display, bool reset){
	static unsigned char ucPhase;
	bool returnvalue;

	if(reset){
		AmuletWellMeasurementMenu_write255Bytes(display, 0, TRUE);
		ucPhase = PHASE_WELLMEASURE_DRAWGRAPH_GRAPH1;
		returnvalue = FALSE;
	}else{
		switch(ucPhase){
			case PHASE_WELLMEASURE_DRAWGRAPH_GRAPH1:
				if(AmuletWellMeasurementMenu_write255Bytes(display, 0, FALSE)){
					AmuletWellMeasurementMenu_write255Bytes(display, 0, TRUE);
					ucPhase = PHASE_WELLMEASURE_DRAWGRAPH_GRAPH2;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_DRAWGRAPH_GRAPH2:
				if(AmuletWellMeasurementMenu_write255Bytes(display, 254, FALSE)){
					AmuletWellMeasurementMenu_write255Bytes(display, 254, TRUE);
					ucPhase = PHASE_WELLMEASURE_DRAWGRAPH_GRAPH3;
				}
				returnvalue = FALSE;
				break;

			case PHASE_WELLMEASURE_DRAWGRAPH_GRAPH3:
				if(AmuletWellMeasurementMenu_write255Bytes(display, 508, FALSE)){
					returnvalue = TRUE;
				}else{
					returnvalue = FALSE;
				}
				break;
		}
	}
	return returnvalue;
}

void AmuletWellMeasurement_menu(void){
	char acMsg[100];
	float combinedgain1, combinedgain2, cpm, roicounts, primaryenergy;
	short acquiretime, timetype, index, nucIndex, startCh, endCh;
	static bool readspectrum;
	static bool displayadvancedbutton;
	static bool calibrated;
	static char title[51];
	static unsigned char display[512];
	float hv, hvcode;
	long int acc, average;
	float tempCPM, tempCPS;
	// float maxcounts;
	bool detectedROI;
	static long currentRepIndex;
	//PEAK peak[40];
	short calcStart, calcEnd;
	//short dummynp1;
	WELLWIPELOCATION *selectedWipeLocation;
	int locationIndex;
	WELLWIPENUCLIDE *wipeNuclide;
	NUCDATA nuc;

//	short hv_value;
//	static short hv_direction = 1;
//	bool flgFoundPrimary;

	switch(m_iPhase){
		case PHASE_WELLMEASURE_PRE_INIT:
			if(m_ucClear == 18){
				Mca_setTimeType(COUNT_LIVE);

				AmuletWellMeasurementMenu_cursor = 0;
				strcpy(title, (char *)m_acTitle);
				spec_meas.done_flag = FALSE;
				displayadvancedbutton = FALSE;

				for(index=0; index<10; index++){
					startEV[index] = -1.0;
					endEV[index] = -1.0;
					energyPeak[index].energy = 0;
					energyPeak[index].peakindex = -1;
					energyPeak[index].startCh = -1;
					energyPeak[index].endCh = -1;
					energyPeak[index].found = FALSE;
				}

				switch(m_ucSetWellMeasure){
					case 1: // General
						generalNuclideID = -1;
						displayadvancedbutton = TRUE;
						Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_GENERAL]);
						break;

					case 2: // Background
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						if(AmuletWellMeasurementMenu_WipeBackgroundThreshold) Mca_setAcquireTime(WipesCurrentType(1)->CountTime);
						else Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_BACKGROUND]);
						break;

					case 3: // Efficiency
						generalNuclideID = -1;
						energyPeak[0].energy = AmuletWellMeasureEfficiency_effectiveEnergy;
						displayadvancedbutton = FALSE;
						Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_EFFICIENCY]);
						break;

					case 4:	// Wipe Test
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						LoadWipeLocationData(title);
						break;

					case 7: // Schilling Standard
					case 8: // Schilling Aliqot
						generalNuclideID = -1;
						startEV[0] = AmuletWellSchillingMenu_test.StartROI;
						endEV[0] = AmuletWellSchillingMenu_test.EndROI;
						displayadvancedbutton = FALSE;
						Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_SCHILLING]);
						break;

					case 9:  // Plasma Standard
					case 10: // Plasma Whole Blood
					case 11: // Plasma Plasma
						generalNuclideID = -1;
						startEV[0] = AmuletWellPlasmaMenu_test.StartROI;
						endEV[0] = AmuletWellPlasmaMenu_test.EndROI;
						displayadvancedbutton = FALSE;
						Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_PLASMA]);
						break;

					case 12: // RBC Whole Standard
					case 13: // RBC Plasma Standard
					case 14: // RBC Whole Sample
					case 15: // RBC Plasma Sample
						generalNuclideID = -1;
						startEV[0] = AmuletWellRBCMenu_test.StartROI;
						endEV[0] = AmuletWellRBCMenu_test.EndROI;
						displayadvancedbutton = FALSE;
						Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_RBC]);
						break;

					case 16: // System Test
						generalNuclideID = -1;
						nucIndex = NuclideData_getIndexFromName("Cs137");
						primaryenergy = NuclideData_getEffectivePrimary(nucIndex);
						Mca_getEndPoints(primaryenergy, &(startEV[0]), &(endEV[0]), FALSE);
						displayadvancedbutton = FALSE;
						Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_SYSTEM]);
						break;

					case 17: // MDA Test
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						Mca_setAcquireTime(AmuletWellMeasureMenu_countTime[WELL_TIME_MDA]);
						break;

					case 18: // Chi Square Test
						generalNuclideID = -1;
						currentRepIndex = 0;
						AmuletWellChiTestMenu_finished = FALSE;
						for(index=0;index<20; index++){
							AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.SpectraID = 0;
							AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.MeasuredOn = 0;
						}
						startEV[0] = AmuletWellChiTestMenu_test.StartROI;
						endEV[0] = AmuletWellChiTestMenu_test.EndROI;
						Mca_setAcquireTime(AmuletWellChiTestMenu_test.SampleTime);
						displayadvancedbutton = FALSE;
						break;

					case 19: // Stability Test
						SetAmuletBackHTML();
						return;
						/*generalNuclideID = -1;
						currentRepIndex = 0;
						AmuletWellStabilityTestMenu_finished = FALSE;
						displayadvancedbutton = FALSE;
						break;*/

					case 20:
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						startEV[0] = I131Lower_keV;
						endEV[0] = I131Upper_keV;
						Mca_setAcquireTime(AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime);
						break;

					case 21:
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						startEV[0] = I125Lower_keV;
						endEV[0] = I125Upper_keV;
						Mca_setAcquireTime(AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime);
						break;

					case 22:
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						startEV[0] = I123Lower_keV;
						endEV[0] = I123Upper_keV;
						Mca_setAcquireTime(AmuletWellSetupBioAssayMeasureEfficiencyMenu_countTime);
						break;

					case 23:
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						Mca_setAcquireTime(BioAssayCountTime());
						break;

					case 24:
					case 25:
					case 26:
					case 27:
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						startEV[0] = AmuletWellThyroidUptakeMeasurePatient_test.StartROI;
						endEV[0] = AmuletWellThyroidUptakeMeasurePatient_test.EndROI;
						Mca_setAcquireTime(AmuletWellThyroidUptakeMeasurePatient_test.CountingTime);
						break;

					case 28:
					case 29:
					case 30:
					case 31:
					case 32:
					case 33:
					case 34:
					case 35:
					case 36:
					case 37:
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						startEV[0] = AmuletWellThyroidUptakeMeasureDose_test.StartROI;
						endEV[0] = AmuletWellThyroidUptakeMeasureDose_test.EndROI;
						Mca_setAcquireTime(AmuletWellThyroidUptakeMeasureDose_test.CountingTime);
						break;

					case 38:
					case 39:
						generalNuclideID = -1;
						displayadvancedbutton = FALSE;
						startEV[0] = AmuletWellRBCSurvivalMeasureMenu_test.StartROI;
						endEV[0] = AmuletWellRBCSurvivalMeasureMenu_test.EndROI;
						Mca_setAcquireTime(AmuletWellRBCSurvivalMeasureMenu_test.CountingTime);
						break;

					default:
						SetAmuletBackHTML();
						return;
				}
				m_ucClear = 0;

				if(!Mca_getCalibrationStatus()){
					PopPageStack();
					if(Mca_existsAutoCal()){
						Amulet_DisplayError("Calibration Error", "Detector parameters have changed\nPlease perform Auto Calibration", TRUE);
					}else{
						Amulet_DisplayError("Calibration Error", "Detector has never been calibrated\nPlease perform Auto Calibration first",TRUE);
					}
					return;
				}else if(!Mca_getCalibratedToday()){
					calibrated = TRUE;
					DISPLAY_WARNING("Warning", "Expired calibration\nPlease run Auto Calibration");
				}else{
					calibrated = TRUE;
				}

				Mca_startAcquisition();
			}

			if(calibrated){
				switch(m_ucSetWellMeasure){
					case 1:
						if(Mca_installedDetector == DET_DRILLEDPROBE700){
							if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_WELL) SetAmuletString(92, "As Well");
							else if(AmuletWellMeasurementMenu_generalDetectorTest == DET_DRILLEDPROBE700_AS_PROBE) SetAmuletString(92, "As Probe");
							else  SetAmuletString(92, "");
						}
					case 3:
						if(Mca_installedDetector == DET_DRILLEDPROBE700){
							if(AmuletWellMeasureEfficiency_mode == MEASURE_EFF_EDIT_WELL || AmuletWellMeasureEfficiency_mode == MEASURE_EFF_ADD_WELL) SetAmuletString(92, "As Well");
							else if(AmuletWellMeasureEfficiency_mode == MEASURE_EFF_EDIT_PROBE || AmuletWellMeasureEfficiency_mode == MEASURE_EFF_ADD_PROBE) SetAmuletString(92, "As Probe");
						}
						break;

					case 4:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
						if(Mca_installedDetector == DET_DRILLEDPROBE700){
							SetAmuletString(92, "As Well");
						}
						break;

					case 16:
						if(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest == DET_DRILLEDPROBE700_AS_WELL) SetAmuletString(92, "As Well");
						else if(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) SetAmuletString(92, "As Probe");
						break;

					case 17:
						if(AmuletWellMDATestMenu_mda.DetectorTest == DET_DRILLEDPROBE700_AS_WELL) SetAmuletString(92, "As Well");
						else if(AmuletWellMDATestMenu_mda.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) SetAmuletString(92, "As Probe");
						break;

					case 18:
						if(AmuletWellChiTestMenu_test.DetectorTest == DET_DRILLEDPROBE700_AS_WELL) SetAmuletString(92, "As Well");
						else if(AmuletWellChiTestMenu_test.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) SetAmuletString(92, "As Probe");
						break;

					case 20:
					case 21:
					case 22:
					case 23:
					case 24:
					case 25:
					case 26:
					case 27:
					case 28:
					case 29:
					case 30:
					case 31:
					case 32:
					case 33:
					case 34:
					case 35:
					case 36:
					case 37:
						if(Mca_installedDetector == DET_DRILLEDPROBE700) SetAmuletString(92, "As Probe");
						break;

					case 38:
					case 39:
						if(Mca_installedDetector == DET_DRILLEDPROBE700) SetAmuletString(92, "As Well");
						break;
				}

				sprintf(acMsg, "%d", Mca_getNumOfChannels());
				SetAmuletString(118, acMsg);

				combinedgain1 = Mca_getGain1();
				combinedgain2 = Mca_getGain2();
				combinedgain1 /= 64.0;
				combinedgain2 /= 64.0;
				combinedgain1 *= combinedgain2;
				sprintf(acMsg, "%.2f", combinedgain1);
				SetAmuletString(101, acMsg);

				sprintf(acMsg, "%d", Mca_getZeroOpAmpOffset());
				SetAmuletString(102, acMsg);

				sprintf(acMsg, "%d", Mca_getThreshold());
				SetAmuletString(103, acMsg);

				hvcode = Mca_getHVValue();
				if((Mca_installedDetector == DET_WELL) || (Mca_installedDetector == DET_BETA)) hv = (.158974 * hvcode) + 595.0;
				else if((Mca_installedDetector == DET_PROBE700) || (Mca_installedDetector == DET_WELL700) || (Mca_installedDetector == DET_DRILLEDPROBE700)) hv = .263158 * hvcode;

				//sprintf(acMsg, "%.0f Volts (%.0f)", hv, hvcode);
				sprintf(acMsg, "%.0f Volts", hv);
				SetAmuletString(104, acMsg);

				acquiretime = Mca_getAcquireTime();
				timetype = Mca_getTimeType();
				if(timetype == COUNT_REAL) sprintf(acMsg, "%d (sec) real", acquiretime);
				else sprintf(acMsg, "%d (sec) live", acquiretime);
				SetAmuletString(105, acMsg);
				SetAmuletString(108, title);
				if(current.system == CI) SetAmuletString(111, "total cpm:");
				else if(current.system == BQ) SetAmuletString(111, "total cps:");

				if(generalNuclideID == -2){
					if(startEV[0]>=0.0 && endEV[0]>=0.0){
						sprintf(acMsg, "%.1f - %.1f keV", startEV[0], endEV[0]);
						SetAmuletString(90, acMsg);
					}else{
						startEV[0] = -1.0;
						endEV[0] = -1.0;
						generalNuclideID = -1;
					}
				}else if(generalNuclideID>=0){
					if(startEV[0]>=0.0 && endEV[0]>=0.0){
						NuclideData_getName(generalNuclideID, acMsg);
						SetAmuletString(90, acMsg);
					}else{
						startEV[0] = -1.0;
						endEV[0] = -1.0;
						generalNuclideID = -1;
					}
				}

				Mca_sendDetectorType();
				SetAmuletWord(100, 0xFFFF); // Display MCA parameters

				if(spec_meas.done_flag){
					delayloop(100);
					readspectrum = FALSE;
					m_iPhase = PHASE_WELLMEASURE_CREATE512GRAPH;
				}else{
					readspectrum = TRUE;

					if(displayadvancedbutton) SetAmuletWord(92, 0xFFFF); // Display Change ROI Button

					if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(79, 0xFFFF);

					if((m_ucSetWellMeasure==18) || (m_ucSetWellMeasure==19)){ // Display Measure Chi Square and Cancel Buttons
						SetAmuletWord(82, 0xFFFF);
						SetAmuletWord(94, 0xFFFF);
					}
					else if(m_ucSetWellMeasure==4){ // Display Change Time, Measure, Skip, Skip All Buttons
						if(!AmuletWellMeasurementMenu_autoMeasure){
							SetAmuletWord(80, 0xFFFF);
							SetAmuletWord(81, 0xFFFF);
							SetAmuletWord(88, 0xFFFF);
							SetAmuletWord(91, 0xFFFF);
						}else{
							SetAmuletWord(80, 0);
							SetAmuletWord(81, 0);
							SetAmuletWord(88, 0);
							SetAmuletWord(91, 0);
						}
					}
					else{ //Display Change Time, Measure and Stop Buttons
						SetAmuletWord(80, 0xFFFF);
						SetAmuletWord(81, 0xFFFF);
						SetAmuletWord(94, 0xFFFF);
					}

					//SetAmuletWord(119, 0xFFFF); // Clear Graph
					//delayloop(50);

					SetAmuletWord(95, 0xFFFF);
					if((m_ucSetWellMeasure == 4) && AmuletWellMeasurementMenu_autoMeasure){
						strcpy(AmuletWellMeasurementMenu_testIDPCComm, AmuletWellMeasurementMenu_testID);
						AmuletWellMeasureMenu_wellWipePCComm.WellWipeID = 0;
						m_iPhase = PHASE_WELLMEASURE_START;
					}else m_iPhase = PHASE_WELLMEASURE_WAIT;
				}

				SetAmuletLine(288, 75, 288, 330, 0, 1);
				SetAmuletLine(288, 330, 800, 330, 0, 1);
			}else{
				m_iPhase = PHASE_WELLMEASURE_WAITFOREVER;
			}
			break;

		case PHASE_WELLMEASURE_WAIT:
			break;

		case PHASE_WELLMEASURE_START:
			beep_amulet();
			if((m_ucSetWellMeasure==18) || (m_ucSetWellMeasure==19)){
				sprintf(acMsg, "%ld of %ld", currentRepIndex + 1, AmuletWellMeasurementMenu_reps);
				SetAmuletString(114, acMsg);
				SetAmuletWord(110, 0xFFFF); // Display Chi-Square count
				SetAmuletWord(114, 0xFFFF); // Display Stop Button
				AmuletWellMeasurementMenu_partialreps = 0;
			}else{
				SetAmuletWord(113, 0xFFFF); // Display Stop Button
			}

			AmuletError_setNewPage(0);
			readspectrum = TRUE;
			Mca_refreshAcquireTime();
			Mca_startAcquisition();
			m_iPhase = PHASE_WELLMEASURE_CREATE512GRAPH;
			break;

		case PHASE_WELLMEASURE_REP_START:
			// Need to add code for done_flag
			beep_amulet();

			AmuletError_setNewPage(0);
			readspectrum = TRUE;

			Mca_refreshAcquireTime();
			Mca_startAcquisition();
			m_iPhase = PHASE_WELLMEASURE_CREATE512GRAPH;
			break;

		case PHASE_WELLMEASURE_CREATE512GRAPH:
			if(readspectrum) Mca_getSpectrum();
			switch(spec_meas.num_of_channels){
				case 256:
					for(index=0; index<511; index++){
						if((index%2)== 0){
							display[index] = spec_meas.display[index/2];
						}else{
							average = spec_meas.display[(index-1)/2];
							average += spec_meas.display[(index+1)/2];
							average >>= 1;
							display[index] = average;
						}
						display[511] = display[510];
					}
					break;

				case 512:
					for(index=0; index<512; index++) display[index] = spec_meas.display[index];
					break;

				case 1024:
					for(index=0; index<512; index++){
						average = spec_meas.display[2*index];
						average += spec_meas.display[2*index + 1];
						average >>= 1;
						display[index] = average;
					}
					break;

				case 2048:
					for(index=0; index<512; index++){
						average = spec_meas.display[4*index];
						average += spec_meas.display[4*index+1];
						average += spec_meas.display[4*index+2];
						average += spec_meas.display[4*index+3];
						average >>= 2;
						display[index] = average;
					}
					break;

				case 4096:
					for(index=0; index<512; index++){
						average = spec_meas.display[8*index];
						average += spec_meas.display[8*index+1];
						average += spec_meas.display[8*index+2];
						average += spec_meas.display[8*index+3];
						average += spec_meas.display[8*index+4];
						average += spec_meas.display[8*index+5];
						average += spec_meas.display[8*index+6];
						average += spec_meas.display[8*index+7];
						average >>= 3;
						display[index] = average;
					}
					break;
			}

			sprintf(acMsg, "%ld", spec_meas.max_count);
			SetAmuletString(106, acMsg);
			sprintf(acMsg, "%.2f", spec_meas.real_time);
			SetAmuletString(107, acMsg);
			sprintf(acMsg, "%.2f", spec_meas.live_time);
			SetAmuletString(115, acMsg);
			cpm = spec_meas.averagecpm;
			if(current.system == BQ) cpm /= 60.0;
			sprintf(acMsg, "%.0f", cpm);
			SetAmuletString(110, acMsg);
			SetAmuletWord(101, 0xFFFF); // Update elapsed time, and Cpm

			AmuletWellMeasurementMenu_drawGraph(display, TRUE);
			m_iPhase = PHASE_WELLMEASURE_DRAWGRAPH;
			break;

		case PHASE_WELLMEASURE_DRAWGRAPH:
			if(AmuletWellMeasurementMenu_drawGraph(display, FALSE)){
				m_iPhase = PHASE_WELLMEASURE_DONE;
			}
			break;

		case PHASE_WELLMEASURE_DONE:
			delayloop(50);
			if(spec_meas.done_flag){
				if(m_ucSetWellMeasure != 18) SetAmuletWord(102, 0xFFFF); // Hide Stop Button

				for(index=0; index<10; index++){
					energyPeak[index].peakindex = -1;
					energyPeak[index].startCh = -1;
					energyPeak[index].endCh = -1;
					energyPeak[index].found = FALSE;
				}

				if((m_ucSetWellMeasure == 20) || (m_ucSetWellMeasure == 21) || (m_ucSetWellMeasure == 22)){
					if(Mca_installedDetector == DET_DRILLEDPROBE700) Mca_findPeak(spec_meas.spectrum, energyPeak, TRUE, NULL);
					else Mca_findPeak(spec_meas.spectrum, energyPeak, FALSE, NULL);
				}else{
					Mca_findPeak(spec_meas.spectrum, energyPeak, FALSE, NULL);
				}

				detectedROI = FALSE;
				roicounts = 0;
				for(index=0; index<10; index++){
					if((energyPeak[index].energy > 0) && (energyPeak[index].found)){
						if((energyPeak[index].startCh >=0) && (energyPeak[index].endCh>=0)){
							Mca_drawROI(energyPeak[index].startCh, energyPeak[index].endCh, display, spec_meas.num_of_channels);
							delayloop(20);
							detectedROI = TRUE;
							roicounts += Mca_getROICounts(spec_meas.spectrum, energyPeak[index].startCh, energyPeak[index].endCh);
						}
					}

					if((startEV[index]>=0.0) && (endEV[index]>=0.0)){
						Mca_getROIChannels(startEV[index], endEV[index], &calcStart, &calcEnd, NULL);
						Mca_drawROI(calcStart, calcEnd, display, spec_meas.num_of_channels);
						delayloop(20);
						detectedROI = TRUE;
						roicounts += Mca_getROICounts(spec_meas.spectrum, calcStart, calcEnd);
					}
				}

				if(detectedROI){
					tempCPS = roicounts;
					tempCPS /= spec_meas.live_time;
					tempCPM = tempCPS * 60.0;
					if(current.system == CI){
						SetAmuletString(112, "ROI cpm:");
						sprintf(acMsg, "%.0f", tempCPM);
					}else{
						SetAmuletString(112, "ROI cps:");
						sprintf(acMsg, "%.0f", tempCPS);
					}
					SetAmuletString(113, acMsg);
					SetAmuletWord(105, 0xFFFF); // Show ROI Cpm
				}

				if(readspectrum) beep_amulet();

				//if((m_ucSetWellMeasure != 18) && (m_ucSetWellMeasure != 19) && (m_ucSetWellMeasure != 4)) SetAmuletWord(102, 0xFFFF); // Remove Stop and Show Measure, ChangeTime and Finished Label

				ClearNextPhase();
				m_iPhase = PHASE_WELLMEASURE_WAIT;

				if(spec_meas.max_exceeded) DISPLAY_ERROR("ERROR", "Count Rate is Too High");

				// HV Drift
//					if(m_ucSetWellMeasure == 4){
//						hv_value = Mca_getHVValue();
//						if(hv_direction) hv_value++;
//						else hv_value--;
//						MCASetHV(hv_value, TRUE);
//						if(hv_value > 720) hv_direction = 0;
//						else if(hv_value < 600) hv_direction = 1;
//					}

				if(Mca_isMultiDetector() && AmuletWellMeasurementMenu_unlockDetector) SetAmuletWord(79, 0xFFFF);
				delayloop(50);
				switch(m_ucSetWellMeasure){
					case 1:
						AmuletWellMeasurementMenu_cursor = 0;
						UpdateSpectrumText();
						delayloop(30);
						SetAmuletWord(116, 0xFFFF); // Show Cursor text
						SetAmuletWord(80, 0xFFFF);
						SetAmuletWord(81, 0xFFFF);
						SetAmuletWord(83, 0xFFFF);
						SetAmuletWord(84, 0xFFFF);
						SetAmuletWord(85, 0xFFFF);
						delayloop(30);
						SetAmuletWord(86, 0xFFFF);
						SetAmuletWord(87, 0xFFFF);
						SetAmuletWord(92, 0xFFFF);
						SetAmuletWord(93, 0xFFFF);
						SetAmuletWord(94, 0xFFFF);
						SetAmuletWord(95, 0xFFFF);
						break;

					case 2:
						if(AmuletWellMeasurementMenu_WipeBackgroundThreshold){
							tempCPM = WipesCurrentType(1)->Threshold;
							if(spec_meas.averagecpm > tempCPM){
								if(current.system==CI){
									sprintf(acMsg, "Background has exceeded threshold of %.1f cpm\nRemove any sources from detector and try again", tempCPM);
								}else{
									tempCPS = tempCPM;
									tempCPS /= 60.0;
									sprintf(acMsg, "Background has exceeded threshold of %.1f cps\nRemove any sources from detector and try again", tempCPS);
								}

								DISPLAY_ERROR("Background Error", acMsg);
								SetAmuletWord(80, 0xFFFF); // Count Time Button
								SetAmuletWord(81, 0xFFFF); // Measure Button
								SetAmuletWord(94, 0xFFFF); // Cancel Button
								SetAmuletWord(95, 0xFFFF);
								spec_meas.done_flag = FALSE;
								//SetAmuletWord(121, 0xFFFF);	// Show Cancel Button
							}else{
								SetAmuletWord(80, 0xFFFF); // Count Time Button
								SetAmuletWord(81, 0xFFFF); // Measure Button
								SetAmuletWord(83, 0xFFFF); // Finished Message
								SetAmuletWord(84, 0xFFFF); // 10 Back Button
								SetAmuletWord(85, 0xFFFF); // 1 Back Button
								SetAmuletWord(86, 0xFFFF); // 1 Forward Button
								delayloop(30);
								SetAmuletWord(87, 0xFFFF); // 10 Forward Button
								SetAmuletWord(93, 0xFFFF); // Accept Button
								SetAmuletWord(94, 0xFFFF); // Cancel Button
								SetAmuletWord(95, 0xFFFF);
								delayloop(30);
								AmuletWellMeasurementMenu_cursor = 0;
								UpdateSpectrumText();
								SetAmuletWord(116, 0xFFFF); // Show Cursor text
							}
						}else{
							SetAmuletWord(80, 0xFFFF); // Count Time Button
							SetAmuletWord(81, 0xFFFF); // Measure Button
							SetAmuletWord(83, 0xFFFF); // Finished Message
							SetAmuletWord(84, 0xFFFF); // 10 Back Button
							SetAmuletWord(85, 0xFFFF); // 1 Back Button
							SetAmuletWord(86, 0xFFFF); // 1 Forward Button
							delayloop(30);
							SetAmuletWord(87, 0xFFFF); // 10 Forward Button
							SetAmuletWord(93, 0xFFFF); // Accept Button
							SetAmuletWord(94, 0xFFFF); // Cancel Button
							SetAmuletWord(95, 0xFFFF);
							delayloop(30);
							AmuletWellMeasurementMenu_cursor = 0;
							UpdateSpectrumText();
							SetAmuletWord(116, 0xFFFF); // Show Cursor text
						}
						break;

					case 4: // Wipe
						// Process Wipe Spectra
						AmuletWellMeasureMenu_wellWipe.WellWipeID = 0;
						AmuletWellMeasureMenu_wellWipe.CreatedOn = 0;
						AmuletWellMeasureMenu_wellWipe.Comment[0] = 0;
						AmuletWellMeasureMenu_wellWipe.InactiveReason[0] = 0;
						AmuletWellMeasureMenu_wellWipe.Efficiency = Mca_getFullEfficiency();
						DB_SPEC_MEAS_to_DB_SPEC(&AmuletWellMeasureMenu_wellWipe.Spectrum, &spec_meas);
						locationIndex = GetSelectWipeLocationIndex();
						selectedWipeLocation = SelectedWipeLocations();
						DB_CopyWipeLocation(&AmuletWellMeasureMenu_wellWipe.WipeLocation, &(selectedWipeLocation[locationIndex]));
						memcpy(&AmuletWellMeasureMenu_wellWipe.Background, Mca_getBackgroundMirror(), sizeof(BACKGND));
						for(index=0; index<10; index++){
							if(AmuletWellMeasureMenu_wellWipe.WipeLocation.NuclideID[index] >= 0){
								wipeNuclide = GetWipeNuclide(AmuletWellMeasureMenu_wellWipe.WipeLocation.NuclideID[index]);
								if(wipeNuclide == NULL){
									AmuletWellMeasureMenu_wellWipe.WellWipeNuclide[index].WellWipeNuclideID = -1;
									Amulet_DisplayError("Wipe Error", "Unable to find Wipe Nuclide", TRUE);
									//printf("Unable to find Wipe Nuclide\n");
								}else{
									memcpy(&(AmuletWellMeasureMenu_wellWipe.WellWipeNuclide[index]), wipeNuclide, sizeof(WELLWIPENUCLIDE));
								}
							}else{
								AmuletWellMeasureMenu_wellWipe.WellWipeNuclide[index].WellWipeNuclideID = -1;
							}
						}

						AmuletWellMeasureMenu_wellWipe.NetCPM = AmuletWellMeasureMenu_wellWipe.Spectrum.TotalCPM - AmuletWellMeasureMenu_wellWipe.Background.Spectrum.TotalCPM;
						AmuletWellMeasureMenu_wellWipe.Activity = 100.0 * AmuletWellMeasureMenu_wellWipe.NetCPM;
						AmuletWellMeasureMenu_wellWipe.Activity /= AmuletWellMeasureMenu_wellWipe.Efficiency;
						if(AmuletWellMeasureMenu_wellWipe.Activity > AmuletWellMeasureMenu_wellWipe.WipeLocation.Threshold){
							AmuletWellMeasureMenu_wellWipe.ExceedsThreshold = 1;
							AmuletWellMeasureMenu_wellWipe.OverallHighActivity = 1;
						}else{
							AmuletWellMeasureMenu_wellWipe.ExceedsThreshold = 0;
							AmuletWellMeasureMenu_wellWipe.OverallHighActivity = 0;
						}

						Mca_calcPeaks(&AmuletWellMeasureMenu_wellWipe.Spectrum, AmuletWellMeasureMenu_wellWipe.Peaks);
						Mca_matchWipePeaks(AmuletWellMeasureMenu_wellWipe.Peaks, AmuletWellMeasureMenu_wellWipe.WipeLocation.Threshold, AmuletWellMeasureMenu_wellWipe.WellWipeNuclide, AmuletWellMeasureMenu_wellWipe.Spectrum.NumOfChannels);

						if(!AmuletWellMeasureMenu_wellWipe.OverallHighActivity){
							for(index=0; index<40; index++){
								if(AmuletWellMeasureMenu_wellWipe.Peaks[index].Energy >= 0.0){
									if(AmuletWellMeasureMenu_wellWipe.Peaks[index].ExceedsThreshold){
										AmuletWellMeasureMenu_wellWipe.OverallHighActivity = 1;
										break;
									}
								}
							}
						}
						if(AmuletWellMeasurementMenu_autoMeasure){
							//DB_CreateWellWipe(&AmuletWellMeasureMenu_wellWipe, TRUE); // Save Wipe to DB
							AmuletWellMeasureMenu_wellWipe.WellWipeID = 1; // Fake Wipe ID from saving to DB
							memcpy(&AmuletWellMeasureMenu_wellWipePCComm, &AmuletWellMeasureMenu_wellWipe, sizeof(WELLWIPE));
							AmuletWellMeasurementMenu_testID[0] = 0;
							SetAmuletBackHTML();
							return;
						}else{
							SetAmuletWord(80, 0xFFFF); // Count Time Button
							SetAmuletWord(81, 0xFFFF); // Measure Button
							SetAmuletWord(83, 0xFFFF); // Finished Message
							SetAmuletWord(84, 0xFFFF); // 10 Back Button
							SetAmuletWord(85, 0xFFFF); // 1 Back Button
							SetAmuletWord(86, 0xFFFF); // 1 Forward Button
							delayloop(30);
							SetAmuletWord(87, 0xFFFF); // 10 Forward Button
							SetAmuletWord(88, 0xFFFF); // Skip
							SetAmuletWord(89, 0xFFFF); // Save
							SetAmuletWord(90, 0xFFFF); // View
							SetAmuletWord(91, 0xFFFF); // Skip All
							SetAmuletWord(95, 0xFFFF);
							delayloop(30);
							AmuletWellMeasurementMenu_cursor = 0;
							UpdateSpectrumText();
							SetAmuletWord(116, 0xFFFF); // Show Cursor text
						}
						break;

					case 3: // Efficiency
					case 5: // Unrestricted
					case 6: // Sealed
					case 7: // Schilling Standard
					case 8: // Schilling Aliqot
					case 9: // Plasma Standard
					case 10: // Plasma Whole
					case 11: // Plasma Plasma
					case 12: // RBC Whole Standard
					case 13: // RBC Plasma Standard
					case 14: // RBC Whole Sample
					case 15: // RBC Plasma Sample
					case 16: // System Test
					case 17: // MDA Test
					case 20: // BioAssay Measure I131
					case 21: // BioAssay Measure I125
					case 22: // BioAssay Measure I123
					case 23: // BioAssay
					case 24: // TU Leg 1
					case 25: // TU Leg 2
					case 26: // TU Neck 1
					case 27: // TU Neck 2
					case 28: // Dose 1 Value1
					case 29: // Dose 1 Value2
					case 30: // Dose 2 Value1
					case 31: // Dose 2 Value2
					case 32: // Dose 3 Value1
					case 33: // Dose 3 Value2
					case 34: // Dose 4 Value1
					case 35: // Dose 4 Value2
					case 36: // Dose 5 Value1
					case 37: // Dose 5 Value2
					case 38: // RBC Survival Value1
					case 39: // RBC Survival Value2
						AmuletWellMeasurementMenu_cursor = 0;
						UpdateSpectrumText();
						delayloop(30);
						SetAmuletWord(116, 0xFFFF); // Show Cursor text
						SetAmuletWord(80, 0xFFFF); // Count Time Button
						SetAmuletWord(81, 0xFFFF); // Measure Button
						SetAmuletWord(83, 0xFFFF); // Finished Message
						SetAmuletWord(84, 0xFFFF); // 10 Back Button
						delayloop(30);
						SetAmuletWord(85, 0xFFFF); // 1 Back Button
						SetAmuletWord(86, 0xFFFF); // 1 Forward Button
						SetAmuletWord(87, 0xFFFF); // 10 Forward Button
						SetAmuletWord(93, 0xFFFF); // Accept Button
						SetAmuletWord(94, 0xFFFF); // Cancel Button
						SetAmuletWord(95, 0xFFFF);
						break;

					case 18:
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellChiTestMenu_test.WellChiSquareSample[currentRepIndex].Spectrum), &spec_meas);
						Mca_getROIChannels(AmuletWellChiTestMenu_test.StartROI, AmuletWellChiTestMenu_test.EndROI, &calcStart, &calcEnd, NULL);
						acc = 0;
						for(index=calcStart; index<=calcEnd; index++) acc+= AmuletWellChiTestMenu_test.WellChiSquareSample[currentRepIndex].Spectrum.DecompressedSpectra[index];
						AmuletWellChiTestMenu_test.WellChiSquareSample[currentRepIndex].ROICounts = acc;
						currentRepIndex++;
						if(currentRepIndex<AmuletWellMeasurementMenu_reps){
							delayloop(100);
							SetAmuletWord(109, 0xFFFF); // Hide ROI CPM
							sprintf(acMsg, "%ld of %ld", currentRepIndex + 1, AmuletWellMeasurementMenu_reps);
							SetAmuletString(114, acMsg);
							SetAmuletWord(110, 0xFFFF); // Display Chi-Square count
							m_iPhase = PHASE_WELLMEASURE_REP_START;
						}else{
							currentRepIndex = 0;
							SetAmuletWord(111, 0xFFFF); // DISAPPEAR StopChi, Show Measure Chi and Finished label
							AmuletWellMeasurementMenu_cursor = 0;
							UpdateSpectrumText();
							delayloop(30);
							SetAmuletWord(116, 0xFFFF); // Show Cursor text
							SetAmuletWord(82, 0xFFFF);  // Show Measure Chi
							SetAmuletWord(83, 0xFFFF); // Finished Message
							SetAmuletWord(84, 0xFFFF); // 10 Back Button
							SetAmuletWord(85, 0xFFFF); // 1 Back Button
							delayloop(30);
							SetAmuletWord(86, 0xFFFF); // 1 Forward Button
							SetAmuletWord(87, 0xFFFF); // 10 Forward Button
							SetAmuletWord(93, 0xFFFF); // Accept Button
							SetAmuletWord(94, 0xFFFF); // Cancel Button
							SetAmuletWord(95, 0xFFFF);
						}
						break;

					case 19:
						/*// Process spectrum
						Mca_getPeaks(&(peak[0]), &dummynp1, FALSE);
						//Modify
						Mca_dedupPeaks(peak, dummynp1, 256);

						maxcounts = 0.0;
						for(index=0; index<40; index++){
							if((peak[index].height>0) && (peak[index].centroid>0)){
								if(peak[index].height > maxcounts) maxcounts = peak[index].height;
							}
						}

						for(index=0; index<40; index++){
							if((peak[index].height>0) && (peak[index].centroid>0)){
								if(peak[index].height == maxcounts){
									peak[index].height = 0.0;
									peak[index].centroid = 0.0;
									break;
								}
							}
						}

						maxcounts = 0.0;
						for(index=0; index<40; index++){
							if((peak[index].height>0) && (peak[index].centroid>0)){
								if(peak[index].height > maxcounts) maxcounts = peak[index].height;
							}
						}

						for(index=0; index<40; index++){
							if((peak[index].height>0) && (peak[index].centroid>0)){
								if(peak[index].height == maxcounts){
									AmuletWellStabilityTestMenu_peak[currentRepIndex] = Mca_convertChToEnergy(peak[index].centroid, NULL);
									AmuletWellStabilityTestMenu_stamp[currentRepIndex] = clock_time;
									currentRepIndex++;
									break;
								}
							}
						}

						if(currentRepIndex<AmuletWellMeasurementMenu_reps){
							delayloop(100);
							SetAmuletWord(109, 0xFFFF); // Hide ROI CPM
							m_iPhase = PHASE_WELLMEASURE_START;
						}else{
							m_iPhase = PHASE_WELLMEASURE_ACCEPT;
							//currentRepIndex = 0;
							//SetAmuletWord(111, 0xFFFF); // DISAPPEAR StopChi, Show Measure Chi and Finished label
							//SetAmuletWord(106, 0xFFFF); // Show Accept and Cancel Button
						}*/
						break;
				}
			}else{
				m_iPhase = PHASE_WELLMEASURE_CREATE512GRAPH;
			}
			break;

		case PHASE_WELLMEASURE_STOP:
			beep_amulet();
			if(m_ucSetWellMeasure == 18){
				beep_amulet();
				currentRepIndex = 0;
				AmuletWellChiTestMenu_finished = FALSE;
				for(index=0;index<20;index++){
					AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.SpectraID = 0;
					AmuletWellChiTestMenu_test.WellChiSquareSample[index].Spectrum.MeasuredOn = 0;
				}
				SetAmuletString(114, "");
				SetAmuletWord(110, 0xFFFF);
				SetAmuletWord(82, 0xFFFF);  // Show Measure Chi
				SetAmuletWord(94, 0xFFFF); // Cancel Button
				SetAmuletWord(95, 0xFFFF);
				m_iPhase = PHASE_WELLMEASURE_WAIT;
			}else if(m_ucSetWellMeasure == 19){
				AmuletWellMeasurementMenu_partialreps = currentRepIndex;
				currentRepIndex = 0;
				AmuletWellStabilityTestMenu_finished = FALSE;
				//SetAmuletWord(111, 0xFFFF); // DISAPPEAR StopChi, Show Measure Chi and Finished label
				//SetAmuletWord(106, 0xFFFF); // Show Accept and Cancel Button
				AmuletWellMeasurementMenu_cursor = 0;
				UpdateSpectrumText();
				SetAmuletWord(116, 0xFFFF); // Show Cursor text

				SetAmuletWord(82, 0xFFFF);
				SetAmuletWord(83, 0xFFFF);
				//SetAmuletWord(93, 0xFFFF);
				SetAmuletWord(94, 0xFFFF);
				SetAmuletWord(95, 0xFFFF);
				m_iPhase = PHASE_WELLMEASURE_WAIT;
			}else{
				if((m_ucSetWellMeasure == 4) && AmuletWellMeasurementMenu_autoMeasure){
					AmuletWellMeasurementMenu_testID[0] = 0;
					AmuletWellMeasureMenu_wellWipePCComm.WellWipeID = -1;
					SetAmuletBackHTML();
					return;
				}else{
					readspectrum = FALSE;
					spec_meas.done_flag = TRUE;
					m_iPhase = PHASE_WELLMEASURE_CREATE512GRAPH;
				}
			}
			break;

		case PHASE_WELLMEASURE_WAITFOREVER:
			break;

		case PHASE_WELLMEASURE_ACCEPT:
			beep_amulet();
			switch(m_ucSetWellMeasure){
				case 1:
					DB_SPEC_MEAS_to_DB_SPEC(&AmuletWellGeneralAnalysis_spectrum, &spec_meas);
					AmuletWellGeneralAnalysis_generalNuclideID = generalNuclideID;
					AmuletWellGeneralAnalysis_startEV = startEV[0];
					AmuletWellGeneralAnalysis_endEV = endEV[0];
					SetAmuletHTML(AmuletHTMLIndex[WELLGENERALANALYSIS_HTM]);
					PushPageStack(AmuletHTMLIndex[WELLGENERALANALYSIS_HTM]);
					return;

				case 2:
					AmuletWellSchillingMenu_backgroundMeasured = TRUE;
					AmuletWellPlasmaMenu_backgroundMeasured = TRUE;
					AmuletWellRBCMenu_backgroundMeasured = TRUE;
					AmuletWellSetupBioAssayMeasureEfficiencyMenu_backgroundMeasured = TRUE;
					AmuletWellBioAssay_backgroundMeasured = TRUE;
					Mca_saveToBackgroundMirror();
					break;

				case 3:
					DB_SPEC_MEAS_to_DB_SPEC(&AmuletWellMeasureEfficiency_spectrum, &spec_meas);
					break;

				case 7: // Schilling Standard
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellSchillingMenu_test.SchillingStandard_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellSchillingMenu_test.StartROI, AmuletWellSchillingMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellSchillingMenu_test.SchillingStandard_Spectrum.AutoCal);
					AmuletWellSchillingMenu_test.StandardROICounts = Mca_getROICounts(AmuletWellSchillingMenu_test.SchillingStandard_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellSchillingMenu_test.StandardROICPM = AmuletWellSchillingMenu_test.StandardROICounts;
					AmuletWellSchillingMenu_test.StandardROICPM /= AmuletWellSchillingMenu_test.SchillingStandard_Spectrum.LiveTime;
					AmuletWellSchillingMenu_test.StandardROICPM *= 60.0;
					break;

				case 8: // Schilling Aliqot
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellSchillingMenu_test.StartROI, AmuletWellSchillingMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum.AutoCal);
					AmuletWellSchillingMenu_test.AliquotROICounts = Mca_getROICounts(AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellSchillingMenu_test.AliquotROICPM = AmuletWellSchillingMenu_test.AliquotROICounts;
					AmuletWellSchillingMenu_test.AliquotROICPM /= AmuletWellSchillingMenu_test.SchillingAliquot_Spectrum.LiveTime;
					AmuletWellSchillingMenu_test.AliquotROICPM *= 60.0;
					break;

				case 9: // Plasma Standard
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellPlasmaMenu_test.Standard_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellPlasmaMenu_test.StartROI, AmuletWellPlasmaMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellPlasmaMenu_test.Standard_Spectrum.AutoCal);
					AmuletWellPlasmaMenu_test.StandardROICounts = Mca_getROICounts(AmuletWellPlasmaMenu_test.Standard_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellPlasmaMenu_test.StandardROICPM = AmuletWellPlasmaMenu_test.StandardROICounts;
					AmuletWellPlasmaMenu_test.StandardROICPM /= AmuletWellPlasmaMenu_test.Standard_Spectrum.LiveTime;
					AmuletWellPlasmaMenu_test.StandardROICPM *= 60.0;
					break;

				case 10: // Plasma Whole Blood
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellPlasmaMenu_test.WholeBlood_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellPlasmaMenu_test.StartROI, AmuletWellPlasmaMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellPlasmaMenu_test.WholeBlood_Spectrum.AutoCal);
					AmuletWellPlasmaMenu_test.WholeBloodROICounts = Mca_getROICounts(AmuletWellPlasmaMenu_test.WholeBlood_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellPlasmaMenu_test.WholeBloodROICPM = AmuletWellPlasmaMenu_test.WholeBloodROICounts;
					AmuletWellPlasmaMenu_test.WholeBloodROICPM /= AmuletWellPlasmaMenu_test.WholeBlood_Spectrum.LiveTime;
					AmuletWellPlasmaMenu_test.WholeBloodROICPM *= 60.0;
					break;

				case 11: // Plasma Plasma
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellPlasmaMenu_test.Plasma_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellPlasmaMenu_test.StartROI, AmuletWellPlasmaMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellPlasmaMenu_test.Plasma_Spectrum.AutoCal);
					AmuletWellPlasmaMenu_test.PlasmaROICounts = Mca_getROICounts(AmuletWellPlasmaMenu_test.Plasma_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellPlasmaMenu_test.PlasmaROICPM = AmuletWellPlasmaMenu_test.PlasmaROICounts;
					AmuletWellPlasmaMenu_test.PlasmaROICPM /= AmuletWellPlasmaMenu_test.Plasma_Spectrum.LiveTime;
					AmuletWellPlasmaMenu_test.PlasmaROICPM *= 60.0;
					break;

				case 12: // RBC Whole Standard
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellRBCMenu_test.StartROI, AmuletWellRBCMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum.AutoCal);
					AmuletWellRBCMenu_test.WholeBloodStandardROICounts = Mca_getROICounts(AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellRBCMenu_test.WholeBloodStandardROICPM = AmuletWellRBCMenu_test.WholeBloodStandardROICounts;
					AmuletWellRBCMenu_test.WholeBloodStandardROICPM /= AmuletWellRBCMenu_test.WholeBloodStandard_Spectrum.LiveTime;
					AmuletWellRBCMenu_test.WholeBloodStandardROICPM *= 60.0;
					break;

				case 13: // RBC Plasma Standard
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellRBCMenu_test.PlasmaStandard_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellRBCMenu_test.StartROI, AmuletWellRBCMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellRBCMenu_test.PlasmaStandard_Spectrum.AutoCal);
					AmuletWellRBCMenu_test.PlasmaStandardROICounts = Mca_getROICounts(AmuletWellRBCMenu_test.PlasmaStandard_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellRBCMenu_test.PlasmaStandardROICPM = AmuletWellRBCMenu_test.PlasmaStandardROICounts;
					AmuletWellRBCMenu_test.PlasmaStandardROICPM /= AmuletWellRBCMenu_test.PlasmaStandard_Spectrum.LiveTime;
					AmuletWellRBCMenu_test.PlasmaStandardROICPM *= 60.0;
					break;

				case 14: // RBC Whole Sample
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellRBCMenu_test.WholeBloodSample_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellRBCMenu_test.StartROI, AmuletWellRBCMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellRBCMenu_test.WholeBloodSample_Spectrum.AutoCal);
					AmuletWellRBCMenu_test.WholeBloodSampleROICounts = Mca_getROICounts(AmuletWellRBCMenu_test.WholeBloodSample_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellRBCMenu_test.WholeBloodSampleROICPM = AmuletWellRBCMenu_test.WholeBloodSampleROICounts;
					AmuletWellRBCMenu_test.WholeBloodSampleROICPM /= AmuletWellRBCMenu_test.WholeBloodSample_Spectrum.LiveTime;
					AmuletWellRBCMenu_test.WholeBloodSampleROICPM *= 60.0;
					break;

				case 15: // RBC Plasma Sample
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellRBCMenu_test.PlasmaSample_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellRBCMenu_test.StartROI, AmuletWellRBCMenu_test.EndROI, &calcStart, &calcEnd, &AmuletWellRBCMenu_test.PlasmaSample_Spectrum.AutoCal);
					AmuletWellRBCMenu_test.PlasmaSampleROICounts = Mca_getROICounts(AmuletWellRBCMenu_test.PlasmaSample_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellRBCMenu_test.PlasmaSampleROICPM = AmuletWellRBCMenu_test.PlasmaSampleROICounts;
					AmuletWellRBCMenu_test.PlasmaSampleROICPM /= AmuletWellRBCMenu_test.PlasmaSample_Spectrum.LiveTime;
					AmuletWellRBCMenu_test.PlasmaSampleROICPM *= 60.0;
					break;

				case 16: // System Test
					AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID = 0;
					DB_SPEC_MEAS_to_DB_SPEC(&AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum, &spec_meas);
					strcpy(AmuletWellSystemTestAnalysisMenu_systemTest.NuclideName, "Cs137");
					AmuletWellMainScreenMenu_getWellStandardSN(AmuletWellSystemTestAnalysisMenu_systemTest.SerialNumber);
					AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationActivity = AmuletWellMainScreenMenu_getWellStandardActivity();
					AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationDate = AmuletWellMainScreenMenu_getWellStandardDate();
					nucIndex = NuclideData_getIndexFromName("Cs137");
					AmuletWellSystemTestAnalysisMenu_systemTest.Energy = NuclideData_getEffectivePrimary(nucIndex);
					if(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) Mca_getEndPoints(AmuletWellSystemTestAnalysisMenu_systemTest.Energy, &AmuletWellSystemTestAnalysisMenu_systemTest.StartROI, &AmuletWellSystemTestAnalysisMenu_systemTest.EndROI, TRUE);
					else Mca_getEndPoints(AmuletWellSystemTestAnalysisMenu_systemTest.Energy, &AmuletWellSystemTestAnalysisMenu_systemTest.StartROI, &AmuletWellSystemTestAnalysisMenu_systemTest.EndROI, FALSE);
					if(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest == DET_DRILLEDPROBE700_AS_WELL) AmuletWellSystemTestAnalysisMenu_systemTest.Efficiency = NuclideData_getEffectiveEff(nucIndex, DET_WELL700);
					else if(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest == DET_DRILLEDPROBE700_AS_PROBE) AmuletWellSystemTestAnalysisMenu_systemTest.Efficiency = NuclideData_getEffectiveEff(nucIndex, DET_PROBE700);
					else AmuletWellSystemTestAnalysisMenu_systemTest.Efficiency = NuclideData_getEffectiveEff(nucIndex, AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest);
					GetExtendedNuclideInfo(nucIndex, &nuc, acMsg);
					AmuletWellSystemTestAnalysisMenu_systemTest.Halflife = nuc.halflife;
					AmuletWellSystemTestAnalysisMenu_systemTest.HalflifeUnit = nuc.hlunit;
					AmuletWellSystemTestAnalysisMenu_systemTest.PredictedActivity = nucdecay(AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationActivity, AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationDate, AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.MeasuredOn, AmuletWellSystemTestAnalysisMenu_systemTest.Halflife, AmuletWellSystemTestAnalysisMenu_systemTest.HalflifeUnit);
					Mca_getROIChannels(AmuletWellSystemTestAnalysisMenu_systemTest.StartROI, AmuletWellSystemTestAnalysisMenu_systemTest.EndROI, &startCh, &endCh, &AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.AutoCal);
					AmuletWellSystemTestAnalysisMenu_systemTest.ROICounts = 0;
					for(index=startCh; index<=endCh; index++) AmuletWellSystemTestAnalysisMenu_systemTest.ROICounts += AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.DecompressedSpectra[index];
					AmuletWellSystemTestAnalysisMenu_systemTest.ROICPM = AmuletWellSystemTestAnalysisMenu_systemTest.ROICounts;
					AmuletWellSystemTestAnalysisMenu_systemTest.ROICPM /= AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.LiveTime;
					AmuletWellSystemTestAnalysisMenu_systemTest.ROICPM *= 60.0;
					AmuletWellSystemTestAnalysisMenu_systemTest.BackgroundROICounts = Mca_getROIBackgroundCounts(startCh, endCh, NULL);
					AmuletWellSystemTestAnalysisMenu_systemTest.BackgroundROICPM = Mca_getROIBackgroundCPM(startCh, endCh, NULL);
					AmuletWellSystemTestAnalysisMenu_systemTest.NetROICPM = AmuletWellSystemTestAnalysisMenu_systemTest.ROICPM - AmuletWellSystemTestAnalysisMenu_systemTest.BackgroundROICPM;
					AmuletWellSystemTestAnalysisMenu_systemTest.Activity = AmuletWellSystemTestAnalysisMenu_systemTest.NetROICPM;
					AmuletWellSystemTestAnalysisMenu_systemTest.Activity /= AmuletWellSystemTestAnalysisMenu_systemTest.Efficiency;
					AmuletWellSystemTestAnalysisMenu_systemTest.Activity *= 100.0;
					AmuletWellSystemTestAnalysisMenu_systemTest.Activity = Mca_convertDpmToCi(AmuletWellSystemTestAnalysisMenu_systemTest.Activity);
					AmuletWellSystemTestAnalysisMenu_systemTest.Deviation = AmuletWellSystemTestAnalysisMenu_systemTest.Activity - AmuletWellSystemTestAnalysisMenu_systemTest.PredictedActivity;
					AmuletWellSystemTestAnalysisMenu_systemTest.Deviation /= AmuletWellSystemTestAnalysisMenu_systemTest.PredictedActivity;
					AmuletWellSystemTestAnalysisMenu_systemTest.Deviation *= 100.0;
					AmuletWellSystemTestAnalysisMenu_systemTest.WellBackgroundID = ((BACKGND *)Mca_getBackgroundMirror())->WellBackgroundID;
					AmuletWellSystemTestAnalysisMenu_systemTest.CreatedOn = 0;
					AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason[0] = 0;
					AmuletWellSystemTestAnalysisMenu_systemTest.Inactive = FALSE;
#ifdef TIMES1000
					for(index=0; index<TIMES1000; index++){
#endif
						DB_CreateWellSystemTest(&AmuletWellSystemTestAnalysisMenu_systemTest, TRUE);
#ifdef TIMES1000
						AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.MeasuredOn -= 86400;
					}
#endif
					AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID = 0;
					PopPageStack();
					SetAmuletHTML(AmuletHTMLIndex[WELLSYSTEMTESTANALYSIS_HTM]);
					PushPageStack(AmuletHTMLIndex[WELLSYSTEMTESTANALYSIS_HTM]);
					return;

				case 17:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellMDATestMenu_mda.Spectrum), &spec_meas);
					break;

				case 18:
					AmuletWellChiTestMenu_finished = TRUE;
					break;

				case 19:
					/*if(AmuletWellMeasurementMenu_partialreps!=0) AmuletWellStabilityTestMenu_reps = AmuletWellMeasurementMenu_partialreps;
					AmuletWellStabilityTestMenu_finished = TRUE;*/
					break;

				case 20:
					for(index=0; index<10; index++){
						energyPeak[index].peakindex = -1;
						energyPeak[index].startCh = -1;
						energyPeak[index].endCh = -1;
						energyPeak[index].found = FALSE;
					}
					nucIndex = NuclideData_getIndexFromName("I 131");
					energyPeak[0].energy = NuclideData_getEffectivePrimary(nucIndex);
					if(Mca_installedDetector == DET_DRILLEDPROBE700) Mca_findPeak(spec_meas.spectrum, energyPeak, TRUE, NULL);
					else Mca_findPeak(spec_meas.spectrum, energyPeak, FALSE, NULL);
					if(energyPeak[0].found){
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131PrimaryEnergy = energyPeak[0].energy;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131StartEV = I131Lower_keV;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131EndEV = I131Upper_keV;
						Mca_getROIChannels(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131StartEV, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131EndEV, &(energyPeak[0].startCh), &(energyPeak[0].endCh), NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131BackgroundID = ((BACKGND *) Mca_getBackgroundMirror())->WellBackgroundID;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131SpectraID = -1;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131Spectrum), &spec_meas);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICounts = Mca_getROICounts(spec_meas.spectrum, energyPeak[0].startCh, energyPeak[0].endCh);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICounts;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM /= spec_meas.live_time;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM *= 60.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131BackgroundROICounts = Mca_getROIBackgroundCounts(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131BackgroundROICPM = Mca_getROIBackgroundCPM(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131NetROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131ROICPM - AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131BackgroundROICPM;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125StartEV = I125Lower_keV;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125EndEV = I125Upper_keV;
						Mca_getROIChannels(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125StartEV, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125EndEV, &(energyPeak[0].startCh), &(energyPeak[0].endCh), NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125ROICounts = Mca_getROICounts(spec_meas.spectrum, energyPeak[0].startCh, energyPeak[0].endCh);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125ROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125ROICounts;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125ROICPM /= spec_meas.live_time;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125ROICPM *= 60.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125BackgroundROICounts = Mca_getROIBackgroundCounts(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125BackgroundROICPM = Mca_getROIBackgroundCPM(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125NetROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125ROICPM - AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I131I125BackgroundROICPM;
					}else{
						Amulet_DisplayError("BioAssay Measurement", "Unable to detect I131 Primary Peak", TRUE);
						return;
					}
					break;

				case 21:
					for(index=0; index<10; index++){
						energyPeak[index].peakindex = -1;
						energyPeak[index].startCh = -1;
						energyPeak[index].endCh = -1;
						energyPeak[index].found = FALSE;
					}
					nucIndex = NuclideData_getIndexFromName("I 125");
					energyPeak[0].energy = NuclideData_getEffectivePrimary(nucIndex);
					if(Mca_installedDetector == DET_DRILLEDPROBE700) Mca_findPeak(spec_meas.spectrum, energyPeak, TRUE, NULL);
					else Mca_findPeak(spec_meas.spectrum, energyPeak, FALSE, NULL);
					if(energyPeak[0].found){
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125PrimaryEnergy = energyPeak[0].energy;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125StartEV = I125Lower_keV;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125EndEV = I125Upper_keV;
						Mca_getROIChannels(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125StartEV, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125EndEV, &(energyPeak[0].startCh), &(energyPeak[0].endCh), NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125BackgroundID = ((BACKGND *) Mca_getBackgroundMirror())->WellBackgroundID;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125SpectraID = -1;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125Spectrum), &spec_meas);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICounts = Mca_getROICounts(spec_meas.spectrum, energyPeak[0].startCh, energyPeak[0].endCh);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICounts;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM /= spec_meas.live_time;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM *= 60.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125BackgroundROICounts = Mca_getROIBackgroundCounts(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125BackgroundROICPM = Mca_getROIBackgroundCPM(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125NetROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125ROICPM - AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I125BackgroundROICPM;
					}else{
						Amulet_DisplayError("BioAssay Measurement", "Unable to detect I125 Primary Peak", TRUE);
						return;
					}
					break;

				case 22:
					for(index=0; index<10; index++){
						energyPeak[index].peakindex = -1;
						energyPeak[index].startCh = -1;
						energyPeak[index].endCh = -1;
						energyPeak[index].found = FALSE;
					}
					nucIndex = NuclideData_getIndexFromName("I 123");
					energyPeak[0].energy = NuclideData_getEffectivePrimary(nucIndex);
					if(Mca_installedDetector == DET_DRILLEDPROBE700) Mca_findPeak(spec_meas.spectrum, energyPeak, TRUE, NULL);
					else Mca_findPeak(spec_meas.spectrum, energyPeak, FALSE, NULL);
					if(energyPeak[0].found){
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123PrimaryEnergy = energyPeak[0].energy;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123StartEV = I123Lower_keV;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123EndEV = I123Upper_keV;
						Mca_getROIChannels(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123StartEV, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123EndEV, &(energyPeak[0].startCh), &(energyPeak[0].endCh), NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123BackgroundID = ((BACKGND *) Mca_getBackgroundMirror())->WellBackgroundID;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123SpectraID = -1;
						DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123Spectrum), &spec_meas);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICounts = Mca_getROICounts(spec_meas.spectrum, energyPeak[0].startCh, energyPeak[0].endCh);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICounts;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM /= spec_meas.live_time;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM *= 60.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123BackgroundROICounts = Mca_getROIBackgroundCounts(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123BackgroundROICPM = Mca_getROIBackgroundCPM(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123NetROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123ROICPM - AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123BackgroundROICPM;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125StartEV = I125Lower_keV;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125EndEV = I125Upper_keV;
						Mca_getROIChannels(AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125StartEV, AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125EndEV, &(energyPeak[0].startCh), &(energyPeak[0].endCh), NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125ROICounts = Mca_getROICounts(spec_meas.spectrum, energyPeak[0].startCh, energyPeak[0].endCh);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125ROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125ROICounts;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125ROICPM /= spec_meas.live_time;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125ROICPM *= 60.0;
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125BackgroundROICounts = Mca_getROIBackgroundCounts(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125BackgroundROICPM = Mca_getROIBackgroundCPM(energyPeak[0].startCh, energyPeak[0].endCh, NULL);
						AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125NetROICPM = AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125ROICPM - AmuletWellSetupBioAssayMeasureEfficiencyMenu_eff.I123I125BackgroundROICPM;
					}else{
						Amulet_DisplayError("BioAssay Measurement", "Unable to detect I123 Primary Peak", TRUE);
						return;
					}
					break;

				case 23:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellBioAssay_test.Spectrum), &spec_meas);
					break;

				case 24: // TU Leg 1
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum),&spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasurePatient_test.StartROI, AmuletWellThyroidUptakeMeasurePatient_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasurePatient.Leg1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICounts;
					AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM /= AmuletWellThyroidUptakeMeasurePatient.Leg1_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM;
					else{
						AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM /= 2.0;
					}
#ifdef ZERO_TU_PATIENT_BACKGROUND
					AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM = 0;
					AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = 0;
#endif
					break;

				case 25: // TU Leg 2
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum),&spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasurePatient_test.StartROI, AmuletWellThyroidUptakeMeasurePatient_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM = AmuletWellThyroidUptakeMeasurePatient.Leg2ROICounts;
					AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM /= AmuletWellThyroidUptakeMeasurePatient.Leg2_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM;
					else{
						AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Leg1ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM /= 2.0;
					}
#ifdef ZERO_TU_PATIENT_BACKGROUND
					AmuletWellThyroidUptakeMeasurePatient.Leg2ROICPM = 0;
					AmuletWellThyroidUptakeMeasurePatient.LegROIAverageCPM = 0;
#endif
					break;

				case 26: // TU Neck 1
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum),&spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasurePatient_test.StartROI, AmuletWellThyroidUptakeMeasurePatient_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICounts;
					AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM /= AmuletWellThyroidUptakeMeasurePatient.Neck1_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
					else{
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM /= 2.0;
					}
					break;

				case 27: // TU Neck 2
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum),&spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasurePatient_test.StartROI, AmuletWellThyroidUptakeMeasurePatient_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICounts;
					AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM /= AmuletWellThyroidUptakeMeasurePatient.Neck2_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM == -1) AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
					else{
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM = AmuletWellThyroidUptakeMeasurePatient.Neck1ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM += AmuletWellThyroidUptakeMeasurePatient.Neck2ROICPM;
						AmuletWellThyroidUptakeMeasurePatient.NeckROIAverageCPM /= 2.0;
					}
					break;

				case 28:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 29:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[0].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 30:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 31:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[1].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 32:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 33:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[2].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 34:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 35:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[3].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 36:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 37:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellThyroidUptakeMeasureDose_test.StartROI, AmuletWellThyroidUptakeMeasureDose_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICounts = Mca_getROICounts(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICounts;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM /= AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2_Spectrum.LiveTime;
					AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM *= 60.0;
					if(AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM == -1) AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM;
					else{
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM = AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose1ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM += AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].Dose2ROICPM;
						AmuletWellThyroidUptakeMeasureDose_total.SingleMeasurement[4].DoseROIAverageCPM /= 2.0;
					}
					break;

				case 38:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellRBCSurvivalMeasureMenu_test.StartROI, AmuletWellRBCSurvivalMeasureMenu_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICounts = Mca_getROICounts(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICounts;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM /= AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1_Spectrum.LiveTime;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM *= 60.0;
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM == -1) AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM;
					else{
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM;
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM += AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM;
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM /= 2.0;
					}
					break;

				case 39:
					DB_SPEC_MEAS_to_DB_SPEC(&(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum), &spec_meas);
					Mca_getROIChannels(AmuletWellRBCSurvivalMeasureMenu_test.StartROI, AmuletWellRBCSurvivalMeasureMenu_test.EndROI, &calcStart, &calcEnd, NULL);
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICounts = Mca_getROICounts(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.DecompressedSpectra, calcStart, calcEnd);
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICounts;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM /= AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2_Spectrum.LiveTime;
					AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM *= 60.0;
					if(AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM == -1) AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM;
					else{
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM = AmuletWellRBCSurvivalMeasureMenu_measurement.Sample1ROICPM;
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM += AmuletWellRBCSurvivalMeasureMenu_measurement.Sample2ROICPM;
						AmuletWellRBCSurvivalMeasureMenu_measurement.SampleROIAverageCPM /= 2.0;
					}
					break;
			}

			SetAmuletBackHTML();
			break;

		case PHASE_WELLMEASURE_WIPESKIP:
			beep_amulet();
			IncSelectWipeLocationIndex();
			if(GetSelectWipeLocationIndex() == SelectedWipeLocationCount()){
				SetAmuletBackHTML();
			}else{
				SetAmuletString(110, "");
				SetAmuletWord(80, 0);
				SetAmuletWord(81, 0);
				SetAmuletWord(82, 0);
				SetAmuletWord(83, 0);
				SetAmuletWord(84, 0);
				SetAmuletWord(85, 0);
				SetAmuletWord(86, 0);
				SetAmuletWord(87, 0);
				SetAmuletWord(88, 0);
				SetAmuletWord(89, 0);
				SetAmuletWord(90, 0);
				SetAmuletWord(91, 0);
				SetAmuletWord(92, 0);
				SetAmuletWord(93, 0);
				SetAmuletWord(94, 0);
				SetAmuletWord(120, 0xFFFF);
				SetAmuletWord(119, 0xFFFF); // Clear Graph
				SetAmuletFillRect(289, 75, 511, 255, 0xFFFFFF, 1);
				delayloop(50);
				m_ucClear = 18;
				m_iPhase = PHASE_WELLMEASURE_PRE_INIT;
			}
			break;

		case PHASE_WELLMEASURE_WIPESAVE:
			beep_amulet();
#ifdef TIMES1000
			for(index=0; index<TIMES1000; index++){
#endif
				DB_CreateWellWipe(&AmuletWellMeasureMenu_wellWipe, TRUE);
#ifdef TIMES1000
				AmuletWellMeasureMenu_wellWipe.Spectrum.MeasuredOn -= 86400;
			}
#endif
			IncSelectWipeLocationIndex();
			if(GetSelectWipeLocationIndex() == SelectedWipeLocationCount()){
				SetAmuletBackHTML();
			}else{
				SetAmuletString(110, "");
				SetAmuletWord(80, 0);
				SetAmuletWord(81, 0);
				SetAmuletWord(82, 0);
				SetAmuletWord(83, 0);
				SetAmuletWord(84, 0);
				SetAmuletWord(85, 0);
				SetAmuletWord(86, 0);
				SetAmuletWord(87, 0);
				SetAmuletWord(88, 0);
				SetAmuletWord(89, 0);
				SetAmuletWord(90, 0);
				SetAmuletWord(91, 0);
				SetAmuletWord(92, 0);
				SetAmuletWord(93, 0);
				SetAmuletWord(94, 0);
				SetAmuletWord(120, 0xFFFF);
				SetAmuletWord(119, 0xFFFF); // Clear Graph
				SetAmuletFillRect(289, 75, 511, 255, 0xFFFFFF, 1);
				delayloop(50);
				m_ucClear = 18;
				m_iPhase = PHASE_WELLMEASURE_PRE_INIT;
			}
			break;

		case PHASE_WELLMEASURE_WIPEVIEW:
			beep_amulet();
			memcpy(&AmuletWellWipeReportMenu_wellWipe, &AmuletWellMeasureMenu_wellWipe, sizeof(WELLWIPE));
			SetAmuletHTML(AmuletHTMLIndex[WELLWIPEREPORT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLWIPEREPORT_HTM]);
			return;

		case PHASE_WELLMEASURE_WIPEEND:
			break;

		case PHASE_WELLMEASURE_BACK10:
			beep_amulet();
			AmuletWellMeasurementMenu_cursor -= (10 * (spec_meas.num_of_channels / 256));
			if(AmuletWellMeasurementMenu_cursor < 0) AmuletWellMeasurementMenu_cursor = 0;
			UpdateSpectrum(display);
			m_iPhase = PHASE_WELLMEASURE_WAIT;

			break;

		case PHASE_WELLMEASURE_BACK1:
			beep_amulet();
			AmuletWellMeasurementMenu_cursor -= 1;
			if(AmuletWellMeasurementMenu_cursor < 0) AmuletWellMeasurementMenu_cursor = 0;
			UpdateSpectrum(display);
			m_iPhase = PHASE_WELLMEASURE_WAIT;
			break;

		case PHASE_WELLMEASURE_FORWARD1:
			beep_amulet();
			AmuletWellMeasurementMenu_cursor += 1;
			if(AmuletWellMeasurementMenu_cursor > spec_meas.num_of_channels - 1) AmuletWellMeasurementMenu_cursor = spec_meas.num_of_channels - 1;
			UpdateSpectrum(display);
			m_iPhase = PHASE_WELLMEASURE_WAIT;
			break;

		case PHASE_WELLMEASURE_FORWARD10:
			beep_amulet();
			AmuletWellMeasurementMenu_cursor += (10 * (spec_meas.num_of_channels / 256));
			if(AmuletWellMeasurementMenu_cursor > spec_meas.num_of_channels - 1) AmuletWellMeasurementMenu_cursor = spec_meas.num_of_channels - 1;
			UpdateSpectrum(display);
			m_iPhase = PHASE_WELLMEASURE_WAIT;
			break;

		case PHASE_WELLMEASURE_CURSOR:
			switch(spec_meas.num_of_channels){
				case 256:
					if(AmuletWellMeasurementMenu_cursor == 0) SetAmuletLine(2 * AmuletWellMeasurementMenu_cursor + 289, 75, 2 * AmuletWellMeasurementMenu_cursor + 289, 329, 0, 0x01);
					else SetAmuletLine(2 * AmuletWellMeasurementMenu_cursor + 289, 75, 2 * AmuletWellMeasurementMenu_cursor + 289, 329, 0xFF0000, 0x01);
					break;

				case 512:
					SetAmuletLine(AmuletWellMeasurementMenu_cursor + 289, 75, AmuletWellMeasurementMenu_cursor + 289, 329, 0xFF0000, 0x01);
					break;

				case 1024:
					SetAmuletLine((AmuletWellMeasurementMenu_cursor/2) + 289, 75, (AmuletWellMeasurementMenu_cursor/2) + 289, 329, 0xFF0000, 0x01);
					break;

				case 2048:
					SetAmuletLine((AmuletWellMeasurementMenu_cursor/4) + 289, 75, (AmuletWellMeasurementMenu_cursor/4) + 289, 329, 0xFF0000, 0x01);
					break;

				case 4096:
					SetAmuletLine((AmuletWellMeasurementMenu_cursor/8) + 289, 75, (AmuletWellMeasurementMenu_cursor/8) + 289, 329, 0xFF0000, 0x01);
					break;
			}
			m_iPhase = PHASE_WELLMEASURE_WAIT;
			break;
	}
}

static void LoadWipeLocationData(char *title){
	WELLWIPELOCATION *selectedWipeLocation;
	char locationName[30];
	int indexSelected, total, index;

	indexSelected = GetSelectWipeLocationIndex();
	total = SelectedWipeLocationCount();
	selectedWipeLocation = SelectedWipeLocations();

	// Load Title from Wipe Location
	strcpy(locationName, selectedWipeLocation[indexSelected].Name);
	trim(locationName);
	sprintf(title, "Wipe: %s (%d/%d)", locationName, indexSelected+1, total);

	// Load energyPeak from Wipe Location
	for(index=0; index<10; index++){
		if(selectedWipeLocation[indexSelected].NuclideID[index] >= 0){
			energyPeak[index].energy = NuclideData_getEffectivePrimary(selectedWipeLocation[indexSelected].NuclideID[index]);
		}
	}

	// Set Acquire Time from Wipe Location
	Mca_setTimeType(COUNT_LIVE);
	Mca_setAcquireTime(selectedWipeLocation[indexSelected].CountTime);
}

static void UpdateSpectrum(unsigned char *display){
	char acMsg[100];

	sprintf(acMsg, "%.2f keV", Mca_convertChToEnergy(AmuletWellMeasurementMenu_cursor, NULL));
	SetAmuletString(116, acMsg);
	sprintf(acMsg, "%lu", spec_meas.spectrum[AmuletWellMeasurementMenu_cursor]);
	SetAmuletString(117, acMsg);
	SetAmuletWord(117, 0xFFFF);
	AmuletWellMeasurementMenu_drawGraph(display, TRUE);
	while(!AmuletWellMeasurementMenu_drawGraph(display, FALSE));
}

static void UpdateSpectrumText(void){
	char acMsg[100];

	sprintf(acMsg, "%.2f keV", Mca_convertChToEnergy(AmuletWellMeasurementMenu_cursor, NULL));
	SetAmuletString(116, acMsg);
	sprintf(acMsg, "%lu", spec_meas.spectrum[AmuletWellMeasurementMenu_cursor]);
	SetAmuletString(117, acMsg);
}
