#define PHASE_WELLRBCSEARCH_PRE_INIT	0
#define PHASE_WELLRBCSEARCH_WAIT		1
#define PHASE_WELLRBCSEARCH_PGDOWN		2
#define PHASE_WELLRBCSEARCH_PGUP		3
#define PHASE_WELLRBCSEARCH_ROW1		4
#define PHASE_WELLRBCSEARCH_ROW2		5
#define PHASE_WELLRBCSEARCH_ROW3		6
#define PHASE_WELLRBCSEARCH_ROW4		7
#define PHASE_WELLRBCSEARCH_ROW5		8
#define PHASE_WELLRBCSEARCH_ROW6		9
#define PHASE_WELLRBCSEARCH_ROW7		10
#define PHASE_WELLRBCSEARCH_ROW8		11
#define PHASE_WELLRBCSEARCH_ROW9		12
#define PHASE_WELLRBCSEARCH_ROW10		13
#define PHASE_WELLRBCSEARCH_VIEW		14
#define PHASE_WELLRBCSEARCH_SEARCH		15
#define PHASE_WELLRBCSEARCH_FROM		16
#define PHASE_WELLRBCSEARCH_TO			17
#define PHASE_WELLRBCSEARCH_CRITERIA	18
#define PHASE_WELLRBCSEARCH_PRINT		19
#define PHASE_WELLRBCSEARCH_HOME		20
#define PHASE_WELLRBCSEARCH_BACK		21

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
extern WELLRBCTEST AmuletWellRBCReportMenu_test;

time_t AmuletWellRBCSearch_from;
time_t AmuletWellRBCSearch_to;
bool AmuletWellRBCSearch_accepted;

