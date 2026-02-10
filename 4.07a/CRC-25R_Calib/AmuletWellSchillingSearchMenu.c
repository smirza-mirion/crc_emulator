#define PHASE_WELLSCHILLINGSEARCH_PRE_INIT	0
#define PHASE_WELLSCHILLINGSEARCH_WAIT		1
#define PHASE_WELLSCHILLINGSEARCH_PGDOWN		2
#define PHASE_WELLSCHILLINGSEARCH_PGUP		3
#define PHASE_WELLSCHILLINGSEARCH_ROW1		4
#define PHASE_WELLSCHILLINGSEARCH_ROW2		5
#define PHASE_WELLSCHILLINGSEARCH_ROW3		6
#define PHASE_WELLSCHILLINGSEARCH_ROW4		7
#define PHASE_WELLSCHILLINGSEARCH_ROW5		8
#define PHASE_WELLSCHILLINGSEARCH_ROW6		9
#define PHASE_WELLSCHILLINGSEARCH_ROW7		10
#define PHASE_WELLSCHILLINGSEARCH_ROW8		11
#define PHASE_WELLSCHILLINGSEARCH_ROW9		12
#define PHASE_WELLSCHILLINGSEARCH_ROW10		13
#define PHASE_WELLSCHILLINGSEARCH_VIEW		14
#define PHASE_WELLSCHILLINGSEARCH_SEARCH		15
#define PHASE_WELLSCHILLINGSEARCH_FROM		16
#define PHASE_WELLSCHILLINGSEARCH_TO			17
#define PHASE_WELLSCHILLINGSEARCH_CRITERIA	18
#define PHASE_WELLSCHILLINGSEARCH_PRINT		19
#define PHASE_WELLSCHILLINGSEARCH_HOME		20
#define PHASE_WELLSCHILLINGSEARCH_BACK		21

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
extern WELLSCHILLINGTEST AmuletWellSchillingReport_test;

time_t AmuletWellSchillingSearch_from;
time_t AmuletWellSchillingSearch_to;
bool AmuletWellSchillingSearch_accepted;

