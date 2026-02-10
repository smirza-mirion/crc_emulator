/**
 * \file
 * \details This file handles calls from the Amulet AutoLinearity Search Screen
 */
#define PHASE_AUTOLINEARITYSEARCH_PRE_INIT	0
#define PHASE_AUTOLINEARITYSEARCH_WAIT		1
#define PHASE_AUTOLINEARITYSEARCH_PGDOWN	2
#define PHASE_AUTOLINEARITYSEARCH_PGUP		3
#define PHASE_AUTOLINEARITYSEARCH_ROW1		4
#define PHASE_AUTOLINEARITYSEARCH_ROW2		5
#define PHASE_AUTOLINEARITYSEARCH_ROW3		6
#define PHASE_AUTOLINEARITYSEARCH_ROW4		7
#define PHASE_AUTOLINEARITYSEARCH_ROW5		8
#define PHASE_AUTOLINEARITYSEARCH_ROW6		9
#define PHASE_AUTOLINEARITYSEARCH_ROW7		10
#define PHASE_AUTOLINEARITYSEARCH_ROW8		11
#define PHASE_AUTOLINEARITYSEARCH_ROW9		12
#define PHASE_AUTOLINEARITYSEARCH_ROW10		13
#define PHASE_AUTOLINEARITYSEARCH_VIEW		14
#define PHASE_AUTOLINEARITYSEARCH_SEARCH	15
#define PHASE_AUTOLINEARITYSEARCH_FROM		16
#define PHASE_AUTOLINEARITYSEARCH_TO		17
#define PHASE_AUTOLINEARITYSEARCH_CRITERIA	18
#define PHASE_AUTOLINEARITYSEARCH_PRINT		19

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "wipes.h"
#include "time.h"
#include "mca.h"
#include "database.h"
#include "printer.h"
#include "message.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CHAMBER  chamber[];
extern CHAMBERVALS chamb_vals[];
extern CURRENT current;
extern time_t clock_time;
extern AUTOLINEARITYTEST AmuletAutoLinearityTest_retrieved;
extern int AmuletAutoLinearityTest_mode;

time_t AmuletAutoLinearitySearch_from;
time_t AmuletAutoLinearitySearch_to;

static int AmuletAutoLinearitySearch_testCount;
static int AmuletAutoLinearitySearch_currentPage;	// Zero based
static int AmuletAutoLinearitySearch_lastPage;		// Zero based
static int AmuletAutoLinearitySearch_currentIndex;	// Zero based
static AUTOLINEARITYSEARCH AmuletAutoLinearitySearch_test[100];
static char AmuletAutoLinearitySearch_ch1_SN[7];
static char AmuletAutoLinearitySearch_ch2_SN[7];

void trim_and_shrink(char *acByte);
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void GetExtendedTimeInfoLanguageSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletAutoLinearitySearch_displayPage(void);