static time_t AmuletWellRBCSearch_fromSearched;
static time_t AmuletWellRBCSearch_toSearched;
static int AmuletWellRBCSearch_testCount;
static int AmuletWellRBCSearch_currentPage;	// Zero based
static int AmuletWellRBCSearch_lastPage;		// Zero based
static int AmuletWellRBCSearch_currentIndex;	// Zero based
static WELLRBCSEARCH AmuletWellRBCSearch_test[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void SetAmuletHomeHTML(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellRBCSearch_displayPage(void);
static void AmuletWellRBCSearch_print(void);

void AmuletWellRBCSearch_menu(void){
	USER guestuser;
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLRBCSEARCH_PRE_INIT:
			DB_GetUser("Guest", &guestuser);

			if(m_ucClear == 70){
				AmuletWellRBCSearch_testCount = 0;
				AmuletWellRBCSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellRBCSearch_from = mk_time(&tmt);
				AmuletWellRBCSearch_to = mk_time(&tmt);

				m_ucClear = 0;

				if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 0;
					m_ucClear = 75;
					SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
					PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
					AmuletWellRBCSearch_accepted = FALSE;
					return;
				}else{
					AmuletWellRBCSearch_accepted = TRUE;
				}
			}

			if(!AmuletWellRBCSearch_accepted || (guestuser.UserID == current.user_id)){
				SetAmuletBackHTML();
				return;
			}

			dateout(message, &AmuletWellRBCSearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellRBCSearch_to, 4);
			SetAmuletString(132, message);

			AmuletWellRBCSearch_displayPage();
			SetAmuletByte(99, 0xFF);
			//if(AmuletWellRBCSearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellRBCSearch_testCount && (current.printer != NONE_PRINTER) &&( current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			break;

		case PHASE_WELLRBCSEARCH_WAIT:
			break;

		case PHASE_WELLRBCSEARCH_PGDOWN:
			beep_amulet();
			AmuletWellRBCSearch_currentPage++;
			AmuletWellRBCSearch_displayPage();
			m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			break;

		case PHASE_WELLRBCSEARCH_PGUP:
			beep_amulet();
			if(AmuletWellRBCSearch_currentPage){
				AmuletWellRBCSearch_currentPage--;
				AmuletWellRBCSearch_displayPage();
			}
			m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			break;

		case PHASE_WELLRBCSEARCH_ROW1:
		case PHASE_WELLRBCSEARCH_ROW2:
		case PHASE_WELLRBCSEARCH_ROW3:
		case PHASE_WELLRBCSEARCH_ROW4:
		case PHASE_WELLRBCSEARCH_ROW5:
		case PHASE_WELLRBCSEARCH_ROW6:
		case PHASE_WELLRBCSEARCH_ROW7:
		case PHASE_WELLRBCSEARCH_ROW8:
		case PHASE_WELLRBCSEARCH_ROW9:
		case PHASE_WELLRBCSEARCH_ROW10:
			beep_amulet();
			AmuletWellRBCSearch_currentIndex = (10 * AmuletWellRBCSearch_currentPage) + (m_iPhase - PHASE_WELLRBCSEARCH_ROW1);
			m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			break;

		case PHASE_WELLRBCSEARCH_VIEW:
			beep_amulet();
			AmuletWellRBCReportMenu_test.WellRBCTestID = AmuletWellRBCSearch_test[AmuletWellRBCSearch_currentIndex].WellRBCTestID;
			DB_RetrieveRBCTest(&AmuletWellRBCReportMenu_test);
			SetAmuletHTML(AmuletHTMLIndex[WELLRBCREPORT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLRBCREPORT_HTM]);
			return;

		case PHASE_WELLRBCSEARCH_SEARCH:
			beep_amulet();
			AmuletWellRBCSearch_fromSearched = AmuletWellRBCSearch_from;
			AmuletWellRBCSearch_toSearched = AmuletWellRBCSearch_to;
			AmuletWellRBCSearch_testCount = DB_SearchRBCTests(AmuletWellRBCSearch_test, AmuletWellRBCSearch_from, AmuletWellRBCSearch_to, 100);
			AmuletWellRBCSearch_currentPage = 0;
			if(AmuletWellRBCSearch_testCount == -1){
				AmuletWellRBCSearch_testCount = 0;
				Amulet_DisplayError("Search RBC Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellRBCSearch_testCount == 0){
				Amulet_DisplayError("Search RBC Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLRBCSEARCH_PRE_INIT;
			break;

		case PHASE_WELLRBCSEARCH_FROM:
			m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			break;

		case PHASE_WELLRBCSEARCH_TO:
			m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			break;

		case PHASE_WELLRBCSEARCH_CRITERIA:
			m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			break;

		case PHASE_WELLRBCSEARCH_PRINT:
			beep_amulet();
			AmuletWellRBCSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLRBCSEARCH_WAIT;
			m_iPhase = PHASE_WELLRBCSEARCH_PRE_INIT;
			break;

		case PHASE_WELLRBCSEARCH_HOME:
			beep_amulet();
			if(current.security_mode == 2){
				DB_GetUser("Guest", &guestuser);
				current.user_id = guestuser.UserID;
			}
			SetAmuletHomeHTML();
			return;

		case PHASE_WELLRBCSEARCH_BACK:
			if(current.security_mode == 2){
				DB_GetUser("Guest", &guestuser);
				current.user_id = guestuser.UserID;
			}
			SetAmuletBackHTML();
			return;
	}
}

static void AmuletWellRBCSearch_displayPage(void){
	int index;
	char message[26];
	char lastname[31];
	char firstname[31];
	char measuredon[31];

	AmuletWellRBCSearch_lastPage = AmuletWellRBCSearch_testCount / 10;
	if(AmuletWellRBCSearch_testCount % 10) AmuletWellRBCSearch_lastPage++;
	AmuletWellRBCSearch_lastPage--;
	if(AmuletWellRBCSearch_lastPage < 0) AmuletWellRBCSearch_lastPage = 0;

	if(AmuletWellRBCSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellRBCSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellRBCSearch_currentPage == AmuletWellRBCSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellRBCSearch_currentPage + index) < AmuletWellRBCSearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellRBCSearch_test[10 * AmuletWellRBCSearch_currentPage + index].Inactive){
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
			strcat(lastname, AmuletWellRBCSearch_test[10 * AmuletWellRBCSearch_currentPage + index].LastName);
			SetAmuletString(3*index + 100, lastname);

			strcat(firstname, AmuletWellRBCSearch_test[10 * AmuletWellRBCSearch_currentPage + index].FirstName);
			SetAmuletString(3*index + 101, firstname);

			GetExtendedTimeInfo(&AmuletWellRBCSearch_test[10 * AmuletWellRBCSearch_currentPage + index].MeasuredOn, message);
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
	if(AmuletWellRBCSearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellRBCSearch_currentPage + 1, AmuletWellRBCSearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellRBCSearch_currentIndex = -1;
}

static void AmuletWellRBCSearch_print(void){
	char prtype;
	char strng[90];
	int index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "RBC Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 4, 1, 0, "From:");
		inserttime_t(strng, 6, 0, 0, AmuletWellRBCSearch_fromSearched, TRUE);
		pr_write2(strng, "RBC Search");

		insertconst(strng, 4, 1, 0, "To:");
		inserttime_t(strng, 6, 0, 0, AmuletWellRBCSearch_toSearched, TRUE);
		pr_write2(strng, "RBC Search");

		for(index=0; index<AmuletWellRBCSearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellRBCReportMenu_test.WellRBCTestID = AmuletWellRBCSearch_test[index].WellRBCTestID;
			DB_RetrieveRBCTest(&AmuletWellRBCReportMenu_test);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "RBC Search");

			inserttime_t(strng, 33, 1, 0, AmuletWellRBCReportMenu_test.CreatedOn, FALSE);
			pr_write2(strng, "RBC Search");

			insertconst(strng, 9, 1, 0, "First:");
			insertconst(strng, 11, 0, 0, AmuletWellRBCReportMenu_test.TestIdent.FirstName);
			pr_write2(strng, "RBC Search");

			insertconst(strng, 9, 1, 0, "Last:");
			insertconst(strng, 11, 0, 0, AmuletWellRBCReportMenu_test.TestIdent.LastName);
			pr_write2(strng, "RBC Search");

			if(AmuletWellRBCReportMenu_test.Inactive){
				insertconst(strng, 9, 1, 0, "INACTIVE");
				if(strlen(AmuletWellRBCReportMenu_test.InactiveReason) > 0){
					insertstring(strng, 10, 0, 0, ": %s", AmuletWellRBCReportMenu_test.InactiveReason);
				}
				pr_write2(strng, "RBC Search");
			}else{
				insertconst(strng, 10, 1, 0, "RBC Vol:");
				insertfloat(strng, 30, 1, 0, "%.0f", AmuletWellRBCReportMenu_test.RBCVolume);
				insertconst(strng, 33, 1, 0, "ml");
				pr_write2(strng, "RBC Search");

				insertconst(strng, 10, 1, 0, "Blood Vol:");
				insertfloat(strng, 30, 1, 0, "%.0f", AmuletWellRBCReportMenu_test.WholeBloodVolume);
				insertconst(strng, 33, 1, 0, "ml");
				pr_write2(strng, "RBC Search");

				insertconst(strng, 10, 1, 0, "Plasma Vol:");
				insertfloat(strng, 30, 1, 0, "%.0f", AmuletWellRBCReportMenu_test.PlasmaVolume);
				insertconst(strng, 33, 1, 0, "ml");
				pr_write2(strng, "RBC Search");
			}
		}
		formfeed(prtype);
	}
}

void AmuletWellRBCSearch_inactivate(long long int WellRBCTestID){
	int index;

	if(AmuletWellRBCSearch_testCount > 0){
		for(index=0; index<AmuletWellRBCSearch_testCount; index++){
			if(AmuletWellRBCSearch_test[index].WellRBCTestID == WellRBCTestID){
				AmuletWellRBCSearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