static time_t AmuletWellSchillingSearch_fromSearched;
static time_t AmuletWellSchillingSearch_toSearched;
static int AmuletWellSchillingSearch_testCount;
static int AmuletWellSchillingSearch_currentPage;	// Zero based
static int AmuletWellSchillingSearch_lastPage;		// Zero based
static int AmuletWellSchillingSearch_currentIndex;	// Zero based
static WELLSCHILLINGSEARCH AmuletWellSchillingSearch_test[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void SetAmuletHomeHTML(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellSchillingSearch_displayPage(void);
static void AmuletWellSchillingSearch_print(void);

void AmuletWellSchillingSearch_menu(void){
	USER guestuser;
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLSCHILLINGSEARCH_PRE_INIT:
			DB_GetUser("Guest", &guestuser);

			if(m_ucClear == 68){
				AmuletWellSchillingSearch_testCount = 0;
				AmuletWellSchillingSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellSchillingSearch_from = mk_time(&tmt);
				AmuletWellSchillingSearch_to = mk_time(&tmt);

				m_ucClear = 0;

				if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 0;
					m_ucClear = 75;
					SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
					PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
					AmuletWellSchillingSearch_accepted = FALSE;
					return;
				}else{
					AmuletWellSchillingSearch_accepted = TRUE;
				}
			}

			if(!AmuletWellSchillingSearch_accepted || (guestuser.UserID == current.user_id)){
				SetAmuletBackHTML();
				return;
			}

			dateout(message, &AmuletWellSchillingSearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellSchillingSearch_to, 4);
			SetAmuletString(132, message);

			AmuletWellSchillingSearch_displayPage();
			SetAmuletByte(99, 0xFF);
			//if(AmuletWellSchillingSearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellSchillingSearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_WAIT:
			break;

		case PHASE_WELLSCHILLINGSEARCH_PGDOWN:
			beep_amulet();
			AmuletWellSchillingSearch_currentPage++;
			AmuletWellSchillingSearch_displayPage();
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_PGUP:
			beep_amulet();
			if(AmuletWellSchillingSearch_currentPage){
				AmuletWellSchillingSearch_currentPage--;
				AmuletWellSchillingSearch_displayPage();
			}
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_ROW1:
		case PHASE_WELLSCHILLINGSEARCH_ROW2:
		case PHASE_WELLSCHILLINGSEARCH_ROW3:
		case PHASE_WELLSCHILLINGSEARCH_ROW4:
		case PHASE_WELLSCHILLINGSEARCH_ROW5:
		case PHASE_WELLSCHILLINGSEARCH_ROW6:
		case PHASE_WELLSCHILLINGSEARCH_ROW7:
		case PHASE_WELLSCHILLINGSEARCH_ROW8:
		case PHASE_WELLSCHILLINGSEARCH_ROW9:
		case PHASE_WELLSCHILLINGSEARCH_ROW10:
			beep_amulet();
			AmuletWellSchillingSearch_currentIndex = (10 * AmuletWellSchillingSearch_currentPage) + (m_iPhase - PHASE_WELLSCHILLINGSEARCH_ROW1);
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_VIEW:
			beep_amulet();
			AmuletWellSchillingReport_test.WellSchillingTestID = AmuletWellSchillingSearch_test[AmuletWellSchillingSearch_currentIndex].WellSchillingTestID;
			DB_RetrieveSchillingTest(&AmuletWellSchillingReport_test);
			SetAmuletHTML(AmuletHTMLIndex[WELLSCHILLINGREPORT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLSCHILLINGREPORT_HTM]);
			return;

		case PHASE_WELLSCHILLINGSEARCH_SEARCH:
			beep_amulet();
			AmuletWellSchillingSearch_fromSearched = AmuletWellSchillingSearch_from;
			AmuletWellSchillingSearch_toSearched = AmuletWellSchillingSearch_to;
			AmuletWellSchillingSearch_testCount = DB_SearchSchillingTests(AmuletWellSchillingSearch_test, AmuletWellSchillingSearch_from, AmuletWellSchillingSearch_to, 100);
			AmuletWellSchillingSearch_currentPage = 0;
			if(AmuletWellSchillingSearch_testCount == -1){
				AmuletWellSchillingSearch_testCount = 0;
				Amulet_DisplayError("Search Schilling Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellSchillingSearch_testCount == 0){
				Amulet_DisplayError("Search Schilling Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_PRE_INIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_FROM:
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_TO:
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_CRITERIA:
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_PRINT:
			beep_amulet();
			AmuletWellSchillingSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLSCHILLINGSEARCH_WAIT;
			m_iPhase = PHASE_WELLSCHILLINGSEARCH_PRE_INIT;
			break;

		case PHASE_WELLSCHILLINGSEARCH_HOME:
			beep_amulet();
			if(current.security_mode == 2){
				DB_GetUser("Guest", &guestuser);
				current.user_id = guestuser.UserID;
			}
			SetAmuletHomeHTML();
			return;

		case PHASE_WELLSCHILLINGSEARCH_BACK:
			if(current.security_mode == 2){
				DB_GetUser("Guest", &guestuser);
				current.user_id = guestuser.UserID;
			}
			SetAmuletBackHTML();
			return;
	}
}

static void AmuletWellSchillingSearch_displayPage(void){
	int index;
	char message[26];
	char lastname[31];
	char firstname[31];
	char measuredon[31];

	AmuletWellSchillingSearch_lastPage = AmuletWellSchillingSearch_testCount / 10;
	if(AmuletWellSchillingSearch_testCount % 10) AmuletWellSchillingSearch_lastPage++;
	AmuletWellSchillingSearch_lastPage--;
	if(AmuletWellSchillingSearch_lastPage < 0) AmuletWellSchillingSearch_lastPage = 0;

	if(AmuletWellSchillingSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellSchillingSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellSchillingSearch_currentPage == AmuletWellSchillingSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellSchillingSearch_currentPage + index) < AmuletWellSchillingSearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellSchillingSearch_test[10 * AmuletWellSchillingSearch_currentPage + index].Inactive){
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
			strcat(lastname, AmuletWellSchillingSearch_test[10 * AmuletWellSchillingSearch_currentPage + index].LastName);
			SetAmuletString(3*index + 100, lastname);

			strcat(firstname, AmuletWellSchillingSearch_test[10 * AmuletWellSchillingSearch_currentPage + index].FirstName);
			SetAmuletString(3*index + 101, firstname);

			GetExtendedTimeInfo(&AmuletWellSchillingSearch_test[10 * AmuletWellSchillingSearch_currentPage + index].MeasuredOn, message);
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
	if(AmuletWellSchillingSearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellSchillingSearch_currentPage + 1, AmuletWellSchillingSearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellSchillingSearch_currentIndex = -1;
}

static void AmuletWellSchillingSearch_print(void){
	char prtype;
	char strng[90];
	int index;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "Schilling Search" , clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 4, 1, 0, "From:");
		inserttime_t(strng, 6, 0, 0, AmuletWellSchillingSearch_fromSearched, TRUE);
		pr_write2(strng, "Schilling Search");

		insertconst(strng, 4, 1, 0, "To:");
		inserttime_t(strng, 6, 0, 0, AmuletWellSchillingSearch_toSearched, TRUE);
		pr_write2(strng, "Schilling Search");

		for(index=0; index<AmuletWellSchillingSearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellSchillingReport_test.WellSchillingTestID = AmuletWellSchillingSearch_test[index].WellSchillingTestID;
			DB_RetrieveSchillingTest(&AmuletWellSchillingReport_test);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "Schilling Search");

			inserttime_t(strng, 33, 1, 0, AmuletWellSchillingReport_test.CreatedOn, FALSE);
			pr_write2(strng, "Schilling Search");

			insertconst(strng, 9, 1, 0, "First:");
			insertconst(strng, 11, 0, 0, AmuletWellSchillingReport_test.TestIdent.FirstName);
			pr_write2(strng, "Schilling Search");

			insertconst(strng, 9, 1, 0, "Last:");
			insertconst(strng, 11, 0, 0, AmuletWellSchillingReport_test.TestIdent.LastName);
			pr_write2(strng, "Schilling Search");

			if(AmuletWellSchillingReport_test.Inactive){
				insertconst(strng, 8, 1, 0, "INACTIVE");
				if(strlen(AmuletWellSchillingReport_test.InactiveReason) > 0){
					insertstring(strng, 9, 0, 0, ": %s", AmuletWellSchillingReport_test.InactiveReason);
				}
				pr_write2(strng, "Schilling Search");
			}else{
				insertconst(strng, 9, 1, 0, "Excretion:");
				insertfloat(strng, 11, 0, 0, "%.1f %%", AmuletWellSchillingReport_test.Excretion);
				pr_write2(strng, "Schilling Search");
			}

		}
		formfeed(prtype);
	}
}

void AmuletWellSchillingSearch_inactivate(long long int WellSchillingTestID){
	int index;

	if(AmuletWellSchillingSearch_testCount > 0){
		for(index=0; index<AmuletWellSchillingSearch_testCount; index++){
			if(AmuletWellSchillingSearch_test[index].WellSchillingTestID == WellSchillingTestID){
				AmuletWellSchillingSearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