/**
 * \details Handles the Amulet Screen AutoLinearitySearch.htm. AutoLinearitySearch.htm searches the database for autolinearity test from a date range.
 * \param Amulet_Byte_ID Description
 * \param 80 Language	(STATE) English = 0, French = 1
 * \param 91 PageUp
 * \param 92 PageDown
 * \param 99 Show Buttons
 * \param 100 Refresh Grid
 * \param 101 Line1 Active (0xFF is Active)
 * \param 102 Line2 Active (0xFF is Active)
 * \param 103 Line3 Active (0xFF is Active)
 * \param 104 Line4 Active (0xFF is Active)
 * \param 105 Line5 Active (0xFF is Active)
 * \param 106 Line6 Active (0xFF is Active)
 * \param 107 Line7 Active (0xFF is Active)
 * \param 108 Line8 Active (0xFF is Active)
 * \param 109 Line9 Active (0xFF is Active)
 * \param 110 Line10 Active (0xFF is Active)
 * \param 111 Clear Grid
 * \param Amulet_String_ID Description
 * \param 80_81 Search AutoLinearity Tests
 * \param 82 From:
 * \param 83_84 Enter Start Date:
 * \param 85 To:
 * \param 86_87 Enter End Date:
 * \param 88 Search
 * \param 89 Chamber
 * \param 90 Nuclide
 * \param 91 Date
 * \param 92 View
 * \param 100 sfC1R1
 * \param 101 sfC2R1
 * \param 102 sfC3R1
 * \param 103 sfC1R2
 * \param 104 sfC2R2
 * \param 105 sfC3R2
 * \param 106 sfC1R3
 * \param 107 sfC2R3
 * \param 108 sfC3R3
 * \param 109 sfC1R4
 * \param 110 sfC2R4
 * \param 111 sfC3R4
 * \param 112 sfC1R5
 * \param 113 sfC2R5
 * \param 114 sfC3R5
 * \param 115 sfC1R6
 * \param 116 sfC2R6
 * \param 117 sfC3R6
 * \param 118 sfC1R7
 * \param 119 sfC2R7
 * \param 120 sfC3R7
 * \param 121 sfC1R8
 * \param 122 sfC2R8
 * \param 123 sfC3R8
 * \param 124 sfC1R9
 * \param 125 sfC2R9
 * \param 126 sfC3R9
 * \param 127 sfC1R10
 * \param 128 sfC2R10
 * \param 129 sfC3R10
 * \param 130 sfPage
 * \param 131 btnFrom
 * \param 132 btnTo
 * \param 133 btnCriteria
 * \returns None
 */
