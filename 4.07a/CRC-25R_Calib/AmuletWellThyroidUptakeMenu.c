#define PHASE_WELLTHYROIDUPTAKE_PRE_INIT	0
#define PHASE_WELLTHYROIDUPTAKE_WAIT		1
#define PHASE_WELLTHYROIDUPTAKE_PGDOWN		2
#define PHASE_WELLTHYROIDUPTAKE_PGUP		3
#define PHASE_WELLTHYROIDUPTAKE_ROW1		4
#define PHASE_WELLTHYROIDUPTAKE_ROW2		5
#define PHASE_WELLTHYROIDUPTAKE_ROW3		6
#define PHASE_WELLTHYROIDUPTAKE_ROW4		7
#define PHASE_WELLTHYROIDUPTAKE_ROW5		8
#define PHASE_WELLTHYROIDUPTAKE_ROW6		9
#define PHASE_WELLTHYROIDUPTAKE_ROW7		10
#define PHASE_WELLTHYROIDUPTAKE_ROW8		11
#define PHASE_WELLTHYROIDUPTAKE_ROW9		12
#define PHASE_WELLTHYROIDUPTAKE_ROW10		13
#define PHASE_WELLTHYROIDUPTAKE_SEARCH		14
#define PHASE_WELLTHYROIDUPTAKE_ADD			15
#define PHASE_WELLTHYROIDUPTAKE_EDIT		16
#define PHASE_WELLTHYROIDUPTAKE_RUN			17

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
extern bool AmuletLoginMenu_blockGuest;
extern int AmuletLoginMenu_minRole;
extern PROBETHYROIDUPTAKETEST AmuletAddEditThyroidUptakeTest_test;
extern PROBETHYROIDUPTAKETEST AmuletWellThyroidUptakeTest;

bool AmuletWellThyroidUptake_runMode;
char AmuletWellThyroidUptake_lastName[18];
char AmuletWellThyroidUptake_patientID[11];
bool AmuletWellThyroidUptake_accepted;

static int AmuletWellThyroidUptake_testCount;
static int AmuletWellThyroidUptake_currentPage;	// Zero based
static int AmuletWellThyroidUptake_lastPage;		// Zero based
static int AmuletWellThyroidUptake_currentIndex;	// Zero based
static PROBETHYROIDUPTAKESEARCH AmuletWellThyroidUptakeSearch_test[100];
static bool AmuletWellThyroidUptake_runInitialSearch;
static PROBETHYROIDUPTAKETEST test;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellThyroidUptake_displayPage(void);

