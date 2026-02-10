#define PHASE_WELLMDASEARCH_PRE_INIT	0
#define PHASE_WELLMDASEARCH_WAIT		1
#define PHASE_WELLMDASEARCH_PGDOWN		2
#define PHASE_WELLMDASEARCH_PGUP		3
#define PHASE_WELLMDASEARCH_ROW1		4
#define PHASE_WELLMDASEARCH_ROW2		5
#define PHASE_WELLMDASEARCH_ROW3		6
#define PHASE_WELLMDASEARCH_ROW4		7
#define PHASE_WELLMDASEARCH_ROW5		8
#define PHASE_WELLMDASEARCH_ROW6		9
#define PHASE_WELLMDASEARCH_ROW7		10
#define PHASE_WELLMDASEARCH_ROW8		11
#define PHASE_WELLMDASEARCH_ROW9		12
#define PHASE_WELLMDASEARCH_ROW10		13
#define PHASE_WELLMDASEARCH_VIEW		14
#define PHASE_WELLMDASEARCH_SEARCH		15
#define PHASE_WELLMDASEARCH_FROM		16
#define PHASE_WELLMDASEARCH_TO			17
#define PHASE_WELLMDASEARCH_CRITERIA	18
#define PHASE_WELLMDASEARCH_PRINT		19

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "wipes.h"
#include "time.h"
#include "mca.h"
#include "database.h"
#include "printer.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern time_t clock_time;
extern WELLMDATEST AmuletWellMDAAnalysisMenu_mda;

time_t AmuletWellMDASearch_from;
time_t AmuletWellMDASearch_to;
int AmuletWellMDASearch_device; // 0 = All, 1 = Probe, 2 = Well

