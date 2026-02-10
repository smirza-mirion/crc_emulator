/**
 * \file
 * \details This file handles calls from the Amulet Report Search Screen
 */
#define PHASE_CHAMBERSEARCH_PRE_INIT	0
#define PHASE_CHAMBERSEARCH_WAIT		1
#define PHASE_CHAMBERSEARCH_PGDOWN		2
#define PHASE_CHAMBERSEARCH_PGUP		3
#define PHASE_CHAMBERSEARCH_ROW1		4
#define PHASE_CHAMBERSEARCH_ROW2		5
#define PHASE_CHAMBERSEARCH_ROW3		6
#define PHASE_CHAMBERSEARCH_ROW4		7
#define PHASE_CHAMBERSEARCH_ROW5		8
#define PHASE_CHAMBERSEARCH_ROW6		9
#define PHASE_CHAMBERSEARCH_ROW7		10
#define PHASE_CHAMBERSEARCH_ROW8		11
#define PHASE_CHAMBERSEARCH_ROW9		12
#define PHASE_CHAMBERSEARCH_ROW10		13
#define PHASE_CHAMBERSEARCH_VIEW		14
#define PHASE_CHAMBERSEARCH_SEARCH		15
#define PHASE_CHAMBERSEARCH_FROM		16
#define PHASE_CHAMBERSEARCH_TO			17
#define PHASE_CHAMBERSEARCH_CRITERIA	18
#define PHASE_CHAMBERSEARCH_PRINT		19

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
#include "chambfac.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CHAMBER  chamber[];
extern CHAMBERVALS chamb_vals[];
extern CURRENT current;
extern time_t clock_time;

int AmuletChamberSearch_mode;
time_t AmuletChamberSearch_from;
time_t AmuletChamberSearch_to;
CHAMBERDAILYTEST			AmuletChamberSearchMenu_DailyTest;
CHAMBERZERO					AmuletChamberSearchMenu_Zero;
CHAMBERBACKGROUND			AmuletChamberSearchMenu_Background;
CHAMBERVOLTAGE				AmuletChamberSearchMenu_Voltage;
CHAMBERACCURACYTEST			AmuletChamberSearchMenu_AccuracyTest;
CHAMBERACCURACYMEASUREMENT	AmuletChamberSearchMenu_AccuracyMeasurement[6];
CHAMBERAUTOCONSTANCY		AmuletChamberSearchMenu_AutoConstancy[12];

static int AmuletChamberSearch_testCount;
static int AmuletChamberSearch_currentPage;	// Zero based
static int AmuletChamberSearch_lastPage;		// Zero based
static int AmuletChamberSearch_currentIndex;	// Zero based
static CHAMBERSEARCH AmuletChamberSearch_test[100];
static char AmuletChamberSearch_ch1_SN[7];
static char AmuletChamberSearch_ch2_SN[7];

void trim(char *acByte);
void trim_and_shrink(char *acByte);
char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
void send_to_amulet_string(uchar ucIndex, char message0[]);
void GetExtendedTimeInfoLanguageSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletChamberSearch_displayPage(void);
void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output);
void prsetprinter_db(void);
void prdivider_db(char *header_name);
void prdailysection_db(char *header_name);
void przerosection_db(char *header_name, bool space);
void prbackgroundsection_db(char *header_name, bool space);
void prchambervoltagesection_db(char *header_name, bool space);
void praccuracysection_db(short prtype, char *header_name, bool print_chamber_section);

