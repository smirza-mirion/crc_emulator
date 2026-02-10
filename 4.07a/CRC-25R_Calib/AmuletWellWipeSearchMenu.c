#define PHASE_WELLWIPESEARCH_PRE_INIT	0
#define PHASE_WELLWIPESEARCH_WAIT		1
#define PHASE_WELLWIPESEARCH_PGDOWN		2
#define PHASE_WELLWIPESEARCH_PGUP		3
#define PHASE_WELLWIPESEARCH_ROW1		4
#define PHASE_WELLWIPESEARCH_ROW2		5
#define PHASE_WELLWIPESEARCH_ROW3		6
#define PHASE_WELLWIPESEARCH_ROW4		7
#define PHASE_WELLWIPESEARCH_ROW5		8
#define PHASE_WELLWIPESEARCH_ROW6		9
#define PHASE_WELLWIPESEARCH_ROW7		10
#define PHASE_WELLWIPESEARCH_ROW8		11
#define PHASE_WELLWIPESEARCH_ROW9		12
#define PHASE_WELLWIPESEARCH_ROW10		13
#define PHASE_WELLWIPESEARCH_VIEW		14
#define PHASE_WELLWIPESEARCH_SEARCH		15
#define PHASE_WELLWIPESEARCH_FROM		16
#define PHASE_WELLWIPESEARCH_TO			17
#define PHASE_WELLWIPESEARCH_CRITERIA	18
#define PHASE_WELLWIPESEARCH_PRINT		19

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
extern WELLWIPE AmuletWellWipeReportMenu_wellWipe;

time_t AmuletWellWipeSearch_from;
time_t AmuletWellWipeSearch_to;
int AmuletWellWipeSearch_wipeType;
static time_t AmuletWellWipeSearch_fromSearched;
static time_t AmuletWellWipeSearch_toSearched;
static int AmuletWellWipeSearch_wipeTypeSearched;

static int AmuletWellWipeSearch_wipeCount;
static int AmuletWellWipeSearch_currentPage;	// Zero based
static int AmuletWellWipeSearch_lastPage;		// Zero based
static int AmuletWellWipeSearch_currentIndex;	// Zero based
static WELLWIPESEARCH AmuletWellWipeSearch_wipe[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
static void AmuletWellWipeSearch_displayPage(void);
static void AmuletWellWipeSearch_print(void);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);

enum peak_type{
	NOT_FOUND,
	PRIMARY,
	SECONDARY,
	TERTIARY,
	SUMMED
};

