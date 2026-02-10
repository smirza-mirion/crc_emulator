#define PHASE_WELLPLASMASEARCH_PRE_INIT	0
#define PHASE_WELLPLASMASEARCH_WAIT		1
#define PHASE_WELLPLASMASEARCH_PGDOWN		2
#define PHASE_WELLPLASMASEARCH_PGUP		3
#define PHASE_WELLPLASMASEARCH_ROW1		4
#define PHASE_WELLPLASMASEARCH_ROW2		5
#define PHASE_WELLPLASMASEARCH_ROW3		6
#define PHASE_WELLPLASMASEARCH_ROW4		7
#define PHASE_WELLPLASMASEARCH_ROW5		8
#define PHASE_WELLPLASMASEARCH_ROW6		9
#define PHASE_WELLPLASMASEARCH_ROW7		10
#define PHASE_WELLPLASMASEARCH_ROW8		11
#define PHASE_WELLPLASMASEARCH_ROW9		12
#define PHASE_WELLPLASMASEARCH_ROW10		13
#define PHASE_WELLPLASMASEARCH_VIEW		14
#define PHASE_WELLPLASMASEARCH_SEARCH		15
#define PHASE_WELLPLASMASEARCH_FROM		16
#define PHASE_WELLPLASMASEARCH_TO			17
#define PHASE_WELLPLASMASEARCH_CRITERIA	18
#define PHASE_WELLPLASMASEARCH_PRINT	19
#define PHASE_WELLPLASMASEARCH_HOME		20
#define PHASE_WELLPLASMASEARCH_BACK		21

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
extern bool AmuletLoginMenu_blockGuest;
extern int AmuletLoginMenu_minRole;
extern WELLPLASMATEST AmuletWellPlasmaReportMenu_test;

time_t AmuletWellPlasmaSearch_from;
time_t AmuletWellPlasmaSearch_to;
bool AmuletWellPlasmaSearch_accepted;

