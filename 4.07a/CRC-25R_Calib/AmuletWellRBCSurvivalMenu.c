#define PHASE_WELLRBCSURVIVAL_PRE_INIT	0
#define PHASE_WELLRBCSURVIVAL_WAIT		1
#define PHASE_WELLRBCSURVIVAL_PGDOWN	2
#define PHASE_WELLRBCSURVIVAL_PGUP		3
#define PHASE_WELLRBCSURVIVAL_ROW1		4
#define PHASE_WELLRBCSURVIVAL_ROW2		5
#define PHASE_WELLRBCSURVIVAL_ROW3		6
#define PHASE_WELLRBCSURVIVAL_ROW4		7
#define PHASE_WELLRBCSURVIVAL_ROW5		8
#define PHASE_WELLRBCSURVIVAL_ROW6		9
#define PHASE_WELLRBCSURVIVAL_ROW7		10
#define PHASE_WELLRBCSURVIVAL_ROW8		11
#define PHASE_WELLRBCSURVIVAL_ROW9		12
#define PHASE_WELLRBCSURVIVAL_ROW10		13
#define PHASE_WELLRBCSURVIVAL_SEARCH	14
#define PHASE_WELLRBCSURVIVAL_ADD		15
#define PHASE_WELLRBCSURVIVAL_EDIT		16
#define PHASE_WELLRBCSURVIVAL_RUN		17

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
//extern time_t clock_time;
extern bool AmuletLoginMenu_blockGuest;
extern int AmuletLoginMenu_minRole;
extern WELLRBCSURVIVALTEST AmuletAddEditRBCSurvivalTest_test;
extern WELLRBCSURVIVALTEST AmuletWellRBCSurvivalTest;

bool AmuletWellRBCSurvival_runMode;
char AmuletWellRBCSurvival_lastName[18];
char AmuletWellRBCSurvival_patientID[11];
bool AmuletWellRBCSurvival_accepted;

static int AmuletWellRBCSurvival_testCount;
static int AmuletWellRBCSurvival_currentPage;	// Zero based
static int AmuletWellRBCSurvival_lastPage;		// Zero based
static int AmuletWellRBCSurvival_currentIndex;	// Zero based
static WELLRBCSURVIVALSEARCH AmuletWellRBCSurvivalSearch_test[100];
static bool AmuletWellRBCSurvival_runInitialSearch;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellRBCSurvival_displayPage(void);

void AmuletWellRBCSurvival_menu(void){
	USER guestuser;

	switch(m_iPhase){
		case PHASE_WELLRBCSURVIVAL_PRE_INIT:
			DB_GetUser("Guest", &guestuser);

			if(m_ucClear == 99){
				AmuletWellRBCSurvival_lastName[0] = 0;
				AmuletWellRBCSurvival_patientID[0] = 0;
				AmuletWellRBCSurvival_testCount = 0;
				AmuletWellRBCSurvival_runInitialSearch = TRUE;
				AmuletWellRBCSurvival_currentPage = 0;
				m_ucClear = 0;

				if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 0;
					m_ucClear = 75;
					SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
					PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
					AmuletWellRBCSurvival_accepted = FALSE;
					return;
				}else{
					AmuletWellRBCSurvival_accepted = TRUE;
				}
			}

			if(!AmuletWellRBCSurvival_accepted || (guestuser.UserID == current.user_id)){
				SetAmuletBackHTML();
				return;
			}

			if(AmuletWellRBCSurvival_runMode){
				SetAmuletByte(98, 0xFF);
				SetAmuletString(77, "RBC Survival Measurement");
				SetAmuletString(79, "Run");
				if(AmuletWellRBCSurvival_runInitialSearch){
					AmuletWellRBCSurvival_testCount = DB_SearchRBCSurvivalTests(AmuletWellRBCSurvivalSearch_test, AmuletWellRBCSurvival_lastName, AmuletWellRBCSurvival_patientID, AmuletWellRBCSurvival_runMode);
					AmuletWellRBCSurvival_runInitialSearch = FALSE;
				}
				Mca_switchToWell();
			}else{
				SetAmuletByte(98, 0x00);
				SetAmuletString(77, "RBC Survival Report");
				SetAmuletString(79, "View");
				AmuletWellRBCSurvival_runInitialSearch = FALSE;
			}

			SetAmuletString(131, AmuletWellRBCSurvival_lastName);
			SetAmuletString(132, AmuletWellRBCSurvival_patientID);

			AmuletWellRBCSurvival_displayPage();
			SetAmuletByte(99, 0xFF);
			m_iPhase = PHASE_WELLRBCSURVIVAL_WAIT;
			break;

		case PHASE_WELLRBCSURVIVAL_WAIT:
			break;

		case PHASE_WELLRBCSURVIVAL_PGDOWN:
			beep_amulet();
			AmuletWellRBCSurvival_currentPage++;
			AmuletWellRBCSurvival_displayPage();
			m_iPhase = PHASE_WELLRBCSURVIVAL_WAIT;
			break;

		case PHASE_WELLRBCSURVIVAL_PGUP:
			beep_amulet();
			if(AmuletWellRBCSurvival_currentPage){
				AmuletWellRBCSurvival_currentPage--;
				AmuletWellRBCSurvival_displayPage();
			}
			m_iPhase = PHASE_WELLRBCSURVIVAL_WAIT;
			break;

		case PHASE_WELLRBCSURVIVAL_ROW1:
		case PHASE_WELLRBCSURVIVAL_ROW2:
		case PHASE_WELLRBCSURVIVAL_ROW3:
		case PHASE_WELLRBCSURVIVAL_ROW4:
		case PHASE_WELLRBCSURVIVAL_ROW5:
		case PHASE_WELLRBCSURVIVAL_ROW6:
		case PHASE_WELLRBCSURVIVAL_ROW7:
		case PHASE_WELLRBCSURVIVAL_ROW8:
		case PHASE_WELLRBCSURVIVAL_ROW9:
		case PHASE_WELLRBCSURVIVAL_ROW10:
			beep_amulet();
			AmuletWellRBCSurvival_currentIndex = (10 * AmuletWellRBCSurvival_currentPage) + (m_iPhase - PHASE_WELLRBCSURVIVAL_ROW1);
			m_iPhase = PHASE_WELLRBCSURVIVAL_WAIT;
			break;

		case PHASE_WELLRBCSURVIVAL_SEARCH:
			beep_amulet();
			AmuletWellRBCSurvival_testCount = DB_SearchRBCSurvivalTests(AmuletWellRBCSurvivalSearch_test, AmuletWellRBCSurvival_lastName, AmuletWellRBCSurvival_patientID, AmuletWellRBCSurvival_runMode);
			AmuletWellRBCSurvival_currentPage = 0;
			if(AmuletWellRBCSurvival_testCount == 0){
				Amulet_DisplayError("RBC Survival", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLRBCSURVIVAL_PRE_INIT;
			break;

		case PHASE_WELLRBCSURVIVAL_ADD:
			beep_amulet();
			AmuletWellRBCSurvival_lastName[0] = 0;
			AmuletWellRBCSurvival_patientID[0] = 0;
			AmuletWellRBCSurvival_runInitialSearch = TRUE;
			AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestID = -1;
			m_ucClear = 100;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM]);
			return;


		case PHASE_WELLRBCSURVIVAL_EDIT:
			beep_amulet();
			AmuletWellRBCSurvival_lastName[0] = 0;
			AmuletWellRBCSurvival_patientID[0] = 0;
			AmuletWellRBCSurvival_testCount = 0;
			AmuletWellRBCSurvival_runInitialSearch = TRUE;
			AmuletAddEditRBCSurvivalTest_test.WellRBCSurvivalTestID = AmuletWellRBCSurvivalSearch_test[AmuletWellRBCSurvival_currentIndex].WellRBCSurvivalTestID;
			m_ucClear = 100;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITRBCSURVIVALTEST_HTM]);
			return;

		case PHASE_WELLRBCSURVIVAL_RUN:
			beep_amulet();
			if(AmuletWellRBCSurvivalSearch_test[AmuletWellRBCSurvival_currentIndex].WellRBCSurvivalTestPhase < 0){
				Amulet_DisplayError("RBC Survival", "Please complete required test information", TRUE);
				return;
			}
			AmuletWellRBCSurvivalTest.WellRBCSurvivalTestID = AmuletWellRBCSurvivalSearch_test[AmuletWellRBCSurvival_currentIndex].WellRBCSurvivalTestID;
			AmuletWellRBCSurvival_lastName[0] = 0;
			AmuletWellRBCSurvival_patientID[0] = 0;
			AmuletWellRBCSurvival_runInitialSearch = TRUE;
			m_ucClear = 101;
			SetAmuletHTML(AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLRBCSURVIVALTEST_HTM]);
			return;
	}
}

