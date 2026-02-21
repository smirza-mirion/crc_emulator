#define PHASE_WELLSYSTESTANALYSIS_PRE_INIT	0
#define PHASE_WELLSYSTESTANALYSIS_WAIT		1
#define PHASE_WELLSYSTESTANALYSIS_PRINT		2
#define PHASE_WELLSYSTESTANALYSIS_SPECTRUM	3
#define PHASE_WELLSYSTESTANALYSIS_BACKGND	4

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "nuc.h"
#include "mca.h"
#include "printer.h"
#include "amulet.h"
#include "math.h"
#include "database.h"

extern int m_iPhase;
extern CURRENT current;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;
extern BACKGND temp_backgnd;

WELLSYSTEMTEST AmuletWellSystemTestAnalysisMenu_systemTest;

void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void PushPageStack(unsigned char ucPage);
static void AmuletWellSystemTestAnalysis_print(void);

void AmuletWellSystemTestAnalysis_menu(void){
	char message[51];

	switch(m_iPhase){
		case PHASE_WELLSYSTESTANALYSIS_PRE_INIT:
			GetExtendedTimeInfoSec(&AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.MeasuredOn, message);
			SetAmuletString(100, message);

			switch(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest){
				case DET_WELL:
					strcpy(message, "WELL");
					break;

				case DET_WELL700:
					strcpy(message, "Well");
					break;

				case DET_BETA:
					strcpy(message, "Beta");
					break;

				case DET_PROBE700:
					strcpy(message, "Probe");
					break;

				case DET_DRILLEDPROBE700:
					strcpy(message, "Drilled Probe");
					break;

				case DET_DRILLEDPROBE700_AS_WELL:
					strcpy(message, "Drilled Probe As Well");
					break;

				case DET_DRILLEDPROBE700_AS_PROBE:
					strcpy(message, "Drilled Probe As Probe");
					break;
			}
			SetAmuletString(109, message);

			if(current.system == CI) format_activity_system2(Mca_convertDpmToCi((AmuletWellSystemTestAnalysisMenu_systemTest.BackgroundROICPM/AmuletWellSystemTestAnalysisMenu_systemTest.Efficiency)*100.0), message);
			else format_activity_system_kbq(Mca_convertDpmToCi((AmuletWellSystemTestAnalysisMenu_systemTest.BackgroundROICPM/AmuletWellSystemTestAnalysisMenu_systemTest.Efficiency)*100.0), message);
			SetAmuletString(108, message);

			SetAmuletString(101, AmuletWellSystemTestAnalysisMenu_systemTest.SerialNumber);
			GetExtendedTimeInfo(&AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationDate, message);
			SetAmuletString(102, message);
			if(current.system == CI) format_activity_system2(AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationActivity, message);
			else format_activity_system_kbq(AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationActivity, message);
			SetAmuletString(103, message);

			if(current.system == CI) format_activity_system2(AmuletWellSystemTestAnalysisMenu_systemTest.PredictedActivity, message);
			else format_activity_system_kbq(AmuletWellSystemTestAnalysisMenu_systemTest.PredictedActivity, message);
			SetAmuletString(104, message);

			sprintf(message, "%.1f sec", AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.LiveTime);
			SetAmuletString(105, message);

			if(current.system == CI) format_activity_system2(AmuletWellSystemTestAnalysisMenu_systemTest.Activity, message);
			else format_activity_system_kbq(AmuletWellSystemTestAnalysisMenu_systemTest.Activity, message);
			SetAmuletString(106, message);

			sprintf(message, "%.1f %%", AmuletWellSystemTestAnalysisMenu_systemTest.Deviation);
			SetAmuletString(107, message);

			if(fabs(AmuletWellSystemTestAnalysisMenu_systemTest.Deviation) > 10) SetAmuletByte(102, 0xFF);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			if(AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID > 0){
				if(AmuletWellSystemTestAnalysisMenu_systemTest.Inactive){
					SetAmuletString(99, AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason);
					SetAmuletByte(104, 0xFF);
				}else{
					SetAmuletByte(103, 0xFF);
				}
			}else{
				SetAmuletByte(105, 0xFF);
			}

			m_iPhase = PHASE_WELLSYSTESTANALYSIS_WAIT;
			break;

		case PHASE_WELLSYSTESTANALYSIS_WAIT:
			break;

		case PHASE_WELLSYSTESTANALYSIS_PRINT:
			beep_amulet();
			AmuletWellSystemTestAnalysis_print();
			SetAmuletByte(106, 0xFF);
			m_iPhase = PHASE_WELLSYSTESTANALYSIS_WAIT;
			break;

		case PHASE_WELLSYSTESTANALYSIS_SPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum, sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "System Test Spectrum");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if((AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID > 0) && (AmuletWellSystemTestAnalysisMenu_systemTest.Inactive)){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;

		case PHASE_WELLSYSTESTANALYSIS_BACKGND:
			temp_backgnd.WellBackgroundID = AmuletWellSystemTestAnalysisMenu_systemTest.WellBackgroundID;
			DB_RetrieveBackgnd(&temp_backgnd);
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(temp_backgnd.Spectrum), sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "System Test Background");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if((AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID > 0) && (AmuletWellSystemTestAnalysisMenu_systemTest.Inactive)){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason);
				AmuletWellSpectrumMenu_inactive = TRUE;
			}else{
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
				AmuletWellSpectrumMenu_inactive = FALSE;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

static void AmuletWellSystemTestAnalysis_print(void){
	char prtype;
	char strng[90];
	char buf[40], buf2[40], detectortype[30];
	short nc;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		g_resetlinespacing(prtype);

		rawheadersec(prtype, "SYSTEM TEST", AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.MeasuredOn);

		switch(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest){
			case DET_WELL:
				strcpy(detectortype, "Det: WELL");
				break;

			case DET_WELL700:
				strcpy(detectortype, "Det: Well");
				break;

			case DET_BETA:
				strcpy(detectortype, "Det: Beta");
				break;

			case DET_PROBE700:
				strcpy(detectortype, "Det: Probe");
				break;

			case DET_DRILLEDPROBE700:
				strcpy(detectortype, "Det: Drilled Probe");
				break;

			case DET_DRILLEDPROBE700_AS_WELL:
				strcpy(detectortype, "Det: Drilled Probe As Well");
				break;

			case DET_DRILLEDPROBE700_AS_PROBE:
				strcpy(detectortype, "Det: Drilled Probe As Probe");
				break;
		}
		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%s", detectortype);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Nuclide: Cs137", 14);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "%.1f - %.1f keV", AmuletWellSystemTestAnalysisMenu_systemTest.StartROI, AmuletWellSystemTestAnalysisMenu_systemTest.EndROI);
		strncpy(&strng[2], "    ROI: ", 9);
		strncpy(&strng[11], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "    S/N: %s", AmuletWellSystemTestAnalysisMenu_systemTest.SerialNumber);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		GetExtendedTimeInfo(&AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationDate, buf2);
		nc = sprintf(buf, "Date: %s", buf2);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) format_activity_system2(AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationActivity, buf2);
		else format_activity_system_kbq(AmuletWellSystemTestAnalysisMenu_systemTest.CalibrationActivity, buf2);
		replace(buf2, '$', 'u');
		nc = sprintf(buf, "Activity: %s", buf2);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) format_activity_system2(AmuletWellSystemTestAnalysisMenu_systemTest.PredictedActivity, buf2);
		else format_activity_system_kbq(AmuletWellSystemTestAnalysisMenu_systemTest.PredictedActivity, buf2);
		replace(buf2, '$', 'u');
		nc = sprintf(buf, "Decayed Activity: %s", buf2);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) format_activity_system2(AmuletWellSystemTestAnalysisMenu_systemTest.Activity, buf2);
		else format_activity_system_kbq(AmuletWellSystemTestAnalysisMenu_systemTest.Activity, buf2);
		replace(buf2, '$', 'u');
		nc = sprintf(buf, "        Measured: %s", buf2);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "      Count Time: %.1f sec", AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.LiveTime);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "       Deviation: %.1f %%", AmuletWellSystemTestAnalysisMenu_systemTest.Deviation);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		if(fabs(AmuletWellSystemTestAnalysisMenu_systemTest.Deviation) > 10){
			lininit(strng, TRUE, prtype);
			strncpy(&strng[2], "FAILED", 6);
			pr_write(strng);
		}

		feed(1, prtype);

		if((AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID > 0) && (AmuletWellSystemTestAnalysisMenu_systemTest.Inactive)){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "INACTIVE: %s", AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		formfeed(prtype);
	}
}