void AmuletAutoLinearitySearch_menu(void){
	int index;
	char message[26], titlestring[100], amstring1[104];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_AUTOLINEARITYSEARCH_PRE_INIT:
			SetAmuletByte(80, current.language);
			send_amulet_message(L_SEARCH_AUTOLINEARITY_TESTS, 80);    // "Search AutoLinearity Tests"
			send_amulet_message(L_FROM2, 82);    // "From:"
			send_amulet_message(L_ENTER_START_DATE, 83);    // "Enter Start Date:"
			send_amulet_message(L_TO2, 85);    // "To:"
			send_amulet_message(L_ENTER_END_DATE, 86);    // "Enter End Date:"
			send_amulet_message(L_SEARCH, 88);    // "Search"
			send_amulet_message(L_CHAMBER, 89);    // "Chamber"
			send_amulet_message(L_NUCLIDE, 90);    // "Nuclide"
			send_amulet_message(L_DATE, 91);    // "Date"
			send_amulet_message(L_VIEW, 92);    // "View"

			if(m_ucClear == 107){
				AmuletAutoLinearitySearch_testCount = 0;
				AmuletAutoLinearitySearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletAutoLinearitySearch_from = mk_time(&tmt);
				AmuletAutoLinearitySearch_to = mk_time(&tmt);

				if(chamber[0].exists){
					for(index=0; index<6; index++){
						AmuletAutoLinearitySearch_ch1_SN[index] = chamb_vals[0].sn[index];
					}
					AmuletAutoLinearitySearch_ch1_SN[6] = 0;
				}else{
					AmuletAutoLinearitySearch_ch1_SN[0] = 0;
				}

				if(chamber[1].exists){
					for(index=0; index<6; index++){
						AmuletAutoLinearitySearch_ch2_SN[index] = chamb_vals[1].sn[index];
					}
					AmuletAutoLinearitySearch_ch2_SN[6] = 0;
				}else{
					AmuletAutoLinearitySearch_ch2_SN[0] = 0;
				}

				m_ucClear = 0;
			}

			dateout_language(message, &AmuletAutoLinearitySearch_from, 4);
			send_to_amulet_string(131, message);

			dateout_language(message, &AmuletAutoLinearitySearch_to, 4);
			send_to_amulet_string(132, message);

			AmuletAutoLinearitySearch_displayPage();
			SetAmuletByte(99, 0xFF);

			// No Print
			//if(AmuletAutoLinearitySearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
			//	SetAmuletByte(98, 0xFF);
			//else SetAmuletByte(97, 0xFF);

			m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			break;

		case PHASE_AUTOLINEARITYSEARCH_WAIT:
			break;

		case PHASE_AUTOLINEARITYSEARCH_PGDOWN:
			beep_amulet();
			AmuletAutoLinearitySearch_currentPage++;
			AmuletAutoLinearitySearch_displayPage();
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			break;

		case PHASE_AUTOLINEARITYSEARCH_PGUP:
			beep_amulet();
			if(AmuletAutoLinearitySearch_currentPage){
				AmuletAutoLinearitySearch_currentPage--;
				AmuletAutoLinearitySearch_displayPage();
			}
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			break;

		case PHASE_AUTOLINEARITYSEARCH_ROW1:
		case PHASE_AUTOLINEARITYSEARCH_ROW2:
		case PHASE_AUTOLINEARITYSEARCH_ROW3:
		case PHASE_AUTOLINEARITYSEARCH_ROW4:
		case PHASE_AUTOLINEARITYSEARCH_ROW5:
		case PHASE_AUTOLINEARITYSEARCH_ROW6:
		case PHASE_AUTOLINEARITYSEARCH_ROW7:
		case PHASE_AUTOLINEARITYSEARCH_ROW8:
		case PHASE_AUTOLINEARITYSEARCH_ROW9:
		case PHASE_AUTOLINEARITYSEARCH_ROW10:
			beep_amulet();
			AmuletAutoLinearitySearch_currentIndex = (10 * AmuletAutoLinearitySearch_currentPage) + (m_iPhase - PHASE_AUTOLINEARITYSEARCH_ROW1);
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			break;

		case PHASE_AUTOLINEARITYSEARCH_VIEW:
			beep_amulet();
			AmuletAutoLinearityTest_retrieved.AutoLinearityTestID = AmuletAutoLinearitySearch_test[AmuletAutoLinearitySearch_currentIndex].AutoLinearityTestID;
			DB_RetrieveAutoLinearityTest(&AmuletAutoLinearityTest_retrieved);
			AmuletAutoLinearityTest_mode = 1;
			m_ucClear = 105;
			SetAmuletHTML(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
			PushPageStack(AmuletHTMLIndex[AUTOLINEARITYTEST_HTM]);
			return;

		case PHASE_AUTOLINEARITYSEARCH_SEARCH:
			beep_amulet();
			AmuletAutoLinearitySearch_testCount = DB_SearchAutoLinearityTests(AmuletAutoLinearitySearch_test, AmuletAutoLinearitySearch_from, AmuletAutoLinearitySearch_to, 100);
			AmuletAutoLinearitySearch_currentPage = 0;
			if(AmuletAutoLinearitySearch_testCount == -1){
				AmuletAutoLinearitySearch_testCount = 0;

				get_amulet_message(L_SEARCH_AUTOLINEARITY_ERROR, titlestring);    // "Search AutoLinearity Error"
				get_amulet_message(L_MORE_THAN_100_ITEMS_RETURNED, amstring1);    // "More than 100 items have been returned\nPlease refine criteria"
				//Amulet_DisplayError("Search AutoLinearity Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				Amulet_DisplayError(titlestring, amstring1, TRUE);
				return;
			}else if(AmuletAutoLinearitySearch_testCount == 0){
				get_amulet_message(L_SEARCH_AUTOLINEARITY_ERROR, titlestring);    // "Search AutoLinearity Error"
				get_amulet_message(L_0_ITEMS_RETURNED, amstring1);    // "0 items have been returned"
				//Amulet_DisplayError("Search AutoLinearity Error", "0 items have been returned", TRUE);
				Amulet_DisplayError(titlestring, amstring1, TRUE);
				return;
			}
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_PRE_INIT;
			break;

		case PHASE_AUTOLINEARITYSEARCH_FROM:
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			break;

		case PHASE_AUTOLINEARITYSEARCH_TO:
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			break;

		case PHASE_AUTOLINEARITYSEARCH_CRITERIA:
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			break;

		//case PHASE_AUTOLINEARITYSEARCH_PRINT:
			/*beep_amulet();
			AmuletWellSchillingSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_AUTOLINEARITYSEARCH_WAIT;
			m_iPhase = PHASE_AUTOLINEARITYSEARCH_PRE_INIT;*/
			//break;
	}
}