static void AmuletWellRBCSurvival_displayPage(void){
	int index;
	char message[26];
	char name[51];

	AmuletWellRBCSurvival_lastPage = AmuletWellRBCSurvival_testCount / 10;
	if(AmuletWellRBCSurvival_testCount % 10) AmuletWellRBCSurvival_lastPage++;
	AmuletWellRBCSurvival_lastPage--;
	if(AmuletWellRBCSurvival_lastPage < 0) AmuletWellRBCSurvival_lastPage = 0;

	if(AmuletWellRBCSurvival_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellRBCSurvival_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellRBCSurvival_currentPage == AmuletWellRBCSurvival_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Down Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellRBCSurvival_currentPage + index) < AmuletWellRBCSurvival_testCount){
			SetAmuletByte(101 + index, 0xFF);

			strcpy(name, AmuletWellRBCSurvivalSearch_test[10 * AmuletWellRBCSurvival_currentPage + index].LastName);
			strcat(name, ", ");
			strcat(name, AmuletWellRBCSurvivalSearch_test[10 * AmuletWellRBCSurvival_currentPage + index].FirstName);
			SetAmuletString(4*index + 80, name);

			SetAmuletString(4*index + 82, AmuletWellRBCSurvivalSearch_test[10 * AmuletWellRBCSurvival_currentPage + index].PatientID);

			if(AmuletWellRBCSurvivalSearch_test[10 * AmuletWellRBCSurvival_currentPage + index].InjectedOn == (time_t) 0){
				message[0] = 0;
			}else{
				GetExtendedTimeInfo(&AmuletWellRBCSurvivalSearch_test[10 * AmuletWellRBCSurvival_currentPage + index].InjectedOn, message);
			}
			SetAmuletString(4*index + 83, message);
		}else{
			SetAmuletByte(101 + index, 0x00);
			SetAmuletString(4*index + 80, "");
			SetAmuletString(4*index + 82, "");
			SetAmuletString(4*index + 83, "");
		}
	}

	message[0] = 0;
	if(AmuletWellRBCSurvival_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellRBCSurvival_currentPage + 1, AmuletWellRBCSurvival_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellRBCSurvival_currentIndex = -1;
}

void AmuletWellRBCSurvival_clearSearch(void){
	AmuletWellRBCSurvival_lastName[0] = 0;
	AmuletWellRBCSurvival_patientID[0] = 0;
	AmuletWellRBCSurvival_testCount = 0;
	AmuletWellRBCSurvival_runInitialSearch = FALSE;
	AmuletWellRBCSurvival_currentPage = 0;
}
