#define PHASE_WELL_CHI_ANALYSIS_PRE_INIT	0
#define PHASE_WELL_CHI_ANALYSIS_WAIT		1
#define PHASE_WELL_CHI_ANALYSIS_PRINT		2
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM1   3
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM2   4
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM3   5
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM4   6
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM5   7
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM6   8
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM7   9
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM8   10
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM9   11
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM10  12
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM11  13
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM12  14
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM13  15
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM14  16
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM15  17
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM16  18
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM17  19
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM18  20
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM19  21
#define PHASE_WELL_CHI_ANALYSIS_SPECTRUM20  22
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
WELLCHITEST AmuletWellChiAnalysisMenu_chi;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void PushPageStack(unsigned char ucPage);
static void AmuletWellChiAnalysis_print(void);

void AmuletWellChiAnalysis_menu(void){
	int index;
	char message[51];

	switch(m_iPhase){
		case PHASE_WELL_CHI_ANALYSIS_PRE_INIT:
			GetExtendedTimeInfoSec(&AmuletWellChiAnalysisMenu_chi.CreatedOn, message);
			SetAmuletString(90, message);

			switch(AmuletWellChiAnalysisMenu_chi.DetectorTest){
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
			SetAmuletString(92, message);

			SetAmuletString(124, AmuletWellChiAnalysisMenu_chi.NuclideName);
			sprintf(message, "%.1f - %.1f keV", AmuletWellChiAnalysisMenu_chi.StartROI, AmuletWellChiAnalysisMenu_chi.EndROI);
			SetAmuletString(125, message);
			sprintf(message, "%d sec", AmuletWellChiAnalysisMenu_chi.SampleTime);
			SetAmuletString(123, message);
			sprintf(message, "Chi-Square = %.1f", AmuletWellChiAnalysisMenu_chi.ChiSquare);
			SetAmuletString(121, message);

			for(index=0; index<AmuletWellChiAnalysisMenu_chi.NumberOfSamples; index++){
				sprintf(message, "%d) %ld", index+1, AmuletWellChiAnalysisMenu_chi.WellChiSquareSample[index].ROICounts);
				SetAmuletString(101 + index, message);
				SetAmuletByte(101 + index, 0xFF);
				delayloop(20);
			}
			SetAmuletByte(100, 0xFF);

			//if(current.printer != NONE_PRINTER) SetAmuletByte(121, 0xFF);
			if((current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(121, 0xFF);

			if(AmuletWellChiAnalysisMenu_chi.Inactive){
				SetAmuletString(99, AmuletWellChiAnalysisMenu_chi.InactiveReason);
				SetAmuletByte(123, 0xFF);
			}else{
				SetAmuletByte(122, 0xFF);
			}

			m_iPhase = PHASE_WELL_CHI_ANALYSIS_WAIT;
			break;

		case PHASE_WELL_CHI_ANALYSIS_WAIT:
			break;

		case PHASE_WELL_CHI_ANALYSIS_PRINT:
			beep_amulet();
			AmuletWellChiAnalysis_print();
			SetAmuletByte(124, 0xFF);
			m_iPhase = PHASE_WELL_CHI_ANALYSIS_WAIT;
			break;

		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM1:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM2:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM3:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM4:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM5:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM6:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM7:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM8:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM9:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM10:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM11:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM12:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM13:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM14:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM15:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM16:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM17:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM18:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM19:
		case PHASE_WELL_CHI_ANALYSIS_SPECTRUM20:
			beep_amulet();
			memcpy(&AmuletWellSpectrumMenu_spectrum, &(AmuletWellChiAnalysisMenu_chi.WellChiSquareSample[m_iPhase - PHASE_WELL_CHI_ANALYSIS_SPECTRUM1].Spectrum), sizeof(DB_SPEC));
			sprintf(message, "Chi-Square Spectrum #%d", m_iPhase - PHASE_WELL_CHI_ANALYSIS_SPECTRUM1 + 1);
			strcpy(AmuletWellSpectrumMenu_comment1, message);
			AmuletWellSpectrumMenu_comment2[0] = 0;
			AmuletWellSpectrumMenu_comment3[0] = 0;
			if(AmuletWellChiAnalysisMenu_chi.Inactive){
				AmuletWellSpectrumMenu_inactive = TRUE;
				strcpy(AmuletWellSpectrumMenu_inactiveReason, AmuletWellChiAnalysisMenu_chi.InactiveReason);
			}else{
				AmuletWellSpectrumMenu_inactive = FALSE;
				AmuletWellSpectrumMenu_inactiveReason[0] = 0;
			}
			SetAmuletHTML(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSPECTRUM_HTM]);
			return;
	}
}

static void AmuletWellChiAnalysis_print(void){
	char prtype;
	char strng[90], detectortype[30];
	char buf[40];
	short nc, index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "CHI-SQUARE TEST", AmuletWellChiAnalysisMenu_chi.CreatedOn);

		switch(AmuletWellChiAnalysisMenu_chi.DetectorTest){
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
		nc = sprintf(buf, "Count Time: %d sec", AmuletWellChiAnalysisMenu_chi.SampleTime);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		strncpy(&strng[2], "Test", 4);
		strncpy(&strng[26], "Counts", 6);
		pr_write(strng);

		for(index=0; index<AmuletWellChiAnalysisMenu_chi.NumberOfSamples; index++){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "%d", index+1);
			strncpy(&strng[2], buf, nc);
			nc = sprintf(buf, "%ld", AmuletWellChiAnalysisMenu_chi.WellChiSquareSample[index].ROICounts);
			strncpy(&strng[32 - nc], buf, nc);
			pr_write(strng);
		}

		feed(1, prtype);

		lininit(strng, TRUE, prtype);
		nc = sprintf(buf, "Chi-Square = %.1f", AmuletWellChiAnalysisMenu_chi.ChiSquare);
		strncpy(&strng[2], buf, nc);
		pr_write(strng);

		feed(1, prtype);
		if(AmuletWellChiAnalysisMenu_chi.Inactive){
			lininit(strng, TRUE, prtype);
			nc = sprintf(buf, "INACTIVE: %s", AmuletWellChiAnalysisMenu_chi.InactiveReason);
			strncpy(strng, buf, nc);
			pr_write(strng);
		}

		formfeed(prtype);
	}
}
