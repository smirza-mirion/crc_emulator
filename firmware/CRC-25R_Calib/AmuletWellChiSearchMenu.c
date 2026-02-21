#define PHASE_WELLCHISEARCH_PRE_INIT	0
#define PHASE_WELLCHISEARCH_WAIT		1
#define PHASE_WELLCHISEARCH_PGDOWN		2
#define PHASE_WELLCHISEARCH_PGUP		3
#define PHASE_WELLCHISEARCH_ROW1		4
#define PHASE_WELLCHISEARCH_ROW2		5
#define PHASE_WELLCHISEARCH_ROW3		6
#define PHASE_WELLCHISEARCH_ROW4		7
#define PHASE_WELLCHISEARCH_ROW5		8
#define PHASE_WELLCHISEARCH_ROW6		9
#define PHASE_WELLCHISEARCH_ROW7		10
#define PHASE_WELLCHISEARCH_ROW8		11
#define PHASE_WELLCHISEARCH_ROW9		12
#define PHASE_WELLCHISEARCH_ROW10		13
#define PHASE_WELLCHISEARCH_VIEW		14
#define PHASE_WELLCHISEARCH_SEARCH		15
#define PHASE_WELLCHISEARCH_FROM		16
#define PHASE_WELLCHISEARCH_TO			17
#define PHASE_WELLCHISEARCH_CRITERIA	18
#define PHASE_WELLCHISEARCH_PRINT		19

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
extern WELLCHITEST AmuletWellChiAnalysisMenu_chi;

time_t AmuletWellChiSearch_from;
time_t AmuletWellChiSearch_to;
int AmuletWellChiSearch_device; // 0 = All, 1 = Probe, 2 = Well