void AmuletWellWipeSearch_menu(void){
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLWIPESEARCH_PRE_INIT:
			if(m_ucClear == 62){
				AmuletWellWipeSearch_wipeCount = 0;
				AmuletWellWipeSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellWipeSearch_from = mk_time(&tmt);
				AmuletWellWipeSearch_to = mk_time(&tmt);

				AmuletWellWipeSearch_wipeType = 1;
				m_ucClear = 0;
			}

			dateout(message, &AmuletWellWipeSearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellWipeSearch_to, 4);
			SetAmuletString(132, message);

			switch(AmuletWellWipeSearch_wipeType){
				case 1:
					strcpy(message, "All Wipes");
					break;

				case 2:
					strcpy(message, "Work Area");
					break;

				case 3:
					strcpy(message, "Unrestricted Area");
					break;

				case 4:
					strcpy(message, "Sealed Source");
					break;

				case 5:
					strcpy(message, "Package");
					break;

				case 6:
					strcpy(message, "High Activity");
					break;
			}
			SetAmuletString(133, message);
			AmuletWellWipeSearch_displayPage();
			SetAmuletByte(99, 0xFF);
			//if(AmuletWellWipeSearch_wipeCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellWipeSearch_wipeCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			break;

		case PHASE_WELLWIPESEARCH_WAIT:
			break;

		case PHASE_WELLWIPESEARCH_PGDOWN:
			beep_amulet();
			AmuletWellWipeSearch_currentPage++;
			AmuletWellWipeSearch_displayPage();
			m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			break;

		case PHASE_WELLWIPESEARCH_PGUP:
			beep_amulet();
			if(AmuletWellWipeSearch_currentPage){
				AmuletWellWipeSearch_currentPage--;
				AmuletWellWipeSearch_displayPage();
			}
			m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			break;

		case PHASE_WELLWIPESEARCH_ROW1:
		case PHASE_WELLWIPESEARCH_ROW2:
		case PHASE_WELLWIPESEARCH_ROW3:
		case PHASE_WELLWIPESEARCH_ROW4:
		case PHASE_WELLWIPESEARCH_ROW5:
		case PHASE_WELLWIPESEARCH_ROW6:
		case PHASE_WELLWIPESEARCH_ROW7:
		case PHASE_WELLWIPESEARCH_ROW8:
		case PHASE_WELLWIPESEARCH_ROW9:
		case PHASE_WELLWIPESEARCH_ROW10:
			beep_amulet();
			AmuletWellWipeSearch_currentIndex = (10 * AmuletWellWipeSearch_currentPage) + (m_iPhase - PHASE_WELLWIPESEARCH_ROW1);
			m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			break;

		case PHASE_WELLWIPESEARCH_VIEW:
			beep_amulet();
			AmuletWellWipeReportMenu_wellWipe.WellWipeID = AmuletWellWipeSearch_wipe[AmuletWellWipeSearch_currentIndex].WellWipeID;
			DB_RetrieveWipe(&AmuletWellWipeReportMenu_wellWipe);
			SetAmuletHTML(AmuletHTMLIndex[WELLWIPEREPORT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLWIPEREPORT_HTM]);
			return;

		case PHASE_WELLWIPESEARCH_SEARCH:
			beep_amulet();
			// Search
			AmuletWellWipeSearch_fromSearched = AmuletWellWipeSearch_from;
			AmuletWellWipeSearch_toSearched = AmuletWellWipeSearch_to;
			AmuletWellWipeSearch_wipeTypeSearched = AmuletWellWipeSearch_wipeType;
			AmuletWellWipeSearch_wipeCount = DB_SearchWipes(AmuletWellWipeSearch_wipe, AmuletWellWipeSearch_from, AmuletWellWipeSearch_to, AmuletWellWipeSearch_wipeType, 100);
			AmuletWellWipeSearch_currentPage = 0;
			if(AmuletWellWipeSearch_wipeCount == -1){
				AmuletWellWipeSearch_wipeCount = 0;
				Amulet_DisplayError("Search Wipe Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellWipeSearch_wipeCount == 0){
				Amulet_DisplayError("Search Wipe Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLWIPESEARCH_PRE_INIT;
			break;

		case PHASE_WELLWIPESEARCH_FROM:
			m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			break;

		case PHASE_WELLWIPESEARCH_TO:
			m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			break;

		case PHASE_WELLWIPESEARCH_CRITERIA:
			m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			break;

		case PHASE_WELLWIPESEARCH_PRINT:
			beep_amulet();
			AmuletWellWipeSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLWIPESEARCH_WAIT;
			m_iPhase = PHASE_WELLWIPESEARCH_PRE_INIT;
			break;
	}
}

static void AmuletWellWipeSearch_displayPage(void){
	int index;
	char message[26];
	char wellwipelocationname[31];
	char wellwipetypename[31];
	char measuredon[31];

	AmuletWellWipeSearch_lastPage = AmuletWellWipeSearch_wipeCount / 10;
	if(AmuletWellWipeSearch_wipeCount % 10) AmuletWellWipeSearch_lastPage++;
	AmuletWellWipeSearch_lastPage--;
	if(AmuletWellWipeSearch_lastPage < 0) AmuletWellWipeSearch_lastPage = 0;

	if(AmuletWellWipeSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellWipeSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellWipeSearch_currentPage == AmuletWellWipeSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellWipeSearch_currentPage + index) < AmuletWellWipeSearch_wipeCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellWipeSearch_wipe[10 * AmuletWellWipeSearch_currentPage + index].Inactive){
				wellwipelocationname[0] = 4;
				wellwipelocationname[1] = 0;
				wellwipetypename[0] = 4;
				wellwipetypename[1] = 0;
				measuredon[0] = 4;
				measuredon[1] = 0;
			}else{
				wellwipelocationname[0] = 0;
				wellwipetypename[0] = 0;
				measuredon[0] = 0;
			}
			strcat(wellwipelocationname, AmuletWellWipeSearch_wipe[10 * AmuletWellWipeSearch_currentPage + index].WellWipeLocationName);
			SetAmuletString(3*index + 100, wellwipelocationname);
			strcat(wellwipetypename, AmuletWellWipeSearch_wipe[10 * AmuletWellWipeSearch_currentPage + index].WellWipeTypeName);
			SetAmuletString(3*index + 101, wellwipetypename);
			GetExtendedTimeInfoSec(&AmuletWellWipeSearch_wipe[10 * AmuletWellWipeSearch_currentPage + index].MeasuredOn, message);
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
	if(AmuletWellWipeSearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellWipeSearch_currentPage + 1, AmuletWellWipeSearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	for(index=0; index<10; index++){
		if((10 * AmuletWellWipeSearch_currentPage + index) < AmuletWellWipeSearch_wipeCount){
			if(AmuletWellWipeSearch_wipe[10 * AmuletWellWipeSearch_currentPage + index].OverallHighActivity){
				SetAmuletByte(116 + index, 0xFF);
			}
		}
	}

	AmuletWellWipeSearch_currentIndex = -1;
}

static void AmuletWellWipeSearch_print(void){
	char prtype;
	char strng[90], buf[20];
	int index, jndex, kndex;

	prtype = current.printer;

	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "Wipe Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 8, 1, 0, "From:");
		inserttime_tsec(strng, 10, 0, 0, AmuletWellWipeSearch_fromSearched, TRUE);
		pr_write2sec(strng, "Wipe Search");

		insertconst(strng, 8, 1, 0, "To:");
		inserttime_tsec(strng, 10, 0, 0, AmuletWellWipeSearch_toSearched, TRUE);
		pr_write2sec(strng, "Wipe Search");

		insertconst(strng, 8, 1, 0, "Criteria:");
		switch(AmuletWellWipeSearch_wipeTypeSearched){
			case 1:
				insertconst(strng, 10, 0, 0, "All Wipes");
				break;

			case 2:
				insertconst(strng, 10, 0, 0, "Work Area");
				break;

			case 3:
				insertconst(strng, 10, 0, 0, "Unrestricted Area");
				break;

			case 4:
				insertconst(strng, 10, 0, 0, "Sealed Source");
				break;

			case 5:
				insertconst(strng, 10, 0, 0, "Package");
				break;

			case 6:
				insertconst(strng, 10, 0, 0, "High Activity");
				break;
		}
		pr_write2sec(strng, "Wipe Search");

		for(index=0; index<AmuletWellWipeSearch_wipeCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellWipeReportMenu_wellWipe.WellWipeID = AmuletWellWipeSearch_wipe[index].WellWipeID;
			DB_RetrieveWipe(&AmuletWellWipeReportMenu_wellWipe);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2sec(strng, "Wipe Search");

			inserttime_tsec(strng, 30, 1, 0, AmuletWellWipeReportMenu_wellWipe.Spectrum.MeasuredOn, FALSE);
			pr_write2sec(strng, "Wipe Search");

			insertconst(strng, 8, 1, 0, "Location:");
			insertconst(strng, 10, 0, 0, AmuletWellWipeSearch_wipe[index].WellWipeLocationName);
			pr_write2sec(strng, "Wipe Search");

			insertconst(strng, 8, 1, 0, "Type:");
			insertconst(strng, 10, 0, 0, AmuletWellWipeSearch_wipe[index].WellWipeTypeName);
			pr_write2sec(strng, "Wipe Search");

			if(AmuletWellWipeReportMenu_wellWipe.Inactive){
				insertconst(strng, 7, 1, 0, "INACTIVE");
				if(strlen(AmuletWellWipeReportMenu_wellWipe.InactiveReason) > 0){
					insertstring(strng, 8, 0, 0, ": %s", AmuletWellWipeReportMenu_wellWipe.InactiveReason);
				}
				pr_write2sec(strng, "Wipe Search");
			}else{
				insertconst(strng, 10, 1, 0, "Background:");
				if(current.system == CI){
					insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Background.Spectrum.TotalCPM);
					insertconst(strng, 26, 0, 3, "cpm");
				}else{
					insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Background.Spectrum.TotalCPM/60.0);
					insertconst(strng, 26, 0, 3, "cps");
				}
				pr_write2sec(strng, "Wipe Search");

				insertconst(strng, 8, 1, 0, "Total:");
				if(current.system == CI){
					insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Spectrum.TotalCPM);
					insertconst(strng, 26, 0, 3, "cpm");
				}else{
					insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Spectrum.TotalCPM/60.0);
					insertconst(strng, 26, 0, 3, "cps");
				}
				pr_write2sec(strng, "Wipe Search");

				insertconst(strng, 8, 1, 0, "Activity:");
				if(current.system == CI){
					// Convert to nCi for Sealed
					if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4){
						insertfloat(strng, 24, 1, 9, "%.2f", Mca_convertDpmToCi(AmuletWellWipeReportMenu_wellWipe.Activity) * 1e+9);
						insertconst(strng, 26, 0, 3, "nCi");

					}else{
						insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Activity);
						insertconst(strng, 26, 0, 3, "dpm");
					}
				}else{
					insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Activity/60.0);
					insertconst(strng, 26, 0, 2, "Bq");
				}
				if(AmuletWellWipeReportMenu_wellWipe.ExceedsThreshold) insertconst(strng, 33, 1, 4, "HIGH");
				pr_write2sec(strng, "Wipe Search");

				for(jndex=0; jndex<40; jndex++){
					if((AmuletWellWipeReportMenu_wellWipe.Peaks[jndex].Energy > 0) && (AmuletWellWipeReportMenu_wellWipe.Peaks[jndex].PeakType == PRIMARY) && (AmuletWellWipeReportMenu_wellWipe.Peaks[jndex].ExceedsThreshold)){
						buf[0] = 0;
						for(kndex=0; kndex<10; kndex++){
							if(AmuletWellWipeReportMenu_wellWipe.Peaks[jndex].WellWipeNuclideID == AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[kndex].WellWipeNuclideID){
								strcpy(buf, AmuletWellWipeReportMenu_wellWipe.WellWipeNuclide[kndex].Name);
							}
						}

						if(strlen(buf) > 0){
							insertstring(strng, 8, 1, 0, "%s:", buf);
							if(current.system == CI){
								// Convert to nCi for Sealed
								if(AmuletWellWipeReportMenu_wellWipe.WipeLocation.WellWipeTypeGroupID == 4){
									insertfloat(strng, 24, 1, 9, "%.2f", Mca_convertDpmToCi(AmuletWellWipeReportMenu_wellWipe.Peaks[jndex].Activity) * 1e+9);
									insertconst(strng, 26, 0, 3, "nCi");

								}else{
									insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Peaks[jndex].Activity);
									insertconst(strng, 26, 0, 3, "dpm");
								}
							}else{
								insertfloat(strng, 24, 1, 9, "%.0f", AmuletWellWipeReportMenu_wellWipe.Peaks[jndex].Activity/60.0);
								insertconst(strng, 26, 0, 2, "Bq");
							}
							insertconst(strng, 33, 1, 4, "HIGH");
							pr_write2sec(strng, "Wipe Search");
						}
					}
				}
			}
		}
		formfeed(prtype);
	}
}

void AmuletWellWipeSearch_inactivate(long long int WellWipeID){
	int index;

	if(AmuletWellWipeSearch_wipeCount > 0){
		for(index=0; index<AmuletWellWipeSearch_wipeCount; index++){
			if(AmuletWellWipeSearch_wipe[index].WellWipeID == WellWipeID){
				AmuletWellWipeSearch_wipe[index].Inactive = TRUE;
				break;
			}
		}
	}
}
