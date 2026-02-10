#define PHASE_WELLQATESTS_PRE_INIT	0
#define PHASE_WELLQATESTS_WAIT		1
#define PHASE_WELLQATESTS_SYS_TEST	2
#define PHASE_WELLQATESTS_SYS_TEST_DRILLEDPROBE_WELL	3
#define PHASE_WELLQATESTS_SYS_TEST_DRILLEDPROBE_PROBE	4

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "amulet.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern volatile char m_acTitle[51];
extern unsigned char m_ucSetWellMeasure;
extern bool AmuletWellMeasurementMenu_unlockDetector;
extern WELLSYSTEMTEST AmuletWellSystemTestAnalysisMenu_systemTest;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
time_t AmuletWellMainScreenMenu_getWellStandardDate(void);
void PushPageStack(unsigned char ucPage);
void AmuletWellMainScreenMenu_getWellStandardSN(char *SN);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);

void AmuletWellQATests_menu(void){
	time_t standardDate;
	short nucIndex;
	char serialnum[25];
	time_t backgroundstamp;
	char acMsg[100], timestring[25];


	switch(m_iPhase){
		case PHASE_WELLQATESTS_PRE_INIT:
			Mca_sendDetectorType();
			SetAmuletByte(75, 0xFF);
			if(Mca_isMultiDetector()) SetAmuletByte(76, 0xFF);
			if(Mca_installedDetector == DET_DRILLEDPROBE700) SetAmuletByte(101, 0xFF);
			else SetAmuletByte(100, 0xFF);
			m_iPhase = PHASE_WELLQATESTS_WAIT;
			break;

		case PHASE_WELLQATESTS_WAIT:
			break;

		case PHASE_WELLQATESTS_SYS_TEST:
			beep_amulet();
			standardDate = AmuletWellMainScreenMenu_getWellStandardDate();
			if(standardDate == (time_t)0){
				Amulet_DisplayError("System Test Error", "Unable to find Cs137 Test Source\nPlease fill out Test Source Setup",TRUE);
				return;
			}else{
				if(Mca_getCalibrationStatus()){
					if(Mca_getBackgroundStatus()){
						if(Mca_getBackgroundToday()){
							nucIndex = NuclideData_getIndexFromName("Cs137");
							if(NuclideData_getEffectiveEff(nucIndex, Mca_installedDetector) == 0.0){
								Amulet_DisplayError("System Test Error", "Unable to find Cs137 efficiency",TRUE);
								return;
							}else{
								if(NuclideData_getEffectivePrimary(nucIndex) == 0.0){
									Amulet_DisplayError("System Test Error", "Unable to find Cs137 Primary Peak( keV1)",TRUE);
									return;
								}else{
									AmuletWellMainScreenMenu_getWellStandardSN(serialnum);
									strcpy((char *)m_acTitle, "Cs137 S/N: ");
									strcat((char *)m_acTitle, serialnum);
									AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest = Mca_installedDetector;
									m_ucClear = 18;
									m_ucSetWellMeasure = 16;
									AmuletWellMeasurementMenu_unlockDetector = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									return;
								}
							}
						}else{
							backgroundstamp = Mca_getBackgroundStamp();
							GetExtendedTimeInfoSec(&backgroundstamp, timestring);
							strcpy(acMsg, "Background has expired\nBackground measured on ");
							strcat(acMsg, timestring);
							strcat(acMsg, "\nPlease run Background for today");

							Amulet_DisplayError("Background Error", acMsg, TRUE);
							return;
						}
					}else{
						if(Mca_existsBackground()){
							Amulet_DisplayError("Background Error", "AutoCalibration has invalidated background\nPlease run Background",TRUE);
						}else{
							Amulet_DisplayError("Background Error", "Missing Background\nPlease run Background first",TRUE);
						}
						return;
					}
				}else{
					if(Mca_existsAutoCal()){
						Amulet_DisplayError("Calibration Error", "Detector parameters have changed\nPlease perform Auto Calibration", TRUE);
					}else{
						Amulet_DisplayError("Calibration Error", "Detector has never been calibrated\nPlease perform Auto Calibration first",TRUE);
					}
					return;
				}
			}
			//break;

		case PHASE_WELLQATESTS_SYS_TEST_DRILLEDPROBE_WELL:
			beep_amulet();
			standardDate = AmuletWellMainScreenMenu_getWellStandardDate();
			if(standardDate == (time_t)0){
				Amulet_DisplayError("System Test Error", "Unable to find Cs137 Test Source\nPlease fill out Test Source Setup",TRUE);
				return;
			}else{
				if(Mca_getCalibrationStatus()){
					if(Mca_getBackgroundStatus()){
						if(Mca_getBackgroundToday()){
							nucIndex = NuclideData_getIndexFromName("Cs137");
							if(NuclideData_getEffectiveEff(nucIndex, DET_WELL700) == 0.0){
								Amulet_DisplayError("System Test Error", "Unable to find Cs137 efficiency",TRUE);
								return;
							}else{
								if(NuclideData_getEffectivePrimary(nucIndex) == 0.0){
									Amulet_DisplayError("System Test Error", "Unable to find Cs137 Primary Peak( keV1)",TRUE);
									return;
								}else{
									AmuletWellMainScreenMenu_getWellStandardSN(serialnum);
									strcpy((char *)m_acTitle, "Cs137 S/N: ");
									strcat((char *)m_acTitle, serialnum);
									AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest = DET_DRILLEDPROBE700_AS_WELL;
									m_ucClear = 18;
									m_ucSetWellMeasure = 16;
									AmuletWellMeasurementMenu_unlockDetector = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									return;
								}
							}
						}else{
							backgroundstamp = Mca_getBackgroundStamp();
							GetExtendedTimeInfoSec(&backgroundstamp, timestring);
							strcpy(acMsg, "Background has expired\nBackground measured on ");
							strcat(acMsg, timestring);
							strcat(acMsg, "\nPlease run Background for today");

							Amulet_DisplayError("Background Error", acMsg, TRUE);
							return;
						}
					}else{
						if(Mca_existsBackground()){
							Amulet_DisplayError("Background Error", "AutoCalibration has invalidated background\nPlease run Background",TRUE);
						}else{
							Amulet_DisplayError("Background Error", "Missing Background\nPlease run Background first",TRUE);
						}
						return;
					}
				}else{
					if(Mca_existsAutoCal()){
						Amulet_DisplayError("Calibration Error", "Detector parameters have changed\nPlease perform Auto Calibration", TRUE);
					}else{
						Amulet_DisplayError("Calibration Error", "Detector has never been calibrated\nPlease perform Auto Calibration first",TRUE);
					}
					return;
				}
			}
			//break;

		case PHASE_WELLQATESTS_SYS_TEST_DRILLEDPROBE_PROBE:
			beep_amulet();
			standardDate = AmuletWellMainScreenMenu_getWellStandardDate();
			if(standardDate == (time_t)0){
				Amulet_DisplayError("System Test Error", "Unable to find Cs137 Test Source\nPlease fill out Test Source Setup",TRUE);
				return;
			}else{
				if(Mca_getCalibrationStatus()){
					if(Mca_getBackgroundStatus()){
						if(Mca_getBackgroundToday()){
							nucIndex = NuclideData_getIndexFromName("Cs137");
							if(NuclideData_getEffectiveEff(nucIndex, DET_PROBE700) == 0.0){
								Amulet_DisplayError("System Test Error", "Unable to find Cs137 efficiency",TRUE);
								return;
							}else{
								if(NuclideData_getEffectivePrimary(nucIndex) == 0.0){
									Amulet_DisplayError("System Test Error", "Unable to find Cs137 Primary Peak( keV1)",TRUE);
									return;
								}else{
									AmuletWellMainScreenMenu_getWellStandardSN(serialnum);
									strcpy((char *)m_acTitle, "Cs137 S/N: ");
									strcat((char *)m_acTitle, serialnum);
									AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest = DET_DRILLEDPROBE700_AS_PROBE;
									m_ucClear = 18;
									m_ucSetWellMeasure = 16;
									AmuletWellMeasurementMenu_unlockDetector = FALSE;
									SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
									return;
								}
							}
						}else{
							backgroundstamp = Mca_getBackgroundStamp();
							GetExtendedTimeInfoSec(&backgroundstamp, timestring);
							strcpy(acMsg, "Background has expired\nBackground measured on ");
							strcat(acMsg, timestring);
							strcat(acMsg, "\nPlease run Background for today");

							Amulet_DisplayError("Background Error", acMsg, TRUE);
							return;
						}
					}else{
						if(Mca_existsBackground()){
							Amulet_DisplayError("Background Error", "AutoCalibration has invalidated background\nPlease run Background",TRUE);
						}else{
							Amulet_DisplayError("Background Error", "Missing Background\nPlease run Background first",TRUE);
						}
						return;
					}
				}else{
					if(Mca_existsAutoCal()){
						Amulet_DisplayError("Calibration Error", "Detector parameters have changed\nPlease perform Auto Calibration", TRUE);
					}else{
						Amulet_DisplayError("Calibration Error", "Detector has never been calibrated\nPlease perform Auto Calibration first",TRUE);
					}
					return;
				}
			}
			//break;
	}
}