void AmuletWellThyroidUptake_menu(void){
	USER guestuser;

	switch(m_iPhase){
		case PHASE_WELLTHYROIDUPTAKE_PRE_INIT:
			DB_GetUser("Guest", &guestuser);

			if(m_ucClear == 89){
				AmuletWellThyroidUptake_lastName[0] = 0;
				AmuletWellThyroidUptake_patientID[0] = 0;
				AmuletWellThyroidUptake_testCount = 0;
				AmuletWellThyroidUptake_runInitialSearch = TRUE;
				AmuletWellThyroidUptake_currentPage = 0;
				m_ucClear = 0;

				if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 0;
					m_ucClear = 75;
					SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
					PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
					AmuletWellThyroidUptake_accepted = FALSE;
					return;
				}else{
					AmuletWellThyroidUptake_accepted = TRUE;
				}
			}

			if(!AmuletWellThyroidUptake_accepted || (guestuser.UserID == current.user_id)){
				SetAmuletBackHTML();
				return;
			}

			if(AmuletWellThyroidUptake_runMode){
				SetAmuletByte(98, 0xFF);
				SetAmuletString(77, "Thyroid Uptake Measurement");
				SetAmuletString(79, "Run");
				if(AmuletWellThyroidUptake_runInitialSearch){
					AmuletWellThyroidUptake_testCount = DB_SearchThyroidUptakeTests(AmuletWellThyroidUptakeSearch_test, AmuletWellThyroidUptake_lastName, AmuletWellThyroidUptake_patientID, AmuletWellThyroidUptake_runMode);
					AmuletWellThyroidUptake_runInitialSearch = FALSE;
				}
				Mca_switchToProbe();
			}else{
				SetAmuletByte(98, 0x00);
				SetAmuletString(77, "Thyroid Uptake Report");
				SetAmuletString(79, "View");
				AmuletWellThyroidUptake_runInitialSearch = FALSE;
			}

			SetAmuletString(131, AmuletWellThyroidUptake_lastName);
			SetAmuletString(132, AmuletWellThyroidUptake_patientID);

			AmuletWellThyroidUptake_displayPage();
			SetAmuletByte(99, 0xFF);
			m_iPhase = PHASE_WELLTHYROIDUPTAKE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKE_WAIT:
			break;

		case PHASE_WELLTHYROIDUPTAKE_PGDOWN:
			beep_amulet();
			AmuletWellThyroidUptake_currentPage++;
			AmuletWellThyroidUptake_displayPage();
			m_iPhase = PHASE_WELLTHYROIDUPTAKE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKE_PGUP:
			beep_amulet();
			if(AmuletWellThyroidUptake_currentPage){
				AmuletWellThyroidUptake_currentPage--;
				AmuletWellThyroidUptake_displayPage();
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKE_ROW1:
		case PHASE_WELLTHYROIDUPTAKE_ROW2:
		case PHASE_WELLTHYROIDUPTAKE_ROW3:
		case PHASE_WELLTHYROIDUPTAKE_ROW4:
		case PHASE_WELLTHYROIDUPTAKE_ROW5:
		case PHASE_WELLTHYROIDUPTAKE_ROW6:
		case PHASE_WELLTHYROIDUPTAKE_ROW7:
		case PHASE_WELLTHYROIDUPTAKE_ROW8:
		case PHASE_WELLTHYROIDUPTAKE_ROW9:
		case PHASE_WELLTHYROIDUPTAKE_ROW10:
			beep_amulet();
			AmuletWellThyroidUptake_currentIndex = (10 * AmuletWellThyroidUptake_currentPage) + (m_iPhase - PHASE_WELLTHYROIDUPTAKE_ROW1);
			m_iPhase = PHASE_WELLTHYROIDUPTAKE_WAIT;
			break;

		case PHASE_WELLTHYROIDUPTAKE_SEARCH:
			beep_amulet();
			AmuletWellThyroidUptake_testCount = DB_SearchThyroidUptakeTests(AmuletWellThyroidUptakeSearch_test, AmuletWellThyroidUptake_lastName, AmuletWellThyroidUptake_patientID, AmuletWellThyroidUptake_runMode);
			AmuletWellThyroidUptake_currentPage = 0;
			if(AmuletWellThyroidUptake_testCount == 0){
				Amulet_DisplayError("Thyroid Uptake", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLTHYROIDUPTAKE_PRE_INIT;
			break;

		case PHASE_WELLTHYROIDUPTAKE_ADD:
			beep_amulet();
			AmuletWellThyroidUptake_lastName[0] = 0;
			AmuletWellThyroidUptake_patientID[0] = 0;
			AmuletWellThyroidUptake_runInitialSearch = TRUE;
			AmuletAddEditThyroidUptakeTest_test.ProbeTUTestID = -1;
			m_ucClear = 90;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM]);
			return;

		case PHASE_WELLTHYROIDUPTAKE_EDIT:
			beep_amulet();
			AmuletWellThyroidUptake_lastName[0] = 0;
			AmuletWellThyroidUptake_patientID[0] = 0;
			AmuletWellThyroidUptake_testCount = 0;
			AmuletWellThyroidUptake_runInitialSearch = TRUE;
			AmuletAddEditThyroidUptakeTest_test.ProbeTUTestID = AmuletWellThyroidUptakeSearch_test[AmuletWellThyroidUptake_currentIndex].ProbeTUTestID;
			m_ucClear = 90;
			SetAmuletHTML(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM]);
			PushPageStack(AmuletHTMLIndex[ADDEDITTHYROIDUPTAKETEST_HTM]);
			return;

		case PHASE_WELLTHYROIDUPTAKE_RUN:
			beep_amulet();
			test.ProbeTUTestID = AmuletWellThyroidUptakeSearch_test[AmuletWellThyroidUptake_currentIndex].ProbeTUTestID;
			DB_RetrieveProbeThyroidUptakeTest(&test);
			if(test.ProbeTUTestID <= 0){
				Amulet_DisplayError("Thyroid Uptake", "Unable to retrieve TU Test", TRUE);
				return;
			}
			if(test.ProbeTUTestPhase < 0){
				Amulet_DisplayError("Thyroid Uptake", "Please complete required test information", TRUE);
				return;
			}
			AmuletWellThyroidUptakeTest.ProbeTUTestID = AmuletWellThyroidUptakeSearch_test[AmuletWellThyroidUptake_currentIndex].ProbeTUTestID;
			AmuletWellThyroidUptake_lastName[0] = 0;
			AmuletWellThyroidUptake_patientID[0] = 0;
			AmuletWellThyroidUptake_runInitialSearch = TRUE;
			m_ucClear = 91;
			SetAmuletHTML(AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLTHYROIDUPTAKETEST_HTM]);
			return;
	}
}