static time_t AmuletWellPlasmaSearch_fromSearched;
static time_t AmuletWellPlasmaSearch_toSearched;
static int AmuletWellPlasmaSearch_testCount;
static int AmuletWellPlasmaSearch_currentPage;	// Zero based
static int AmuletWellPlasmaSearch_lastPage;		// Zero based
static int AmuletWellPlasmaSearch_currentIndex;	// Zero based
static WELLPLASMASEARCH AmuletWellPlasmaSearch_test[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void SetAmuletHomeHTML(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellPlasmaSearch_displayPage(void);
static void AmuletWellPlasmaSearch_print(void);

void AmuletWellPlasmaSearch_menu(void){
	USER guestuser;
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLPLASMASEARCH_PRE_INIT:
			DB_GetUser("Guest", &guestuser);

			if(m_ucClear == 69){
				AmuletWellPlasmaSearch_testCount = 0;
				AmuletWellPlasmaSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellPlasmaSearch_from = mk_time(&tmt);
				AmuletWellPlasmaSearch_to = mk_time(&tmt);

				m_ucClear = 0;

				if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 0;
					m_ucClear = 75;
					SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
					PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
					AmuletWellPlasmaSearch_accepted = FALSE;
					return;
				}else{
					AmuletWellPlasmaSearch_accepted = TRUE;
				}

			}

			if(!AmuletWellPlasmaSearch_accepted || (guestuser.UserID == current.user_id)){
				SetAmuletBackHTML();
				return;
			}
			dateout(message, &AmuletWellPlasmaSearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellPlasmaSearch_to, 4);
			SetAmuletString(132, message);

			AmuletWellPlasmaSearch_displayPage();
			SetAmuletByte(99, 0xFF);
			//if(AmuletWellPlasmaSearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellPlasmaSearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			break;

		case PHASE_WELLPLASMASEARCH_WAIT:
			break;

		case PHASE_WELLPLASMASEARCH_PGDOWN:
			beep_amulet();
			AmuletWellPlasmaSearch_currentPage++;
			AmuletWellPlasmaSearch_displayPage();
			m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			break;

		case PHASE_WELLPLASMASEARCH_PGUP:
			beep_amulet();
			if(AmuletWellPlasmaSearch_currentPage){
				AmuletWellPlasmaSearch_currentPage--;
				AmuletWellPlasmaSearch_displayPage();
			}
			m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			break;

		case PHASE_WELLPLASMASEARCH_ROW1:
		case PHASE_WELLPLASMASEARCH_ROW2:
		case PHASE_WELLPLASMASEARCH_ROW3:
		case PHASE_WELLPLASMASEARCH_ROW4:
		case PHASE_WELLPLASMASEARCH_ROW5:
		case PHASE_WELLPLASMASEARCH_ROW6:
		case PHASE_WELLPLASMASEARCH_ROW7:
		case PHASE_WELLPLASMASEARCH_ROW8:
		case PHASE_WELLPLASMASEARCH_ROW9:
		case PHASE_WELLPLASMASEARCH_ROW10:
			beep_amulet();
			AmuletWellPlasmaSearch_currentIndex = (10 * AmuletWellPlasmaSearch_currentPage) + (m_iPhase - PHASE_WELLPLASMASEARCH_ROW1);
			m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			break;

		case PHASE_WELLPLASMASEARCH_VIEW:
			beep_amulet();
			AmuletWellPlasmaReportMenu_test.WellPlasmaTestID = AmuletWellPlasmaSearch_test[AmuletWellPlasmaSearch_currentIndex].WellPlasmaTestID;
			DB_RetrievePlasmaTest(&AmuletWellPlasmaReportMenu_test);
			SetAmuletHTML(AmuletHTMLIndex[WELLPLASMAREPORT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLPLASMAREPORT_HTM]);
			return;

		case PHASE_WELLPLASMASEARCH_SEARCH:
			beep_amulet();
			AmuletWellPlasmaSearch_fromSearched = AmuletWellPlasmaSearch_from;
			AmuletWellPlasmaSearch_toSearched = AmuletWellPlasmaSearch_to;
			AmuletWellPlasmaSearch_testCount = DB_SearchPlasmaTests(AmuletWellPlasmaSearch_test, AmuletWellPlasmaSearch_from, AmuletWellPlasmaSearch_to, 100);
			AmuletWellPlasmaSearch_currentPage = 0;
			if(AmuletWellPlasmaSearch_testCount == -1){
				AmuletWellPlasmaSearch_testCount = 0;
				Amulet_DisplayError("Search Plasma Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellPlasmaSearch_testCount == 0){
				Amulet_DisplayError("Search Plasma Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLPLASMASEARCH_PRE_INIT;
			break;

		case PHASE_WELLPLASMASEARCH_FROM:
			m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			break;

		case PHASE_WELLPLASMASEARCH_TO:
			m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			break;

		case PHASE_WELLPLASMASEARCH_CRITERIA:
			m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			break;

		case PHASE_WELLPLASMASEARCH_PRINT:
			beep_amulet();
			AmuletWellPlasmaSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLPLASMASEARCH_WAIT;
			m_iPhase = PHASE_WELLPLASMASEARCH_PRE_INIT;
			break;

		case PHASE_WELLPLASMASEARCH_HOME:
			beep_amulet();
			if(current.security_mode == 2){
				DB_GetUser("Guest", &guestuser);
				current.user_id = guestuser.UserID;
			}
			SetAmuletHomeHTML();
			return;

		case PHASE_WELLPLASMASEARCH_BACK:
			if(current.security_mode == 2){
				DB_GetUser("Guest", &guestuser);
				current.user_id = guestuser.UserID;
			}
			SetAmuletBackHTML();
			return;
	}
}

static void AmuletWellPlasmaSearch_displayPage(void){
	int index;
	char message[26];
	char lastname[31];
	char firstname[31];
	char measuredon[31];

	AmuletWellPlasmaSearch_lastPage = AmuletWellPlasmaSearch_testCount / 10;
	if(AmuletWellPlasmaSearch_testCount % 10) AmuletWellPlasmaSearch_lastPage++;
	AmuletWellPlasmaSearch_lastPage--;
	if(AmuletWellPlasmaSearch_lastPage < 0) AmuletWellPlasmaSearch_lastPage = 0;

	if(AmuletWellPlasmaSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellPlasmaSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellPlasmaSearch_currentPage == AmuletWellPlasmaSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellPlasmaSearch_currentPage + index) < AmuletWellPlasmaSearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellPlasmaSearch_test[10 * AmuletWellPlasmaSearch_currentPage + index].Inactive){
				lastname[0] = 4;
				lastname[1] = 0;
				firstname[0] = 4;
				firstname[1] = 0;
				measuredon[0] = 4;
				measuredon[1] = 0;
			}else{
				lastname[0] = 0;
				firstname[0] = 0;
				measuredon[0] = 0;
			}
			strcat(lastname, AmuletWellPlasmaSearch_test[10 * AmuletWellPlasmaSearch_currentPage + index].LastName);
			SetAmuletString(3*index + 100, lastname);

			strcat(firstname, AmuletWellPlasmaSearch_test[10 * AmuletWellPlasmaSearch_currentPage + index].FirstName);
			SetAmuletString(3*index + 101, firstname);

			GetExtendedTimeInfo(&AmuletWellPlasmaSearch_test[10 * AmuletWellPlasmaSearch_currentPage + index].MeasuredOn, message);
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
	if(AmuletWellPlasmaSearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellPlasmaSearch_currentPage + 1, AmuletWellPlasmaSearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellPlasmaSearch_currentIndex = -1;
}

static void AmuletWellPlasmaSearch_print(void){
	char prtype;
	char strng[90];
	int index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "Plasma Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 4, 1, 0, "From:");
		inserttime_t(strng, 6, 0, 0, AmuletWellPlasmaSearch_fromSearched, TRUE);
		pr_write2(strng, "Plasma Search");

		insertconst(strng, 4, 1, 0, "To:");
		inserttime_t(strng, 6, 0, 0, AmuletWellPlasmaSearch_toSearched, TRUE);
		pr_write2(strng, "Plasma Search");

		for(index=0; index<AmuletWellPlasmaSearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellPlasmaReportMenu_test.WellPlasmaTestID = AmuletWellPlasmaSearch_test[index].WellPlasmaTestID;
			DB_RetrievePlasmaTest(&AmuletWellPlasmaReportMenu_test);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "Plasma Search");

			inserttime_t(strng, 33, 1, 0, AmuletWellPlasmaReportMenu_test.CreatedOn, FALSE);
			pr_write2(strng, "Plasma Search");

			insertconst(strng, 9, 1, 0, "First:");
			insertconst(strng, 11, 0, 0, AmuletWellPlasmaReportMenu_test.TestIdent.FirstName);
			pr_write2(strng, "Plasma Search");

			insertconst(strng, 9, 1, 0, "Last:");
			insertconst(strng, 11, 0, 0, AmuletWellPlasmaReportMenu_test.TestIdent.LastName);
			pr_write2(strng, "Plasma Search");

			if(AmuletWellPlasmaReportMenu_test.Inactive){
				insertconst(strng, 9, 1, 0, "INACTIVE");
				if(strlen(AmuletWellPlasmaReportMenu_test.InactiveReason) > 0){
					insertstring(strng, 10, 0, 0, ": %s", AmuletWellPlasmaReportMenu_test.InactiveReason);
				}
				pr_write2(strng, "Plasma Search");
			}else{
				insertconst(strng, 10, 1, 0, "Blood Vol:");
				insertfloat(strng, 30, 1, 0, "%.0f", AmuletWellPlasmaReportMenu_test.WholeBloodVolume);
				insertconst(strng, 33, 1, 0, "ml");
				pr_write2(strng, "Plasma Search");

				insertconst(strng, 10, 1, 0, "Plasma Vol:");
				insertfloat(strng, 30, 1, 0, "%.0f", AmuletWellPlasmaReportMenu_test.PlasmaVolume);
				insertconst(strng, 33, 1, 0, "ml");
				pr_write2(strng, "Plasma Search");

				insertconst(strng, 10, 1, 0, "RBC Vol:");
				insertfloat(strng, 30, 1, 0, "%.0f", AmuletWellPlasmaReportMenu_test.RBCVolume);
				insertconst(strng, 33, 1, 0, "ml");
				pr_write2(strng, "Plasma Search");

				insertconst(strng, 10, 1, 0, "Hematocrit:");
				insertfloat(strng, 30, 1, 0, "%.1f", AmuletWellPlasmaReportMenu_test.CalculatedHematocrit);
				insertconst(strng, 32, 0, 0, "%");
				pr_write2(strng, "Plasma Search");
			}
		}
		formfeed(prtype);
	}
}

void AmuletWellPlasmaSearch_inactivate(long long int WellPlasmaTestID){
	int index;

	if(AmuletWellPlasmaSearch_testCount > 0){
		for(index=0; index<AmuletWellPlasmaSearch_testCount; index++){
			if(AmuletWellPlasmaSearch_test[index].WellPlasmaTestID == WellPlasmaTestID){
				AmuletWellPlasmaSearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