/**
 * \details Handles the Amulet Screen ChamberSearch.htm. ChamberSearch.htm searches the database for reports based on a date range.
 * \param Amulet_Byte_ID Description
 * \param 80 Language	(STATE) English = 0, French = 1
 * \param 91 PageUp
 * \param 92 PageDown
 * \param 93 Print
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
 * \param 80_81 Search Tests
 * \param 82 From:
 * \param 83_84 Enter Start Date:
 * \param 85 To:
 * \param 86_87 Enter End Date:
 * \param 88 Search
 * \param 89 Chamber
 * \param 90 Results
 * \param 91 Date
 * \param 92 View
 * \param 93 Print
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
void AmuletChamberSearch_menu(void){
	int index;
	char message[26], titlestring[100], amstring1[104];
	struct tm tmt;
	unsigned int amulet_page_index;

	switch(m_iPhase){
		case PHASE_CHAMBERSEARCH_PRE_INIT:
			SetAmuletByte(80, current.language);
			switch(AmuletChamberSearch_mode){
				case 108:
					send_amulet_message(L_SEARCH_DAILY_TESTS, 80);    // "Search Daily Tests"
					break;

				case 109:
					send_amulet_message(L_SEARCH_ZERO_MEASUREMENTS, 80);    // "Search Zero Measurements"
					break;

				case 110:
					send_amulet_message(L_SEARCH_BACKGROUND_MEASUREMENTS, 80);    // "Search Background Measurements"
					break;

				case 111:
					send_amulet_message(L_SEARCH_CHAMBER_VOLTAGE, 80);    // "Search Chamber Voltage Tests"
					break;

				case 112:
					send_amulet_message(L_SEARCH_ACCURACY_TESTS, 80);    // "Search Accuracy Tests"
					break;

				default:
					break;
			}
			delayloop(2);

			send_amulet_message(L_FROM2, 82);    // "From:"
			delayloop(2);

			send_amulet_message(L_ENTER_START_DATE, 83);    // "Enter Start Date:"
			delayloop(2);

			send_amulet_message(L_TO2, 85);    // "To:"
			delayloop(2);

			send_amulet_message(L_ENTER_END_DATE, 86);    // "Enter End Date:"
			delayloop(2);

			send_amulet_message(L_SEARCH, 88);    // "Search"
			delayloop(2);

			send_amulet_message(L_CHAMBER, 89);    // "Chamber"
			delayloop(2);

			if((AmuletChamberSearch_mode != 112) && (AmuletChamberSearch_mode != 108)){
				send_amulet_message(L_RESULTS, 90);    // "Results"
				delayloop(2);
			}

			send_amulet_message(L_DATE, 91);    // "Date"
			delayloop(2);

			send_amulet_message(L_VIEW, 92);    // "View"
			delayloop(2);

			send_amulet_message(L_PRINT, 93);    // "Print"
			delayloop(2);

			if(m_ucClear == 108){
				AmuletChamberSearch_testCount = 0;
				AmuletChamberSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletChamberSearch_from = mk_time(&tmt);
				AmuletChamberSearch_to = mk_time(&tmt);

				if(chamber[0].exists){
					for(index=0; index<6; index++){
						AmuletChamberSearch_ch1_SN[index] = chamb_vals[0].sn[index];
					}
					AmuletChamberSearch_ch1_SN[6] = 0;
				}else{
					AmuletChamberSearch_ch1_SN[0] = 0;
				}

				if(chamber[1].exists){
					for(index=0; index<6; index++){
						AmuletChamberSearch_ch2_SN[index] = chamb_vals[1].sn[index];
					}
					AmuletChamberSearch_ch2_SN[6] = 0;
				}else{
					AmuletChamberSearch_ch2_SN[0] = 0;
				}

				m_ucClear = 0;
			}

			dateout_language(message, &AmuletChamberSearch_from, 4);
			send_to_amulet_string(131, message);
			delayloop(2);

			dateout_language(message, &AmuletChamberSearch_to, 4);
			send_to_amulet_string(132, message);
			delayloop(2);

			AmuletChamberSearch_displayPage();
			SetAmuletByte(99, 0xFF);

			m_iPhase = PHASE_CHAMBERSEARCH_WAIT;
			break;

		case PHASE_CHAMBERSEARCH_WAIT:
			break;

		case PHASE_CHAMBERSEARCH_PGDOWN:
			beep_amulet();
			AmuletChamberSearch_currentPage++;
			AmuletChamberSearch_displayPage();
			m_iPhase = PHASE_CHAMBERSEARCH_WAIT;
			break;

		case PHASE_CHAMBERSEARCH_PGUP:
			beep_amulet();
			if(AmuletChamberSearch_currentPage){
				AmuletChamberSearch_currentPage--;
				AmuletChamberSearch_displayPage();
			}
			m_iPhase = PHASE_CHAMBERSEARCH_WAIT;
			break;

		case PHASE_CHAMBERSEARCH_ROW1:
		case PHASE_CHAMBERSEARCH_ROW2:
		case PHASE_CHAMBERSEARCH_ROW3:
		case PHASE_CHAMBERSEARCH_ROW4:
		case PHASE_CHAMBERSEARCH_ROW5:
		case PHASE_CHAMBERSEARCH_ROW6:
		case PHASE_CHAMBERSEARCH_ROW7:
		case PHASE_CHAMBERSEARCH_ROW8:
		case PHASE_CHAMBERSEARCH_ROW9:
		case PHASE_CHAMBERSEARCH_ROW10:
			beep_amulet();
			AmuletChamberSearch_currentIndex = (10 * AmuletChamberSearch_currentPage) + (m_iPhase - PHASE_CHAMBERSEARCH_ROW1);
			m_iPhase = PHASE_CHAMBERSEARCH_WAIT;
			break;

		case PHASE_CHAMBERSEARCH_VIEW:
			beep_amulet();
			switch(AmuletChamberSearch_mode){
				case 108:
					AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberDailyTestID;
					DB_RetrieveDailyTest(&AmuletChamberSearchMenu_DailyTest);
					AmuletChamberSearchMenu_Zero.ChamberZeroID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberZeroID;
					DB_RetrieveZeroMeasurement(&AmuletChamberSearchMenu_Zero);
					AmuletChamberSearchMenu_Background.ChamberBackgroundID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberBackgroundID;
					DB_RetrieveBackgroundMeasurement(&AmuletChamberSearchMenu_Background);
					AmuletChamberSearchMenu_Voltage.ChamberVoltageID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberVoltageID;
					DB_RetrieveChamberVoltage(&AmuletChamberSearchMenu_Voltage);
					AmuletChamberSearchMenu_AccuracyTest.ChamberAccuracyTestID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberAccuracyTestID;
					DB_RetrieveAccuracyTest(&AmuletChamberSearchMenu_AccuracyTest, AmuletChamberSearchMenu_AccuracyMeasurement, AmuletChamberSearchMenu_AutoConstancy);
					amulet_page_index = AmuletHTMLIndex[CHAMBERDAILYTESTVIEW_HTM];
					break;

				case 109:
					AmuletChamberSearchMenu_Zero.ChamberZeroID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberZeroID;
					DB_RetrieveZeroMeasurement(&AmuletChamberSearchMenu_Zero);
					amulet_page_index = AmuletHTMLIndex[CHAMBERZEROTESTVIEW_HTM];
					break;

				case 110:
					AmuletChamberSearchMenu_Background.ChamberBackgroundID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberBackgroundID;
					DB_RetrieveBackgroundMeasurement(&AmuletChamberSearchMenu_Background);
					amulet_page_index = AmuletHTMLIndex[CHAMBERBACKGROUNDTESTVIEW_HTM];
					break;

				case 111:
					AmuletChamberSearchMenu_Voltage.ChamberVoltageID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberVoltageID;
					DB_RetrieveChamberVoltage(&AmuletChamberSearchMenu_Voltage);
					amulet_page_index = AmuletHTMLIndex[CHAMBERVOLTAGETESTVIEW_HTM];
					break;

				case 112:
					AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID = 0;
					AmuletChamberSearchMenu_AccuracyTest.ChamberAccuracyTestID = AmuletChamberSearch_test[AmuletChamberSearch_currentIndex].ChamberAccuracyTestID;
					DB_RetrieveAccuracyTest(&AmuletChamberSearchMenu_AccuracyTest, AmuletChamberSearchMenu_AccuracyMeasurement, AmuletChamberSearchMenu_AutoConstancy);
					amulet_page_index = AmuletHTMLIndex[CHAMBERACCURACYTESTVIEW_HTM];
					break;

				default:
					amulet_page_index = AmuletHTMLIndex[MAINSCREEN_HTM];
					break;
			}

			SetAmuletHTML(amulet_page_index);
			PushPageStack(amulet_page_index);
			return;

		case PHASE_CHAMBERSEARCH_SEARCH:
			beep_amulet();
			AmuletChamberSearch_testCount = DB_SearchChamberTests(AmuletChamberSearch_mode, AmuletChamberSearch_test, AmuletChamberSearch_from, AmuletChamberSearch_to, 100);
			AmuletChamberSearch_currentPage = 0;
			if((AmuletChamberSearch_testCount == -1) || (AmuletChamberSearch_testCount == 0)){
				get_amulet_message(L_SEARCH, titlestring);    // "Search"

				if(AmuletChamberSearch_testCount == -1){
					AmuletChamberSearch_testCount = 0;
					get_amulet_message(L_MORE_THAN_100_ITEMS_RETURNED, amstring1);    // "More than 100 items have been returned\nPlease refine criteria"
					//Amulet_DisplayError("Search AutoLinearity Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				}else if(AmuletChamberSearch_testCount == 0){
					get_amulet_message(L_0_ITEMS_RETURNED, amstring1);    // "0 items have been returned"
					//Amulet_DisplayError("Search AutoLinearity Error", "0 items have been returned", TRUE);
				}
				Amulet_DisplayError(titlestring, amstring1, TRUE);
				return;
			}
			m_iPhase = PHASE_CHAMBERSEARCH_PRE_INIT;
			break;

		case PHASE_CHAMBERSEARCH_FROM:
			m_iPhase = PHASE_CHAMBERSEARCH_WAIT;
			break;

		case PHASE_CHAMBERSEARCH_TO:
			m_iPhase = PHASE_CHAMBERSEARCH_WAIT;
			break;

		case PHASE_CHAMBERSEARCH_CRITERIA:
			m_iPhase = PHASE_CHAMBERSEARCH_WAIT;
			break;

		case PHASE_CHAMBERSEARCH_PRINT:
			beep_amulet();

			switch(AmuletChamberSearch_mode){
				//case 108:
				//	get_amulet_message(L_DAILY_TEST, titlestring);    // "Daily Test"
				//	break;

				case 109:
					get_amulet_message(L_ZERO_2, titlestring);    // "Zero"
					break;

				case 110:
					get_amulet_message(L_BACKGROUND2, titlestring);    // "Background"
					break;

				case 111:
					get_amulet_message(L_CHAMBER_VOLTAGE_2, titlestring);    // "Chamber Voltage"
					break;

				//case 112:
				//	get_amulet_message(L_ACCURACYTEST_2, titlestring);    // "Accuracy Test"
				//	break;

				default:
					titlestring[0] = 0;
					break;
			}

			// Print header
			prsetprinter_db();
			if(start_printer(current.printer, 1, FALSE, PAPER)){
				prhead_language_db(current.printer);
				feed(1, current.printer);
				pr_set_linecnt(5);

				// Loop all AmuletChamberSearch_test, retrieve record from database, print divider, print section
				for(index=0; index<AmuletChamberSearch_testCount; index++){
					if(index){
						if(AmuletChamberSearch_mode == 108 || AmuletChamberSearch_mode == 112){
							feed_db(1, current.printer);
							prdivider_db(titlestring);
							feed_db(1, current.printer);
						}else{
							feed_db(1, current.printer);
						}
					}

					switch(AmuletChamberSearch_mode){
						case 108:
							AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID = AmuletChamberSearch_test[index].ChamberDailyTestID;
							DB_RetrieveDailyTest(&AmuletChamberSearchMenu_DailyTest);
							AmuletChamberSearchMenu_Zero.ChamberZeroID = AmuletChamberSearch_test[index].ChamberZeroID;
							DB_RetrieveZeroMeasurement(&AmuletChamberSearchMenu_Zero);
							AmuletChamberSearchMenu_Background.ChamberBackgroundID = AmuletChamberSearch_test[index].ChamberBackgroundID;
							DB_RetrieveBackgroundMeasurement(&AmuletChamberSearchMenu_Background);
							AmuletChamberSearchMenu_Voltage.ChamberVoltageID = AmuletChamberSearch_test[index].ChamberVoltageID;
							DB_RetrieveChamberVoltage(&AmuletChamberSearchMenu_Voltage);
							AmuletChamberSearchMenu_AccuracyTest.ChamberAccuracyTestID = AmuletChamberSearch_test[index].ChamberAccuracyTestID;
							DB_RetrieveAccuracyTest(&AmuletChamberSearchMenu_AccuracyTest, AmuletChamberSearchMenu_AccuracyMeasurement, AmuletChamberSearchMenu_AutoConstancy);
							prdailysection_db(titlestring);
							break;

						case 109:
							AmuletChamberSearchMenu_Zero.ChamberZeroID = AmuletChamberSearch_test[index].ChamberZeroID;
							DB_RetrieveZeroMeasurement(&AmuletChamberSearchMenu_Zero);
							przerosection_db(titlestring, FALSE);
							break;

						case 110:
							AmuletChamberSearchMenu_Background.ChamberBackgroundID = AmuletChamberSearch_test[index].ChamberBackgroundID;
							DB_RetrieveBackgroundMeasurement(&AmuletChamberSearchMenu_Background);
							prbackgroundsection_db(titlestring, FALSE);
							break;

						case 111:
							AmuletChamberSearchMenu_Voltage.ChamberVoltageID = AmuletChamberSearch_test[index].ChamberVoltageID;
							DB_RetrieveChamberVoltage(&AmuletChamberSearchMenu_Voltage);
							prchambervoltagesection_db(titlestring, FALSE);
							break;

						case 112:
							AmuletChamberSearchMenu_DailyTest.ChamberDailyTestID = 0;
							AmuletChamberSearchMenu_AccuracyTest.ChamberAccuracyTestID = AmuletChamberSearch_test[index].ChamberAccuracyTestID;
							DB_RetrieveAccuracyTest(&AmuletChamberSearchMenu_AccuracyTest, AmuletChamberSearchMenu_AccuracyMeasurement, AmuletChamberSearchMenu_AutoConstancy);
							praccuracysection_db(current.printer, titlestring, TRUE);
							break;

						default:
							break;
					}
				}
				formfeed(current.printer);
			}
			m_iPhase = PHASE_CHAMBERSEARCH_PRE_INIT;
			break;
	}
}

static void AmuletChamberSearch_displayPage(void){
	int index;
	char message[26];
	char chamber[31];
	char results[31];
	char startedon[31];
	char amstring1[100];

	AmuletChamberSearch_lastPage = AmuletChamberSearch_testCount / 10;
	if(AmuletChamberSearch_testCount % 10) AmuletChamberSearch_lastPage++;
	AmuletChamberSearch_lastPage--;
	if(AmuletChamberSearch_lastPage < 0) AmuletChamberSearch_lastPage = 0;

	if(AmuletChamberSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletChamberSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletChamberSearch_currentPage == AmuletChamberSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}
	delayloop(2);

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletChamberSearch_currentPage + index) < AmuletChamberSearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletChamberSearch_test[10 * AmuletChamberSearch_currentPage + index].Inactive){
				chamber[0] = 4;
				chamber[1] = 0;
				results[0] = 4;
				results[1] = 0;
				startedon[0] = 4;
				startedon[1] = 0;
			}else{
				chamber[0] = 0;
				results[0] = 0;
				startedon[0] = 0;
			}

			AmuletChamberSearch_SerialNumberString(AmuletChamberSearch_test[10 * AmuletChamberSearch_currentPage + index].SerialNumber, message);
			strcat(chamber, message);
			send_to_amulet_string(3*index + 100, chamber);
			delayloop(2);

			strcat(results, AmuletChamberSearch_test[10 * AmuletChamberSearch_currentPage + index].Results);
			send_to_amulet_string(3*index + 101, results);
			delayloop(2);

			GetExtendedTimeInfoLanguageSec(&AmuletChamberSearch_test[10 * AmuletChamberSearch_currentPage + index].StartedOn, message);
			strcat(startedon, message);
			send_to_amulet_string(3*index + 102, startedon);
			delayloop(2);
		}else{
			SetAmuletByte(101 + index, 0x00);
			send_to_amulet_string(3*index + 100, "");
			send_to_amulet_string(3*index + 101, "");
			send_to_amulet_string(3*index + 102, "");
			delayloop(2);
		}
	}

	message[0] = 0;
	if(AmuletChamberSearch_lastPage > 0){
		get_amulet_message(L_OF, amstring1);    // "%d of %d"
		//sprintf(message, "%d of %d", AmuletChamberSearch_currentPage + 1, AmuletChamberSearch_lastPage + 1);
		sprintf(message, amstring1, AmuletChamberSearch_currentPage + 1, AmuletChamberSearch_lastPage + 1);
	}
	send_to_amulet_string(130, message);
	delayloop(2);

	if(current.printer != NONE_PRINTER && current.printer != USB_EPS_LABEL_PRINTER){
		if(AmuletChamberSearch_testCount > 0) SetAmuletByte(93, 0xFF);
		else SetAmuletByte(93, 0x00);
	}else{
		SetAmuletByte(93, 0x00);
	}

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletChamberSearch_currentIndex = -1;
}

void AmuletChamberSearch_inactivate(int mode, long long int TestID){
	int i;
	long long int id;

	if(AmuletChamberSearch_testCount > 0){
		for(i=0; i<AmuletChamberSearch_testCount; i++){
			switch(mode){
				case 0:
					id = AmuletChamberSearch_test[i].ChamberDailyTestID;
					break;
				case 1:
					id = AmuletChamberSearch_test[i].ChamberZeroID;
					break;
				case 2:
					id = AmuletChamberSearch_test[i].ChamberBackgroundID;
					break;
				case 3:
					id = AmuletChamberSearch_test[i].ChamberVoltageID;
					break;
				case 4:
					id = AmuletChamberSearch_test[i].ChamberAccuracyTestID;
					break;

			}
			if(id == TestID){
				AmuletChamberSearch_test[i].Inactive = TRUE;
				break;
			}
		}
	}
}

void AmuletChamberSearch_SerialNumberString(char *serial_number, char *output){
	char *sn_label;

	sn_label = malloc(30);
	if(strcmp(AmuletChamberSearch_ch1_SN, serial_number) == 0){
		sprintf(output, "CH:1 (%s)", serial_number);
	}else if(strcmp(AmuletChamberSearch_ch2_SN, serial_number) == 0){
		sprintf(output, "CH:2 (%s)", serial_number);
	}else{
		get_amulet_message(L_SN2, sn_label);    // "S/N:"
		sprintf(output, "%s %s", sn_label, serial_number);
	}
	free(sn_label);
}

void AmuletChamberSearch_DateString(time_t datetime, char *output){
	char time_out[28];

	dateout_language(output, &datetime, 4);
	strcat(output, " ");
	timeoutsec(time_out, &datetime);
	strcat(output, time_out);
}

void AmuletChamberSearch_DescString(int chamber_type, bool two_stage, char *output){
	char *stage_string;

	*output = 0;

	switch(chamber_type){
		case R_CHAMB:
			strcat(output, "R ");
			break;

		case P_CHAMB:
			strcat(output, "PET ");
			break;

		case B_CHAMB:
			strcat(output, "BT ");
			break;

		case ONE_DOT_EIGHT_CHAMB:
			strcat(output, "1.8 Atm ");
			break;

		case C_CHAMB:
			strcat(output, "HR ");
			break;

		case K_CHAMB:
			strcat(output, "1K ");
			break;
	}

	if(two_stage) strcat(output, "2 ");
	else strcat(output, "3 ");

	stage_string = malloc(20);
	get_amulet_message(L_STAGE, stage_string);    // "Stage"
	strcat(output, stage_string);
	free(stage_string);
}

void AmuletChamberSearch_PrintSNDescString(char *serial_number, int chamber_type, bool two_stage, char *output){
	char *chamber_string, *sn_label, *desc_string;

	chamber_string = malloc(30);
	sn_label = malloc(30);
	desc_string = malloc(40);
	get_amulet_message(L_CHAMBER, chamber_string);    // "Chamber"
	if(strcmp(AmuletChamberSearch_ch1_SN, serial_number) == 0){
		sprintf(output, "%s #1(%s),", chamber_string, serial_number);
	}else if(strcmp(AmuletChamberSearch_ch2_SN, serial_number) == 0){
		sprintf(output, "%s #2(%s),", chamber_string, serial_number);
	}else{
		get_amulet_message(L_CHAMBER_SN, sn_label);    // "Chamber S/N: %s"
		sprintf(output, sn_label, serial_number);
		strcat(output, ",");
	}
	strcat(output, " ");
	AmuletChamberSearch_DescString(chamber_type, two_stage, desc_string);
	strcat(output, desc_string);

	free(desc_string);
	free(sn_label);
	free(chamber_string);
}

void AmuletChamberSearch_InactiveString(char *inactive_reason, char *output){
	char *reason, *bold, *normal, *inactive_string;

	reason = malloc(50);
	bold = malloc(2);
	bold[0] = 5;
	bold[1] = 0;
	normal = malloc(2);
	normal[0] = 3;
	normal[1] = 0;
	inactive_string = malloc(50);

	strcpy(reason, inactive_reason);
	trim(reason);
	get_amulet_message(L_INACTIVE, inactive_string);    // "Inactive:"
	sprintf(output, "%s%s%s %s", bold, inactive_string, normal, reason);
	free(inactive_string);
	free(normal);
	free(bold);
	free(reason);
}