static time_t AmuletWellChiSearch_fromSearched;
static time_t AmuletWellChiSearch_toSearched;
static int AmuletWellChiSearch_testCount;
static int AmuletWellChiSearch_currentPage;	// Zero based
static int AmuletWellChiSearch_lastPage;		// Zero based
static int AmuletWellChiSearch_currentIndex;	// Zero based
static WELLCHISEARCH AmuletWellChiSearch_test[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellChiSearch_displayPage(void);
static void AmuletWellChiSearch_print(void);

void AmuletWellChiSearch_menu(void){
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLCHISEARCH_PRE_INIT:
			if(m_ucClear == 66){
				AmuletWellChiSearch_testCount = 0;
				AmuletWellChiSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellChiSearch_from = mk_time(&tmt);
				AmuletWellChiSearch_to = mk_time(&tmt);
				AmuletWellChiSearch_device = 0;
				m_ucClear = 0;
			}

			dateout(message, &AmuletWellChiSearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellChiSearch_to, 4);
			SetAmuletString(132, message);

			AmuletWellChiSearch_displayPage();

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(90, 0x00);
			else{
				SetAmuletByte(90, 0xFF);
				switch(AmuletWellChiSearch_device){
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
			//if(AmuletWellChiSearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellChiSearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			break;

		case PHASE_WELLCHISEARCH_WAIT:
			break;

		case PHASE_WELLCHISEARCH_PGDOWN:
			beep_amulet();
			AmuletWellChiSearch_currentPage++;
			AmuletWellChiSearch_displayPage();
			m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			break;

		case PHASE_WELLCHISEARCH_PGUP:
			beep_amulet();
			if(AmuletWellChiSearch_currentPage){
				AmuletWellChiSearch_currentPage--;
				AmuletWellChiSearch_displayPage();
			}
			m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			break;

		case PHASE_WELLCHISEARCH_ROW1:
		case PHASE_WELLCHISEARCH_ROW2:
		case PHASE_WELLCHISEARCH_ROW3:
		case PHASE_WELLCHISEARCH_ROW4:
		case PHASE_WELLCHISEARCH_ROW5:
		case PHASE_WELLCHISEARCH_ROW6:
		case PHASE_WELLCHISEARCH_ROW7:
		case PHASE_WELLCHISEARCH_ROW8:
		case PHASE_WELLCHISEARCH_ROW9:
		case PHASE_WELLCHISEARCH_ROW10:
			beep_amulet();
			AmuletWellChiSearch_currentIndex = (10 * AmuletWellChiSearch_currentPage) + (m_iPhase - PHASE_WELLCHISEARCH_ROW1);
			m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			break;

		case PHASE_WELLCHISEARCH_VIEW:
			beep_amulet();
			AmuletWellChiAnalysisMenu_chi.WellChiSquareTestID = AmuletWellChiSearch_test[AmuletWellChiSearch_currentIndex].WellChiSquareTestID;
			DB_RetrieveChiTest(&AmuletWellChiAnalysisMenu_chi);
			SetAmuletHTML(AmuletHTMLIndex[WELLCHIANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLCHIANALYSIS_HTM]);
			return;

		case PHASE_WELLCHISEARCH_SEARCH:
			beep_amulet();
			AmuletWellChiSearch_fromSearched = AmuletWellChiSearch_from;
			AmuletWellChiSearch_toSearched = AmuletWellChiSearch_to;
			AmuletWellChiSearch_testCount = DB_SearchChiTests(AmuletWellChiSearch_test, AmuletWellChiSearch_from, AmuletWellChiSearch_to, 100, AmuletWellChiSearch_device);
			AmuletWellChiSearch_currentPage = 0;
			if(AmuletWellChiSearch_testCount == -1){
				AmuletWellChiSearch_testCount = 0;
				Amulet_DisplayError("Search Chi Square Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellChiSearch_testCount == 0){
				Amulet_DisplayError("Search Chi Square Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLCHISEARCH_PRE_INIT;
			break;

		case PHASE_WELLCHISEARCH_FROM:
			m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			break;

		case PHASE_WELLCHISEARCH_TO:
			m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			break;

		case PHASE_WELLCHISEARCH_CRITERIA:
			m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			break;

		case PHASE_WELLCHISEARCH_PRINT:
			beep_amulet();
			AmuletWellChiSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLCHISEARCH_WAIT;
			m_iPhase = PHASE_WELLCHISEARCH_PRE_INIT;
			break;
	}
}

static void AmuletWellChiSearch_displayPage(void){
	int index;
	char message[26];
	char chisquare[31];
	char detector[31];
	char measuredon[31];

	AmuletWellChiSearch_lastPage = AmuletWellChiSearch_testCount / 10;
	if(AmuletWellChiSearch_testCount % 10) AmuletWellChiSearch_lastPage++;
	AmuletWellChiSearch_lastPage--;
	if(AmuletWellChiSearch_lastPage < 0) AmuletWellChiSearch_lastPage = 0;

	if(AmuletWellChiSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellChiSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellChiSearch_currentPage == AmuletWellChiSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellChiSearch_currentPage + index) < AmuletWellChiSearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellChiSearch_test[10 * AmuletWellChiSearch_currentPage + index].Inactive){
				chisquare[0] = 4;
				chisquare[1] = 0;
				detector[0] = 4;
				detector[1] = 0;
				measuredon[0] = 4;
				measuredon[1] = 0;
			}else{
				chisquare[0] = 0;
				detector[0] = 0;
				measuredon[0] = 0;
			}

			sprintf(message, "%.1f", AmuletWellChiSearch_test[10 * AmuletWellChiSearch_currentPage + index].ChiSquare);
			strcat(chisquare, message);
			SetAmuletString(3*index + 100, chisquare);

			switch(AmuletWellChiSearch_test[10 * AmuletWellChiSearch_currentPage + index].DetectorTest){
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

			GetExtendedTimeInfoSec(&AmuletWellChiSearch_test[10 * AmuletWellChiSearch_currentPage + index].MeasuredOn, message);
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
	if(AmuletWellChiSearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellChiSearch_currentPage + 1, AmuletWellChiSearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellChiSearch_currentIndex = -1;
}

static void AmuletWellChiSearch_print(void){
	char prtype;
	char strng[90];
	int index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "Chi-Square Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 4, 1, 0, "From:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellChiSearch_fromSearched, TRUE);
		pr_write2sec(strng, "Chi-Square Search");

		insertconst(strng, 4, 1, 0, "To:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellChiSearch_toSearched, TRUE);
		pr_write2sec(strng, "Chi-Square Search");

		insertconst(strng, 4, 1, 0, "Det:");
		switch(AmuletWellChiSearch_device){
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
		pr_write2sec(strng, "Chi-Square Search");

		for(index=0; index<AmuletWellChiSearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellChiAnalysisMenu_chi.WellChiSquareTestID = AmuletWellChiSearch_test[index].WellChiSquareTestID;
			DB_RetrieveChiTest(&AmuletWellChiAnalysisMenu_chi);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2sec(strng, "Chi-Square Search");

			inserttime_tsec(strng, 30, 1, 0, AmuletWellChiAnalysisMenu_chi.CreatedOn, FALSE);
			pr_write2sec(strng, "Chi-Square Search");

			insertconst(strng, 4, 1, 0, "Det:");
			switch(AmuletWellChiAnalysisMenu_chi.DetectorTest){
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
			pr_write2sec(strng, "Chi-Square Search");

			insertconst(strng, 10, 1, 0, "Samples:");
			insertint(strng, 29, 1, 0, "%d", AmuletWellChiAnalysisMenu_chi.NumberOfSamples);
			pr_write2sec(strng, "Chi-Square Search");

			insertconst(strng, 10, 1, 0, "Count Time:");
			insertint(strng, 29, 1, 0, "%d", AmuletWellChiAnalysisMenu_chi.SampleTime);
			insertconst(strng, 33, 1, 0, "sec");
			pr_write2sec(strng, "Chi-Square Search");

			if(AmuletWellChiAnalysisMenu_chi.Inactive){
				insertconst(strng, 9, 1, 0, "INACTIVE");
				if(strlen(AmuletWellChiAnalysisMenu_chi.InactiveReason) > 0){
					insertstring(strng, 10, 0, 0, ": %s", AmuletWellChiAnalysisMenu_chi.InactiveReason);
				}
				pr_write2sec(strng, "Chi-Square Search");
			}else{
				insertconst(strng, 10, 1, 0, "Chi-Square:");
				insertfloat(strng, 29, 1, 0, "%.1f", AmuletWellChiAnalysisMenu_chi.ChiSquare);
				pr_write2sec(strng, "Chi-Square Search");
			}
		}
		formfeed(prtype);
	}
}

void AmuletWellChiSearch_inactivate(long long int WellChiSquareTestID){
	int index;

	if(AmuletWellChiSearch_testCount > 0){
		for(index=0; index<AmuletWellChiSearch_testCount; index++){
			if(AmuletWellChiSearch_test[index].WellChiSquareTestID == WellChiSquareTestID){
				AmuletWellChiSearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
