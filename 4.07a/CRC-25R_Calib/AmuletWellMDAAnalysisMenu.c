#define PHASE_WELL_MDA_ANALYSIS_PRE_INIT	0
#define PHASE_WELL_MDA_ANALYSIS_WAIT		1
#define PHASE_WELL_MDA_ANALYSIS_PRINT		2
#define PHASE_WELL_MDA_ANALYSIS_SPECTRUM	3
#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "math.h"
#include "printer.h"
#include "amulet.h"
#include "database.h"

extern int m_iPhase;
extern CURRENT current;
extern DB_SPEC AmuletWellSpectrumMenu_spectrum;
extern char AmuletWellSpectrumMenu_comment1[26];
extern char AmuletWellSpectrumMenu_comment2[26];
extern char AmuletWellSpectrumMenu_comment3[26];
extern char AmuletWellSpectrumMenu_inactiveReason[26];
extern bool AmuletWellSpectrumMenu_inactive;

WELLMDATEST AmuletWellMDAAnalysisMenu_mda;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void PushPageStack(unsigned char ucPage);
static void AmuletWellMDAAnalysis_print(void);

void AmuletWellMDAAnalysis_menu(void){
	char message[51];

	switch(m_iPhase){
		case PHASE_WELL_MDA_ANALYSIS_PRE_INIT:
			GetExtendedTimeInfoSec(&AmuletWellMDAAnalysisMenu_mda.Spectrum.MeasuredOn, message);
			SetAmuletString(100, message);

			switch(AmuletWellMDAAnalysisMenu_mda.DetectorTest){
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

			SetAmuletString(101, AmuletWellMDAAnalysisMenu_mda.NuclideName);

			sprintf(message, "%.3f %%", AmuletWellMDAAnalysisMenu_mda.Efficiency);
			SetAmuletString(102, message);

			sprintf(message, "%.1f - %.1f keV", AmuletWellMDAAnalysisMenu_mda.StartROI, AmuletWellMDAAnalysisMenu_mda.EndROI);
			SetAmuletString(103, message);

			sprintf(message, "%d", AmuletWellMDAAnalysisMenu_mda.ROICounts);
			SetAmuletString(104, message);

			if(current.system == CI) sprintf(message, "%.2f min", AmuletWellMDAAnalysisMenu_mda.Spectrum.LiveTime / 60.0);
			else sprintf(message, "%.2f sec", AmuletWellMDAAnalysisMenu_mda.Spectrum.LiveTime);
			SetAmuletString(105, message);

			sprintf(message, "%.2f", AmuletWellMDAAnalysisMenu_mda.PrecisionFactor);
			SetAmuletString(106, message);

			sprintf(message, "%.2f", AmuletWellMDAAnalysisMenu_mda.CorrectionFactor);
			SetAmuletString(107, message);

			if(current.system == CI) sprintf(message, "%.1f dpm", AmuletWellMDAAnalysisMenu_mda.MDA);
			else sprintf(message, "%.1f Bq", AmuletWellMDAAnalysisMenu_mda.MDA / 60.0);
			SetAmuletString(108, message);

			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(101, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(101, 0xFF);

			if(AmuletWellMDAAnalysisMenu_mda.Inactive){
				SetAmuletString(99, AmuletWellMDAAnalysisMenu_mda.InactiveReason);
				SetAmuletByte(103, 0xFF);
			}else{
				SetAmuletByte(102, 0xFF);
			}

			m_iPhase = PHASE_WELL_MDA_ANALYSIS_WAIT;
			break;

		case PHASE_WELL_MDA_ANALYSIS_WAIT:
			break;

		case PHASE_WELL_MDA_ANALYSIS_PRINT:
			beep_amulet();
			AmuletWellMDAAnalysis_print();
			SetAmuletByte(104, 0xFF);
			m_iPhase = PHASE_WELL_MDA_ANALYSIS_WAIT;
			break;

		case PHASE_WELL_MDA_ANALYSIS_SPECTRUM:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &AmuletWellMDAAnalysisMenu_mda.Spectrum, sizeof(DB_SPEC));
			strcpy(AmuletWellSpectrumMenu_comment1, "MDA Test Spectrum");
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellMDAAnalysisMenu_mda.Inactive){
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellMDAAnalysisMenu_mda.InactiveReason);
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

static void AmuletWellMDAAnalysis_print(void){
	char prtype, strng[90], buf[40], detectortype[30];
	short nc;

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "MDA TEST", AmuletWellMDAAnalysisMenu_mda.Spectrum.MeasuredOn);

		lininit(strng, TRUE, prtype);
		switch(AmuletWellMDAAnalysisMenu_mda.DetectorTest){
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
		strncpy(&strng[2], "Nuclide:", 8);
		nc = strlen(AmuletWellMDAAnalysisMenu_mda.NuclideName);
		strncpy(&strng[11], AmuletWellMDAAnalysisMenu_mda.NuclideName, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "    ROI: ", 9);
		nc = sprintf(buf, "%.1f - %.1f keV", AmuletWellMDAAnalysisMenu_mda.StartROI, AmuletWellMDAAnalysisMenu_mda.EndROI);
		strncpy(&strng[11], buf, nc);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Efficiency: %.3f %%", AmuletWellMDAAnalysisMenu_mda.Efficiency);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[28], "=Eff", 4);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "    Counts: %d", AmuletWellMDAAnalysisMenu_mda.ROICounts);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=N", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) nc = sprintf(buf, "Count Time: %.2f min", AmuletWellMDAAnalysisMenu_mda.Spectrum.LiveTime / 60.0);
		else nc = sprintf(buf, "Count Time: %.2f sec", AmuletWellMDAAnalysisMenu_mda.Spectrum.LiveTime);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=T", 2);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, " Precision: %.2f", AmuletWellMDAAnalysisMenu_mda.PrecisionFactor);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=f", 2);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Correction: %.2f", AmuletWellMDAAnalysisMenu_mda.CorrectionFactor);
		strncpy(&strng[2], buf, nc);
		strncpy(&strng[30], "=C", 2);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		if(current.system == CI) nc = sprintf(buf, "MDA: %.1f dpm", AmuletWellMDAAnalysisMenu_mda.MDA);
		else nc = sprintf(buf, "MDA: %.1f Bq", AmuletWellMDAAnalysisMenu_mda.MDA / 60.0);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "       (f * SQRT(N)) + C", 24);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "MDA = -------------------", 25);
		pr_write(strng);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "            Eff * T", 19);
		pr_write(strng);

		feed(1, prtype);
		if(AmuletWellMDAAnalysisMenu_mda.Inactive){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "INACTIVE: %s", AmuletWellMDAAnalysisMenu_mda.InactiveReason);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		formfeed(prtype);
	}
}