static time_t AmuletWellMDASearch_fromSearched;
static time_t AmuletWellMDASearch_toSearched;
static int AmuletWellMDASearch_testCount;
static int AmuletWellMDASearch_currentPage;	// Zero based
static int AmuletWellMDASearch_lastPage;		// Zero based
static int AmuletWellMDASearch_currentIndex;	// Zero based
static WELLMDASEARCH AmuletWellMDASearch_test[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellMDASearch_displayPage(void);
static void AmuletWellMDASearch_print(void);

void AmuletWellMDASearch_menu(void){
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLMDASEARCH_PRE_INIT:
			if(m_ucClear == 65){
				AmuletWellMDASearch_testCount = 0;
				AmuletWellMDASearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellMDASearch_from = mk_time(&tmt);
				AmuletWellMDASearch_to = mk_time(&tmt);
				AmuletWellMDASearch_device = 0;
				m_ucClear = 0;
			}

			dateout(message, &AmuletWellMDASearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellMDASearch_to, 4);
			SetAmuletString(132, message);

			AmuletWellMDASearch_displayPage();

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(90, 0x00);
			else{
				SetAmuletByte(90, 0xFF);
				switch(AmuletWellMDASearch_device){
					case 0:
						SetAmuletString(133, "All");
						break;

					case 1:
						SetAmuletString(133, "Probe");
						break;

					case 2:
						SetAmuletString(133, "Well");
						break;

				}
			}

			SetAmuletByte(99, 0xFF);
			//if(AmuletWellMDASearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellMDASearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			break;

		case PHASE_WELLMDASEARCH_WAIT:
			break;

		case PHASE_WELLMDASEARCH_PGDOWN:
			beep_amulet();
			AmuletWellMDASearch_currentPage++;
			AmuletWellMDASearch_displayPage();
			m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			break;

		case PHASE_WELLMDASEARCH_PGUP:
			beep_amulet();
			if(AmuletWellMDASearch_currentPage){
				AmuletWellMDASearch_currentPage--;
				AmuletWellMDASearch_displayPage();
			}
			m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			break;

		case PHASE_WELLMDASEARCH_ROW1:
		case PHASE_WELLMDASEARCH_ROW2:
		case PHASE_WELLMDASEARCH_ROW3:
		case PHASE_WELLMDASEARCH_ROW4:
		case PHASE_WELLMDASEARCH_ROW5:
		case PHASE_WELLMDASEARCH_ROW6:
		case PHASE_WELLMDASEARCH_ROW7:
		case PHASE_WELLMDASEARCH_ROW8:
		case PHASE_WELLMDASEARCH_ROW9:
		case PHASE_WELLMDASEARCH_ROW10:
			beep_amulet();
			AmuletWellMDASearch_currentIndex = (10 * AmuletWellMDASearch_currentPage) + (m_iPhase - PHASE_WELLMDASEARCH_ROW1);
			m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			break;

		case PHASE_WELLMDASEARCH_VIEW:
			beep_amulet();
			AmuletWellMDAAnalysisMenu_mda.WellMDATestID = AmuletWellMDASearch_test[AmuletWellMDASearch_currentIndex].WellMDATestID;
			DB_RetrieveMDATest(&AmuletWellMDAAnalysisMenu_mda);
			SetAmuletHTML(AmuletHTMLIndex[WELLMDAANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLMDAANALYSIS_HTM]);
			return;

		case PHASE_WELLMDASEARCH_SEARCH:
			beep_amulet();
			AmuletWellMDASearch_fromSearched = AmuletWellMDASearch_from;
			AmuletWellMDASearch_toSearched = AmuletWellMDASearch_to;
			AmuletWellMDASearch_testCount = DB_SearchMDATests(AmuletWellMDASearch_test, AmuletWellMDASearch_from, AmuletWellMDASearch_to, 100, AmuletWellMDASearch_device);
			AmuletWellMDASearch_currentPage = 0;
			if(AmuletWellMDASearch_testCount == -1){
				AmuletWellMDASearch_testCount = 0;
				Amulet_DisplayError("Search MDA Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellMDASearch_testCount == 0){
				Amulet_DisplayError("Search MDA Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLMDASEARCH_PRE_INIT;
			break;

		case PHASE_WELLMDASEARCH_FROM:
			m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			break;

		case PHASE_WELLMDASEARCH_TO:
			m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			break;

		case PHASE_WELLMDASEARCH_CRITERIA:
			m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			break;

		case PHASE_WELLMDASEARCH_PRINT:
			beep_amulet();
			AmuletWellMDASearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLMDASEARCH_WAIT;
			m_iPhase = PHASE_WELLMDASEARCH_PRE_INIT;
			break;
	}
}

static void AmuletWellMDASearch_displayPage(void){
	int index;
	char message[26];
	char nuclidename[31];
	char mdavalue[31];
	char detector[31];
	char measuredon[31];

	AmuletWellMDASearch_lastPage = AmuletWellMDASearch_testCount / 10;
	if(AmuletWellMDASearch_testCount % 10) AmuletWellMDASearch_lastPage++;
	AmuletWellMDASearch_lastPage--;
	if(AmuletWellMDASearch_lastPage < 0) AmuletWellMDASearch_lastPage = 0;

	if(AmuletWellMDASearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellMDASearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellMDASearch_currentPage == AmuletWellMDASearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellMDASearch_currentPage + index) < AmuletWellMDASearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellMDASearch_test[10 * AmuletWellMDASearch_currentPage + index].Inactive){
				mdavalue[0] = 4;
				mdavalue[1] = 0;
				detector[0] = 4;
				detector[1] = 0;
				measuredon[0] = 4;
				measuredon[1] = 0;
			}else{
				mdavalue[0] = 0;
				detector[0] = 0;
				measuredon[0] = 0;
			}
			strcpy(nuclidename, AmuletWellMDASearch_test[10 * AmuletWellMDASearch_currentPage + index].NuclideName);
			if(current.system == CI) sprintf(message, "%s (%.1f dpm)", nuclidename, AmuletWellMDASearch_test[10 * AmuletWellMDASearch_currentPage + index].MDA);
			else sprintf(message, "%s (%.1f Bq)", nuclidename, (AmuletWellMDASearch_test[10 * AmuletWellMDASearch_currentPage + index].MDA) / 60.0);
			strcat(mdavalue, message);
			SetAmuletString(3*index + 100, mdavalue);

			switch(AmuletWellMDASearch_test[10 * AmuletWellMDASearch_currentPage + index].DetectorType){
				case DET_WELL:
					strcat(detector, "WELL");
					break;

				case DET_WELL700:
					strcat(detector, "Well");
					break;

				case DET_BETA:
					strcat(detector, "Beta");
					break;

				case DET_PROBE700:
					strcat(detector, "Probe");
					break;

				case DET_DRILLEDPROBE700:
					strcat(detector, "Drilled Probe");
					break;

				case DET_DRILLEDPROBE700_AS_WELL:
					strcat(detector, "Drilled Probe As Well");
					break;

				case DET_DRILLEDPROBE700_AS_PROBE:
					strcat(detector, "Drilled Probe As Probe");
					break;
			}
			SetAmuletString(3*index + 101, detector);

			GetExtendedTimeInfoSec(&AmuletWellMDASearch_test[10 * AmuletWellMDASearch_currentPage + index].MeasuredOn, message);
			strcat(measuredon, message);
			SetAmuletString(3*index + 102, measuredon);
		}else{
			SetAmuletByte(101 + index, 0x00);
			SetAmuletString(3*index + 100, "");
			SetAmuletString(3*index + 101, "");
			SetAmuletString(3*index + 102, "");
		}
	}

	message[0] = 0;
	if(AmuletWellMDASearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellMDASearch_currentPage + 1, AmuletWellMDASearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellMDASearch_currentIndex = -1;
}

static void AmuletWellMDASearch_print(void){
	char prtype;
	char strng[90];
	int index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "MDA Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 4, 1, 0, "From:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellMDASearch_fromSearched, TRUE);
		pr_write2sec(strng, "MDA Search");

		insertconst(strng, 4, 1, 0, "To:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellMDASearch_toSearched, TRUE);
		pr_write2sec(strng, "MDA Search");

		insertconst(strng, 4, 1, 0, "Det:");
		switch(AmuletWellMDASearch_device){
			case 0:
				insertstring(strng, 6, 0, 0, "%s", "All");
				break;

			case 1:
				insertstring(strng, 6, 0, 0, "%s", "Probe");
				break;

			case 2:
				insertstring(strng, 6, 0, 0, "%s", "Well");
				break;
		}
		pr_write2sec(strng, "MDA Search");

		for(index=0; index<AmuletWellMDASearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellMDAAnalysisMenu_mda.WellMDATestID = AmuletWellMDASearch_test[index].WellMDATestID;
			DB_RetrieveMDATest(&AmuletWellMDAAnalysisMenu_mda);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2sec(strng, "MDA Search");

			inserttime_tsec(strng, 30, 1, 0, AmuletWellMDAAnalysisMenu_mda.Spectrum.MeasuredOn, FALSE);
			pr_write2sec(strng, "MDA Search");

			insertconst(strng, 10, 1, 0, "Nuclide:");
			insertconst(strng, 12, 0, 0, AmuletWellMDAAnalysisMenu_mda.NuclideName);
			pr_write2sec(strng, "MDA Search");

			if(AmuletWellMDAAnalysisMenu_mda.Inactive){
				insertconst(strng, 9, 1, 0, "INACTIVE");
				if(strlen(AmuletWellMDAAnalysisMenu_mda.InactiveReason) > 0){
					insertstring(strng, 10, 0, 0, ": %s", AmuletWellMDAAnalysisMenu_mda.InactiveReason);
				}
				pr_write2sec(strng, "MDA Search");
			}else{
				insertconst(strng, 4, 1, 0, "Det:");
				switch(AmuletWellMDAAnalysisMenu_mda.DetectorTest){
					case DET_WELL:
						insertstring(strng, 6, 0, 0, "%s", "WELL");
						break;

					case DET_WELL700:
						insertstring(strng, 6, 0, 0, "%s", "Well");
						break;

					case DET_BETA:
						insertstring(strng, 6, 0, 0, "%s", "Beta");
						break;
					case DET_PROBE700:
						insertstring(strng, 6, 0, 0, "%s", "Probe");
						break;
					case DET_DRILLEDPROBE700:
						insertstring(strng, 6, 0, 0, "%s", "Drilled Probe");
						break;
					case DET_DRILLEDPROBE700_AS_WELL:
						insertstring(strng, 6, 0, 0, "%s", "Drilled Probe As Well");
						break;
					case DET_DRILLEDPROBE700_AS_PROBE:
						insertstring(strng, 6, 0, 0, "%s", "Drilled Probe As Probe");
						break;
				}
				pr_write2sec(strng, "MDA Search");

				insertconst(strng, 10, 1, 0, "Precision:");
				insertfloat(strng, 29, 1, 0, "%.2f", AmuletWellMDAAnalysisMenu_mda.PrecisionFactor);
				pr_write2sec(strng, "MDA Search");

				insertconst(strng, 10, 1, 0, "Correction:");
				insertfloat(strng, 29, 1, 0, "%.2f", AmuletWellMDAAnalysisMenu_mda.CorrectionFactor);
				pr_write2sec(strng, "MDA Search");

				insertconst(strng, 10, 1, 0, "MDA:");
				if(current.system == CI){
					insertfloat(strng, 29, 1, 0, "%.2f", AmuletWellMDAAnalysisMenu_mda.MDA);
					insertconst(strng, 31, 0, 0, "dpm");
				}else{
					insertfloat(strng, 29, 1, 0, "%.2f", AmuletWellMDAAnalysisMenu_mda.MDA / 60.0);
					insertconst(strng, 31, 0, 0, "Bq");
				}
				pr_write2sec(strng, "MDA Search");
			}
		}

		formfeed(prtype);
	}
}

void AmuletWellMDASearch_inactivate(long long int WellMDATestID){
	int index;

	if(AmuletWellMDASearch_testCount > 0){
		for(index=0; index<AmuletWellMDASearch_testCount; index++){
			if(AmuletWellMDASearch_test[index].WellMDATestID == WellMDATestID){
				AmuletWellMDASearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
