#define PHASE_WELLBIOASSAYSEARCH_PRE_INIT	0
#define PHASE_WELLBIOASSAYSEARCH_WAIT		1
#define PHASE_WELLBIOASSAYSEARCH_PGDOWN		2
#define PHASE_WELLBIOASSAYSEARCH_PGUP		3
#define PHASE_WELLBIOASSAYSEARCH_ROW1		4
#define PHASE_WELLBIOASSAYSEARCH_ROW2		5
#define PHASE_WELLBIOASSAYSEARCH_ROW3		6
#define PHASE_WELLBIOASSAYSEARCH_ROW4		7
#define PHASE_WELLBIOASSAYSEARCH_ROW5		8
#define PHASE_WELLBIOASSAYSEARCH_ROW6		9
#define PHASE_WELLBIOASSAYSEARCH_ROW7		10
#define PHASE_WELLBIOASSAYSEARCH_ROW8		11
#define PHASE_WELLBIOASSAYSEARCH_ROW9		12
#define PHASE_WELLBIOASSAYSEARCH_ROW10		13
#define PHASE_WELLBIOASSAYSEARCH_VIEW		14
#define PHASE_WELLBIOASSAYSEARCH_SEARCH		15
#define PHASE_WELLBIOASSAYSEARCH_FROM		16
#define PHASE_WELLBIOASSAYSEARCH_TO			17
#define PHASE_WELLBIOASSAYSEARCH_CRITERIA	18
#define PHASE_WELLBIOASSAYSEARCH_PRINT		19

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
extern PROBEBIOASSAYTEST AmuletWellBioAssayAnalysis_test;

time_t AmuletWellBioAssaySearch_from;
time_t AmuletWellBioAssaySearch_to;
int AmuletWellBioAssaySearch_staffID;
bool AmuletWellBioAssaySearch_accepted;