static void AmuletWellThyroidUptake_displayPage(void){
	int index;
	char message[26];
	char name[51];
	char patientid[31];
	char administered[31];

	AmuletWellThyroidUptake_lastPage = AmuletWellThyroidUptake_testCount / 10;
	if(AmuletWellThyroidUptake_testCount % 10) AmuletWellThyroidUptake_lastPage++;
	AmuletWellThyroidUptake_lastPage--;
	if(AmuletWellThyroidUptake_lastPage < 0) AmuletWellThyroidUptake_lastPage = 0;

	if(AmuletWellThyroidUptake_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellThyroidUptake_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellThyroidUptake_currentPage == AmuletWellThyroidUptake_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellThyroidUptake_currentPage + index) < AmuletWellThyroidUptake_testCount){
			SetAmuletByte(101 + index, 0xFF);
			//if(AmuletWellThyroidUptakeSearch_test[10 * AmuletWellThyroidUptake_currentPage + index].Inactive){
			//	name[0] = 4;
			//	name[1] = 0;
			//	patientid[0] = 4;
			//	patientid[1] = 0;
			//	isotope[0] = 4;
			//	isotope[1] = 0;
			//	administered[0] = 4;
			//	administered[1] = 0;
			//}else{
			//	name[0] = 0;
			//	patientid[0] = 0;
			//	isotope[0] = 0;
			//	administered[0] = 0;
			//}
			name[0] = 0;
			patientid[0] = 0;
			administered[0] = 0;

			strcat(name, AmuletWellThyroidUptakeSearch_test[10 * AmuletWellThyroidUptake_currentPage + index].LastName);
			strcat(name, ", ");
			strcat(name, AmuletWellThyroidUptakeSearch_test[10 * AmuletWellThyroidUptake_currentPage + index].FirstName);
			SetAmuletString(4*index + 80, name);

			strcat(patientid, AmuletWellThyroidUptakeSearch_test[10 * AmuletWellThyroidUptake_currentPage + index].PatientID);
			SetAmuletString(4*index + 82, patientid);

			if(AmuletWellThyroidUptakeSearch_test[10 * AmuletWellThyroidUptake_currentPage + index].DoseAdministeredOn == (time_t) 0){
				message[0] = 0;
			}else{
				GetExtendedTimeInfo(&AmuletWellThyroidUptakeSearch_test[10 * AmuletWellThyroidUptake_currentPage + index].DoseAdministeredOn, message);
			}
			strcat(administered, message);
			SetAmuletString(4*index + 83, administered);
		}else{
			SetAmuletByte(101 + index, 0x00);
			SetAmuletString(4*index + 80, "");
			SetAmuletString(4*index + 82, "");
			SetAmuletString(4*index + 83, "");
		}
	}

	message[0] = 0;
	if(AmuletWellThyroidUptake_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellThyroidUptake_currentPage + 1, AmuletWellThyroidUptake_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellThyroidUptake_currentIndex = -1;
}

void AmuletWellThyroidUptake_clearSearch(void){
	AmuletWellThyroidUptake_lastName[0] = 0;
	AmuletWellThyroidUptake_patientID[0] = 0;
	AmuletWellThyroidUptake_testCount = 0;
	AmuletWellThyroidUptake_runInitialSearch = FALSE;
	AmuletWellThyroidUptake_currentPage = 0;
}

/*void AmuletWellBioAssaySearch_inactivate(long long int ProbeBioAssayTestID){
	int index;
	if(AmuletWellThyroidUptake_testCount > 0){
		for(index=0; index<AmuletWellThyroidUptake_testCount; index++){
			if(AmuletWellBioAssaySearch_test[index].ProbeBioAssayTestID == ProbeBioAssayTestID){
				AmuletWellBioAssaySearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}*/
