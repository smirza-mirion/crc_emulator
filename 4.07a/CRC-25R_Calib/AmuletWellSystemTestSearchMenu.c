#define PHASE_WELLSYSTEMTESTSEARCH_PRE_INIT	0
#define PHASE_WELLSYSTEMTESTSEARCH_WAIT		1
#define PHASE_WELLSYSTEMTESTSEARCH_PGDOWN		2
#define PHASE_WELLSYSTEMTESTSEARCH_PGUP		3
#define PHASE_WELLSYSTEMTESTSEARCH_ROW1		4
#define PHASE_WELLSYSTEMTESTSEARCH_ROW2		5
#define PHASE_WELLSYSTEMTESTSEARCH_ROW3		6
#define PHASE_WELLSYSTEMTESTSEARCH_ROW4		7
#define PHASE_WELLSYSTEMTESTSEARCH_ROW5		8
#define PHASE_WELLSYSTEMTESTSEARCH_ROW6		9
#define PHASE_WELLSYSTEMTESTSEARCH_ROW7		10
#define PHASE_WELLSYSTEMTESTSEARCH_ROW8		11
#define PHASE_WELLSYSTEMTESTSEARCH_ROW9		12
#define PHASE_WELLSYSTEMTESTSEARCH_ROW10		13
#define PHASE_WELLSYSTEMTESTSEARCH_VIEW		14
#define PHASE_WELLSYSTEMTESTSEARCH_SEARCH		15
#define PHASE_WELLSYSTEMTESTSEARCH_FROM		16
#define PHASE_WELLSYSTEMTESTSEARCH_TO			17
#define PHASE_WELLSYSTEMTESTSEARCH_CRITERIA	18
#define PHASE_WELLSYSTEMTESTSEARCH_PRINT	19

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
extern WELLSYSTEMTEST AmuletWellSystemTestAnalysisMenu_systemTest;

time_t AmuletWellSystemTestSearch_from;
time_t AmuletWellSystemTestSearch_to;
int AmuletWellSystemTestSearch_device; // 0 = All, 1 = Probe, 2 = Well