static time_t AmuletWellBioAssaySearch_fromSearched;
static time_t AmuletWellBioAssaySearch_toSearched;
static int AmuletWellBioAssaySearch_staffIDSearched;
static int AmuletWellBioAssaySearch_testCount;
static int AmuletWellBioAssaySearch_currentPage;	// Zero based
static int AmuletWellBioAssaySearch_lastPage;		// Zero based
static int AmuletWellBioAssaySearch_currentIndex;	// Zero based
static WELLBIOASSAYSEARCH AmuletWellBioAssaySearch_test[250];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void GetExtendedTimeInfo(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellBioAssaySearch_displayPage(void);
static void AmuletWellBioAssaySearch_print(void);

void AmuletWellBioAssaySearch_menu(void){
	char message[51];
	struct tm tmt;
	USER guestuser, user;

	switch(m_iPhase){
		case PHASE_WELLBIOASSAYSEARCH_PRE_INIT:
			DB_GetUser("Guest", &guestuser);

			if(m_ucClear == 85){
				AmuletWellBioAssaySearch_testCount = 0;
				AmuletWellBioAssaySearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellBioAssaySearch_from = mk_time(&tmt);
				AmuletWellBioAssaySearch_to = mk_time(&tmt);
				m_ucClear = 0;
				AmuletWellBioAssaySearch_staffID = -1;

				if((current.security_mode == 1 && guestuser.UserID == current.user_id) || (current.security_mode == 2)){
					AmuletLoginMenu_blockGuest = TRUE;
					AmuletLoginMenu_minRole = 0;
					m_ucClear = 75;
					SetAmuletHTML(AmuletHTMLIndex[LOGIN_HTM]);
					PushPageStack(AmuletHTMLIndex[LOGIN_HTM]);
					AmuletWellBioAssaySearch_accepted = FALSE;
					return;
				}else{
					AmuletWellBioAssaySearch_accepted = TRUE;
				}
			}

			if(!AmuletWellBioAssaySearch_accepted || (guestuser.UserID == current.user_id)){
				SetAmuletBackHTML();
				return;
			}

			dateout(message, &AmuletWellBioAssaySearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellBioAssaySearch_to, 4);
			SetAmuletString(132, message);

			DB_GetUserFromID(current.user_id, &user);
			if(user.Role < 2){
				SetAmuletByte(90, 0x00);
				SetAmuletByte(91, 0xFF);
				AmuletWellBioAssaySearch_staffID = current.user_id;
			}else{
				SetAmuletByte(90, 0xFF);
				SetAmuletByte(91, 0x00);
			}

			if(AmuletWellBioAssaySearch_staffID == -1){
				message[0] = 0;
			}else{
				DB_GetUserFromID(AmuletWellBioAssaySearch_staffID, &user);
				sprintf(message, "%s\n%s", user.FirstName, user.LastName);
			}
			SetAmuletString(133, message);

			AmuletWellBioAssaySearch_displayPage();

			SetAmuletByte(99, 0xFF);

			//if(AmuletWellBioAssaySearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellBioAssaySearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER)) SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);

			m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_WAIT:
			break;

		case PHASE_WELLBIOASSAYSEARCH_PGDOWN:
			beep_amulet();
			AmuletWellBioAssaySearch_currentPage++;
			AmuletWellBioAssaySearch_displayPage();
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_PGUP:
			beep_amulet();
			if(AmuletWellBioAssaySearch_currentPage){
				AmuletWellBioAssaySearch_currentPage--;
				AmuletWellBioAssaySearch_displayPage();
			}
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_ROW1:
		case PHASE_WELLBIOASSAYSEARCH_ROW2:
		case PHASE_WELLBIOASSAYSEARCH_ROW3:
		case PHASE_WELLBIOASSAYSEARCH_ROW4:
		case PHASE_WELLBIOASSAYSEARCH_ROW5:
		case PHASE_WELLBIOASSAYSEARCH_ROW6:
		case PHASE_WELLBIOASSAYSEARCH_ROW7:
		case PHASE_WELLBIOASSAYSEARCH_ROW8:
		case PHASE_WELLBIOASSAYSEARCH_ROW9:
		case PHASE_WELLBIOASSAYSEARCH_ROW10:
			beep_amulet();
			AmuletWellBioAssaySearch_currentIndex = (10 * AmuletWellBioAssaySearch_currentPage) + (m_iPhase - PHASE_WELLBIOASSAYSEARCH_ROW1);
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_VIEW:
			beep_amulet();
			AmuletWellBioAssayAnalysis_test.ProbeBioAssayTestID = AmuletWellBioAssaySearch_test[AmuletWellBioAssaySearch_currentIndex].ProbeBioAssayTestID;
			DB_RetrieveBioAssayTest(&AmuletWellBioAssayAnalysis_test);
			SetAmuletHTML(AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLBIOASSAYANALYSIS_HTM]);
			return;

		case PHASE_WELLBIOASSAYSEARCH_SEARCH:
			beep_amulet();

			if(AmuletWellBioAssaySearch_staffID == -1){
				Amulet_DisplayError("Search BioAssay Error", "Staff member is missing", TRUE);
				return;
			}

			AmuletWellBioAssaySearch_fromSearched = AmuletWellBioAssaySearch_from;
			AmuletWellBioAssaySearch_toSearched = AmuletWellBioAssaySearch_to;
			AmuletWellBioAssaySearch_staffIDSearched = AmuletWellBioAssaySearch_staffID;

			AmuletWellBioAssaySearch_testCount = DB_SearchBioAssayTests(AmuletWellBioAssaySearch_test, AmuletWellBioAssaySearch_from, AmuletWellBioAssaySearch_to, 250, AmuletWellBioAssaySearch_staffID);

			AmuletWellBioAssaySearch_currentPage = 0;
			if(AmuletWellBioAssaySearch_testCount == -1){
				AmuletWellBioAssaySearch_testCount = 0;
				Amulet_DisplayError("Search BioAssay Error", "More than 250 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellBioAssaySearch_testCount == 0){
				Amulet_DisplayError("Search BioAssay Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_PRE_INIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_FROM:
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_TO:
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_CRITERIA:
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			break;

		case PHASE_WELLBIOASSAYSEARCH_PRINT:
			beep_amulet();
			AmuletWellBioAssaySearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLBIOASSAYSEARCH_WAIT;
			m_iPhase = PHASE_WELLBIOASSAYSEARCH_PRE_INIT;
			break;
	}
}

static void AmuletWellBioAssaySearch_displayPage(void){
	int index;
	char message[26];
	char message2[26];
	char measuredon[31];
	char I131[31];
	char I125[31];
	char I123[31];

	AmuletWellBioAssaySearch_lastPage = AmuletWellBioAssaySearch_testCount / 10;
	if(AmuletWellBioAssaySearch_testCount % 10) AmuletWellBioAssaySearch_lastPage++;
	AmuletWellBioAssaySearch_lastPage--;
	if(AmuletWellBioAssaySearch_lastPage < 0) AmuletWellBioAssaySearch_lastPage = 0;

	if(AmuletWellBioAssaySearch_lastPage == 0){
		SetAmuletByte(92, 0x00); // Hide arrows
		SetAmuletByte(93, 0x00);
	}else{
		if(AmuletWellBioAssaySearch_currentPage == 0){
			SetAmuletByte(92, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(93, 0xFF);
		}else if(AmuletWellBioAssaySearch_currentPage == AmuletWellBioAssaySearch_lastPage){
			SetAmuletByte(92, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(93, 0x00);
		}else{
			SetAmuletByte(92, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(93, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellBioAssaySearch_currentPage + index) < AmuletWellBioAssaySearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].Inactive){
				measuredon[0] = 4;
				measuredon[1] = 0;
				I131[0] = 4;
				I131[1] = 0;
				I125[0] = 4;
				I125[1] = 0;
				I123[0] = 4;
				I123[1] = 0;
			}else{
				measuredon[0] = 0;
				I131[0] = 0;
				I125[0] = 0;
				I123[0] = 0;
			}

			GetExtendedTimeInfo(&AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].CreatedOn, message);
			strcat(measuredon, message);
			SetAmuletString(4*index + 90, measuredon);

			if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I131Active){
				if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I131Activity != -1.0){
					if(current.system == CI) format_activity_system2(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I131Activity, message);
					else format_activity_system_kbq(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I131Activity, message);
				}else{
					if(current.system == CI){
						sprintf(message, "%.0f cpm", AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I131NetROICPM);
					}else{
						sprintf(message, "%.0f cps", AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I131NetROICPM / 60.0);
					}
				}
			}else{
				message[0] = 0;
			}
			if(message[0] != 0){
				if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I131High){
					sprintf(message2, "< %s", message);
					sprintf(message, "%s >", message2);
				}
			}
			strcat(I131, message);
			SetAmuletString(4*index + 91, I131);

			if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I125Active){
				if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I125Activity != -1.0){
					if(current.system == CI) format_activity_system2(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I125Activity, message);
					else format_activity_system_kbq(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I125Activity, message);
				}else{
					if(current.system == CI){
						sprintf(message, "%.0f cpm", AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I125NetROICPMCorrected);
					}else{
						sprintf(message, "%.0f cps", AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I125NetROICPMCorrected / 60.0);
					}
				}
			}else{
				message[0] = 0;
			}
			if(message[0] != 0){
				if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I125High){
					sprintf(message2, "< %s", message);
					sprintf(message, "%s >", message2);
				}
			}
			strcat(I125, message);
			SetAmuletString(4*index + 92, I125);

			if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I123Active){
				if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I123Activity != -1.0){
					if(current.system == CI) format_activity_system2(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I123Activity, message);
					else format_activity_system_kbq(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I123Activity, message);
				}else{
					if(current.system == CI){
						sprintf(message, "%.0f cpm", AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I123NetROICPM);
					}else{
						sprintf(message, "%.0f cps", AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I123NetROICPM / 60.0);
					}
				}
			}else{
				message[0] = 0;
			}
			if(message[0] != 0){
				if(AmuletWellBioAssaySearch_test[10 * AmuletWellBioAssaySearch_currentPage + index].I123High){
					sprintf(message2, "< %s", message);
					sprintf(message, "%s >", message2);
				}
			}
			strcat(I123, message);
			SetAmuletString(4*index + 93, I123);
		}else{
			SetAmuletByte(101 + index, 0x00);
			SetAmuletString(4*index + 90, "");
			SetAmuletString(4*index + 91, "");
			SetAmuletString(4*index + 92, "");
			SetAmuletString(4*index + 93, "");
		}
	}

	message[0] = 0;
	if(AmuletWellBioAssaySearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellBioAssaySearch_currentPage + 1, AmuletWellBioAssaySearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellBioAssaySearch_currentIndex = -1;
}