static void AmuletAutoLinearitySearch_displayPage(void){
	int index;
	char message[26];
	char chamber[31];
	char nuclide[31];
	char startedon[31];
	char amstring1[100];

	AmuletAutoLinearitySearch_lastPage = AmuletAutoLinearitySearch_testCount / 10;
	if(AmuletAutoLinearitySearch_testCount % 10) AmuletAutoLinearitySearch_lastPage++;
	AmuletAutoLinearitySearch_lastPage--;
	if(AmuletAutoLinearitySearch_lastPage < 0) AmuletAutoLinearitySearch_lastPage = 0;

	if(AmuletAutoLinearitySearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletAutoLinearitySearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletAutoLinearitySearch_currentPage == AmuletAutoLinearitySearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletAutoLinearitySearch_currentPage + index) < AmuletAutoLinearitySearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].Inactive){
				chamber[0] = 4;
				chamber[1] = 0;
				nuclide[0] = 4;
				nuclide[1] = 0;
				startedon[0] = 4;
				startedon[1] = 0;
			}else{
				chamber[0] = 0;
				nuclide[0] = 0;
				startedon[0] = 0;
			}

			if(strcmp(AmuletAutoLinearitySearch_ch1_SN, AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].SerialNumber) == 0){
				sprintf(message, "CH:1 (%s)", AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].SerialNumber);
			}else if(strcmp(AmuletAutoLinearitySearch_ch2_SN, AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].SerialNumber) == 0){
				sprintf(message, "CH:2 (%s)", AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].SerialNumber);
			}else{
				get_amulet_message(L_SN2, amstring1);    // "S/N:"
				//sprintf(message, "S/N: %s", AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].SerialNumber);
				sprintf(message, "%s %s", amstring1, AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].SerialNumber);
			}
			strcat(chamber, message);
			send_to_amulet_string(3*index + 100, chamber);

			strcpy(message, AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].NuclideName);
			trim_and_shrink(message);
			strcat(nuclide, message);
			send_to_amulet_string(3*index + 101, nuclide);

			GetExtendedTimeInfoLanguageSec(&AmuletAutoLinearitySearch_test[10 * AmuletAutoLinearitySearch_currentPage + index].StartedOn, message);
			strcat(startedon, message);
			send_to_amulet_string(3*index + 102, startedon);
		}else{
			SetAmuletByte(101 + index, 0x00);
			send_to_amulet_string(3*index + 100, "");
			send_to_amulet_string(3*index + 101, "");
			send_to_amulet_string(3*index + 102, "");
		}
	}

	message[0] = 0;
	if(AmuletAutoLinearitySearch_lastPage > 0){
		get_amulet_message(L_OF, amstring1);    // "%d of %d"
		//sprintf(message, "%d of %d", AmuletAutoLinearitySearch_currentPage + 1, AmuletAutoLinearitySearch_lastPage + 1);
		sprintf(message, amstring1, AmuletAutoLinearitySearch_currentPage + 1, AmuletAutoLinearitySearch_lastPage + 1);
	}
	send_to_amulet_string(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletAutoLinearitySearch_currentIndex = -1;
}

void AmuletAutoLinearitySearch_inactivate(long long int AutoLinearityTestID){
	int index;

	if(AmuletAutoLinearitySearch_testCount > 0){
		for(index=0; index<AmuletAutoLinearitySearch_testCount; index++){
			if(AmuletAutoLinearitySearch_test[index].AutoLinearityTestID == AutoLinearityTestID){
				AmuletAutoLinearitySearch_test[index].Inactive = TRUE;
				break;
			}
		}
	}
}