static time_t AmuletWellSystemTestSearch_fromSearched;
static time_t AmuletWellSystemTestSearch_toSearched;
static int AmuletWellSystemTestSearch_testCount;
static int AmuletWellSystemTestSearch_currentPage;	// Zero based
static int AmuletWellSystemTestSearch_lastPage;		// Zero based
static int AmuletWellSystemTestSearch_currentIndex;	// Zero based
static WELLSYSTEMTESTSEARCH AmuletWellSystemTestSearch_test[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellSystemTestSearch_displayPage(void);
static void AmuletWellSystemTestSearch_print(void);

void AmuletWellSystemTestSearch_menu(void){
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLSYSTEMTESTSEARCH_PRE_INIT:
			if(m_ucClear == 64){
				AmuletWellSystemTestSearch_testCount = 0;
				AmuletWellSystemTestSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellSystemTestSearch_from = mk_time(&tmt);
				AmuletWellSystemTestSearch_to = mk_time(&tmt);
				AmuletWellSystemTestSearch_device = 0;
				m_ucClear = 0;
			}

			dateout(message, &AmuletWellSystemTestSearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellSystemTestSearch_to, 4);
			SetAmuletString(132, message);

			AmuletWellSystemTestSearch_displayPage();

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(90, 0x00);
			else{
				SetAmuletByte(90, 0xFF);
				switch(AmuletWellSystemTestSearch_device){
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

			//if(AmuletWellSystemTestSearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellSystemTestSearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_WAIT:
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_PGDOWN:
			beep_amulet();
			AmuletWellSystemTestSearch_currentPage++;
			AmuletWellSystemTestSearch_displayPage();
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_PGUP:
			beep_amulet();
			if(AmuletWellSystemTestSearch_currentPage){
				AmuletWellSystemTestSearch_currentPage--;
				AmuletWellSystemTestSearch_displayPage();
			}
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_ROW1:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW2:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW3:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW4:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW5:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW6:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW7:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW8:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW9:
		case PHASE_WELLSYSTEMTESTSEARCH_ROW10:
			beep_amulet();
			AmuletWellSystemTestSearch_currentIndex = (10 * AmuletWellSystemTestSearch_currentPage) + (m_iPhase - PHASE_WELLSYSTEMTESTSEARCH_ROW1);
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_VIEW:
			beep_amulet();
			AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID = AmuletWellSystemTestSearch_test[AmuletWellSystemTestSearch_currentIndex].WellSystemTestID;
			DB_RetrieveSystemTest(&AmuletWellSystemTestAnalysisMenu_systemTest);
			SetAmuletHTML(AmuletHTMLIndex[WELLSYSTEMTESTANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSYSTEMTESTANALYSIS_HTM]);
			return;

		case PHASE_WELLSYSTEMTESTSEARCH_SEARCH:
			beep_amulet();
			AmuletWellSystemTestSearch_fromSearched = AmuletWellSystemTestSearch_from;
			AmuletWellSystemTestSearch_toSearched = AmuletWellSystemTestSearch_to;
			AmuletWellSystemTestSearch_testCount = DB_SearchSystemTests(AmuletWellSystemTestSearch_test, AmuletWellSystemTestSearch_from, AmuletWellSystemTestSearch_to, 100, AmuletWellSystemTestSearch_device);
			AmuletWellSystemTestSearch_currentPage = 0;
			if(AmuletWellSystemTestSearch_testCount == -1){
				AmuletWellSystemTestSearch_testCount = 0;
				Amulet_DisplayError("Search System Test Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellSystemTestSearch_testCount == 0){
				Amulet_DisplayError("Search System Test Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_PRE_INIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_FROM:
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_TO:
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_CRITERIA:
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			break;

		case PHASE_WELLSYSTEMTESTSEARCH_PRINT:
			beep_amulet();
			AmuletWellSystemTestSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_WAIT;
			m_iPhase = PHASE_WELLSYSTEMTESTSEARCH_PRE_INIT;
			break;
	}
}

static void AmuletWellSystemTestSearch_displayPage(void){
	int index;
	char message[26];
	char deviation[31];
	char detector[31];
	char measuredon[31];

	AmuletWellSystemTestSearch_lastPage = AmuletWellSystemTestSearch_testCount / 10;
	if(AmuletWellSystemTestSearch_testCount % 10) AmuletWellSystemTestSearch_lastPage++;
	AmuletWellSystemTestSearch_lastPage--;
	if(AmuletWellSystemTestSearch_lastPage < 0) AmuletWellSystemTestSearch_lastPage = 0;

	if(AmuletWellSystemTestSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellSystemTestSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellSystemTestSearch_currentPage == AmuletWellSystemTestSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellSystemTestSearch_currentPage + index) < AmuletWellSystemTestSearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellSystemTestSearch_test[10 * AmuletWellSystemTestSearch_currentPage + index].Inactive){
				deviation[0] = 4;
				deviation[1] = 0;
				detector[0] = 4;
				detector[1] = 0;
				measuredon[0] = 4;
				measuredon[1] = 0;
			}else{
				deviation[0] = 0;
				detector[0] = 0;
				measuredon[0] = 0;
			}

			sprintf(message, "%.2f %%", AmuletWellSystemTestSearch_test[10 * AmuletWellSystemTestSearch_currentPage + index].Deviation);
			strcat(deviation, message);
			SetAmuletString(3*index + 100, deviation);

			switch(AmuletWellSystemTestSearch_test[10 * AmuletWellSystemTestSearch_currentPage + index].DetectorTest){
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

			GetExtendedTimeInfoSec(&AmuletWellSystemTestSearch_test[10 * AmuletWellSystemTestSearch_currentPage + index].MeasuredOn, message);
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
	if(AmuletWellSystemTestSearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellSystemTestSearch_currentPage + 1, AmuletWellSystemTestSearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellSystemTestSearch_currentIndex = -1;
}

static void AmuletWellSystemTestSearch_print(void){
	char prtype;
	char strng[90], message[90];
	int index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "System Test Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 4, 1, 0, "From:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellSystemTestSearch_fromSearched, TRUE);
		pr_write2sec(strng, "System Test Search");

		insertconst(strng, 4, 1, 0, "To:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellSystemTestSearch_toSearched, TRUE);
		pr_write2sec(strng, "System Test Search");

		insertconst(strng, 4, 1, 0, "Det:");
		switch(AmuletWellSystemTestSearch_device){
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
		pr_write2sec(strng, "System Test Search");

		for(index=0; index<AmuletWellSystemTestSearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellSystemTestAnalysisMenu_systemTest.WellSystemTestID = AmuletWellSystemTestSearch_test[index].WellSystemTestID;
			DB_RetrieveSystemTest(&AmuletWellSystemTestAnalysisMenu_systemTest);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2sec(strng, "System Test Search");

			inserttime_tsec(strng, 30, 1, 0, AmuletWellSystemTestAnalysisMenu_systemTest.Spectrum.MeasuredOn, FALSE);
			pr_write2sec(strng, "System Test Search");

			insertconst(strng, 9, 1, 0, "S/N:");
			insertconst(strng, 11, 0, 0, AmuletWellSystemTestAnalysisMenu_systemTest.SerialNumber);
			pr_write2sec(strng, "System Test Search");


			if(AmuletWellSystemTestAnalysisMenu_systemTest.Inactive){
				insertconst(strng, 8, 1, 0, "INACTIVE");
				if(strlen(AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason) > 0){
					insertstring(strng, 9, 0, 0, ": %s", AmuletWellSystemTestAnalysisMenu_systemTest.InactiveReason);
				}
				pr_write2sec(strng, "System Test Search");
			}else{
				insertconst(strng, 4, 1, 0, "Det:");
				switch(AmuletWellSystemTestAnalysisMenu_systemTest.DetectorTest){
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
				pr_write2sec(strng, "System Test Search");

				insertconst(strng, 9, 1, 0, "Activity:");
				if(current.system == CI) format_activity_system2(AmuletWellSystemTestAnalysisMenu_systemTest.Activity, message);
				else format_activity_system_kbq(AmuletWellSystemTestAnalysisMenu_systemTest.Activity, message);
				replace(message, '$', 'u');
				insertconst(strng, 11, 0, 0, message);
				pr_write2sec(strng, "System Test Search");

				insertconst(strng, 9, 1, 0, "Deviation:");
				insertfloat(strng, 11, 0, 0, "%.1f%%", AmuletWellSystemTestAnalysisMenu_systemTest.Deviation);

				if(AmuletWellSystemTestAnalysisMenu_systemTest.Deviation > 10.0) insertconst(strng, 33, 1, 0, "FAIL");
				pr_write2sec(strng, "System Test Search");
			}
		}

		formfeed(prtype);
	}
}

void AmuletWellSystemTestSearch_inactivate(long long int WellSystemTestID){
	int index;

	if(AmuletWellSystemTestSearch_testCount > 0){
		for(index=0; index<AmuletWellSystemTestSearch_testCount; index++){
			if(AmuletWellSystemTestSearch_test[index].WellSystemTestID == WellSystemTestID){
				AmuletWellSystemTestSearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