static void AmuletWellBioAssaySearch_print(void){
	char prtype;
	char strng[90], message[90];
	int index;
	USER user;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheader(prtype, "BioAssay Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		DB_GetUserFromID(AmuletWellBioAssaySearch_staffIDSearched, &user);
		insertstring(strng, 0, 0, 0, "First: %s", user.FirstName);
		pr_write2(strng, "BioAssay Search");

		insertstring(strng, 0, 0, 0, " Last: %s", user.LastName);
		pr_write2(strng, "BioAssay Search");

		insertconst(strng, 5, 1, 0, "From:");
		inserttime_t(strng, 7, 0, 0, AmuletWellBioAssaySearch_fromSearched, TRUE);
		pr_write2(strng, "BioAssay Search");

		insertconst(strng, 5, 1, 0, "To:");
		inserttime_t(strng, 7, 0, 0, AmuletWellBioAssaySearch_toSearched, TRUE);
		pr_write2(strng, "BioAssay Search");

		for(index=0; index<AmuletWellBioAssaySearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellBioAssayAnalysis_test.ProbeBioAssayTestID = AmuletWellBioAssaySearch_test[index].ProbeBioAssayTestID;
			DB_RetrieveBioAssayTest(&AmuletWellBioAssayAnalysis_test);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2(strng, "BioAssay Search");

			inserttime_t(strng, 1, 0, 0, AmuletWellBioAssaySearch_test[index].CreatedOn, FALSE);
			pr_write2(strng, "BioAssay Search");

			if(AmuletWellBioAssayAnalysis_test.Inactive){
				insertconst(strng, 8, 1, 0, "INACTIVE");
				if(strlen(AmuletWellBioAssayAnalysis_test.InactiveReason) > 0){
					insertstring(strng, 9, 0, 0, ": %s", AmuletWellBioAssayAnalysis_test.InactiveReason);
				}
				pr_write2(strng, "BioAssay Search");
			}else{
				if(AmuletWellBioAssaySearch_test[index].I131Active){
					insertconst(strng, 2, 0, 0, "I131:" );
					if(AmuletWellBioAssaySearch_test[index].I131Activity != -1.0){
						if(current.system == CI) format_activity_system2(AmuletWellBioAssaySearch_test[index].I131Activity, message);
						else format_activity_system_kbq(AmuletWellBioAssaySearch_test[index].I131Activity, message);
						replace(message, '$', 'u');
						insertstring(strng, 8, 0, 0, "%s", message);
						if(AmuletWellBioAssaySearch_test[index].I131High) insertconst(strng, 32, 1, 0, "HIGH");
					}else{
						if(current.system == CI){
							insertfloat(strng, 8, 0, 0, "%.0f cpm", AmuletWellBioAssaySearch_test[index].I131NetROICPM);
						}else{
							insertfloat(strng, 8, 0, 0, "%.0f cps", AmuletWellBioAssaySearch_test[index].I131NetROICPM / 60.0);
						}
					}
				}
				pr_write2(strng, "BioAssay Search");

				if(AmuletWellBioAssaySearch_test[index].I125Active){
					insertconst(strng, 2, 0, 0, "I125:" );
					if(AmuletWellBioAssaySearch_test[index].I125Activity != -1.0){
						if(current.system == CI) format_activity_system2(AmuletWellBioAssaySearch_test[index].I125Activity, message);
						else format_activity_system_kbq(AmuletWellBioAssaySearch_test[index].I125Activity, message);
						replace(message, '$', 'u');
						insertstring(strng, 8, 0, 0, "%s", message);
						if(AmuletWellBioAssaySearch_test[index].I125High) insertconst(strng, 32, 1, 0, "HIGH");
					}else{
						if(current.system == CI){
							insertfloat(strng, 8, 0, 0, "%.0f cpm", AmuletWellBioAssaySearch_test[index].I125NetROICPMCorrected);
						}else{
							insertfloat(strng, 8, 0, 0, "%.0f cps", AmuletWellBioAssaySearch_test[index].I125NetROICPMCorrected / 60.0);
						}
					}
				}
				pr_write2(strng, "BioAssay Search");

				if(AmuletWellBioAssaySearch_test[index].I123Active){
					insertconst(strng, 2, 0, 0, "I123:" );
					if(AmuletWellBioAssaySearch_test[index].I123Activity != -1.0){
						if(current.system == CI) format_activity_system2(AmuletWellBioAssaySearch_test[index].I123Activity, message);
						else format_activity_system_kbq(AmuletWellBioAssaySearch_test[index].I123Activity, message);
						replace(message, '$', 'u');
						insertstring(strng, 8, 0, 0, "%s", message);
						if(AmuletWellBioAssaySearch_test[index].I123High) insertconst(strng, 32, 1, 0, "HIGH");
					}else{
						if(current.system == CI){
							insertfloat(strng, 8, 0, 0, "%.0f cpm", AmuletWellBioAssaySearch_test[index].I123NetROICPM);
						}else{
							insertfloat(strng, 8, 0, 0, "%.0f cps", AmuletWellBioAssaySearch_test[index].I123NetROICPM / 60.0);
						}
					}
				}
				pr_write2(strng, "BioAssay Search");

				if(AmuletWellBioAssayAnalysis_test.Comment[0] != 0){
					pr_write2(strng, "BioAssay Search");
					insertconst(strng, 8, 1, 0, "COMMENT");
					insertstring(strng, 9, 0, 0, ": %s", AmuletWellBioAssayAnalysis_test.Comment);
					pr_write2(strng, "BioAssay Search");
				}
			}
		}

		formfeed(prtype);
	}
}

void AmuletWellBioAssaySearch_inactivate(long long int ProbeBioAssayTestID){
	int index;
	if(AmuletWellBioAssaySearch_testCount > 0){
		for(index=0; index<AmuletWellBioAssaySearch_testCount; index++){
			if(AmuletWellBioAssaySearch_test[index].ProbeBioAssayTestID == ProbeBioAssayTestID){
				AmuletWellBioAssaySearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
