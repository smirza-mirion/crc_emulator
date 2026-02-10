#define PHASE_WELLAUTOCALSEARCH_PRE_INIT	0
#define PHASE_WELLAUTOCALSEARCH_WAIT		1
#define PHASE_WELLAUTOCALSEARCH_PGDOWN		2
#define PHASE_WELLAUTOCALSEARCH_PGUP		3
#define PHASE_WELLAUTOCALSEARCH_ROW1		4
#define PHASE_WELLAUTOCALSEARCH_ROW2		5
#define PHASE_WELLAUTOCALSEARCH_ROW3		6
#define PHASE_WELLAUTOCALSEARCH_ROW4		7
#define PHASE_WELLAUTOCALSEARCH_ROW5		8
#define PHASE_WELLAUTOCALSEARCH_ROW6		9
#define PHASE_WELLAUTOCALSEARCH_ROW7		10
#define PHASE_WELLAUTOCALSEARCH_ROW8		11
#define PHASE_WELLAUTOCALSEARCH_ROW9		12
#define PHASE_WELLAUTOCALSEARCH_ROW10		13
#define PHASE_WELLAUTOCALSEARCH_VIEW		14
#define PHASE_WELLAUTOCALSEARCH_SEARCH		15
#define PHASE_WELLAUTOCALSEARCH_FROM		16
#define PHASE_WELLAUTOCALSEARCH_TO			17
#define PHASE_WELLAUTOCALSEARCH_CRITERIA	18
#define PHASE_WELLAUTOCALSEARCH_PRINT		19

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
//extern short Mca_installedDetector;
extern AUTOCAL AmuletWellAutoCalReportMenu_test;

time_t AmuletWellAutoCalSearch_from;
time_t AmuletWellAutoCalSearch_to;
int AmuletWellAutoCalSearch_device; // 0 = All, 1 = Probe, 2 = Well

static time_t AmuletWellAutoCalSearch_fromSearched;
static time_t AmuletWellAutoCalSearch_toSearched;
static int AmuletWellAutoCalSearch_testCount;
static int AmuletWellAutoCalSearch_currentPage;	// Zero based
static int AmuletWellAutoCalSearch_lastPage;		// Zero based
static int AmuletWellAutoCalSearch_currentIndex;	// Zero based
static WELLAUTOCALSEARCH AmuletWellAutoCalSearch_test[100];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void PushPageStack(unsigned char ucPage);
static void AmuletWellAutoCalSearch_displayPage(void);
static void AmuletWellAutoCalSearch_print(void);
float GetFwhm(float sigma);

void AmuletWellAutoCalSearch_menu(void){
	char message[26];
	struct tm tmt;

	switch(m_iPhase){
		case PHASE_WELLAUTOCALSEARCH_PRE_INIT:
			if(m_ucClear == 71){
				AmuletWellAutoCalSearch_testCount = 0;
				AmuletWellAutoCalSearch_currentPage = 0;

				memcpy(&tmt, gmtime(&clock_time), sizeof(tmt));
				tmt.tm_hour = 0;
				tmt.tm_min = 0;
				tmt.tm_sec = 0;
				AmuletWellAutoCalSearch_from = mk_time(&tmt);
				AmuletWellAutoCalSearch_to = mk_time(&tmt);

				AmuletWellAutoCalSearch_device = 0;

				m_ucClear = 0;
			}

			dateout(message, &AmuletWellAutoCalSearch_from, 4);
			SetAmuletString(131, message);

			dateout(message, &AmuletWellAutoCalSearch_to, 4);
			SetAmuletString(132, message);

			AmuletWellAutoCalSearch_displayPage();

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(90, 0x00);
			else{
				SetAmuletByte(90, 0xFF);
				switch(AmuletWellAutoCalSearch_device){
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

			//if(AmuletWellAutoCalSearch_testCount && (current.printer != NONE_PRINTER)) SetAmuletByte(98, 0xFF);
			if(AmuletWellAutoCalSearch_testCount && (current.printer != NONE_PRINTER) && (current.printer != USB_EPS_LABEL_PRINTER))
				SetAmuletByte(98, 0xFF);
			else SetAmuletByte(97, 0xFF);
			m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_WAIT:
			break;

		case PHASE_WELLAUTOCALSEARCH_PGDOWN:
			beep_amulet();
			AmuletWellAutoCalSearch_currentPage++;
			AmuletWellAutoCalSearch_displayPage();
			m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_PGUP:
			beep_amulet();
			if(AmuletWellAutoCalSearch_currentPage){
				AmuletWellAutoCalSearch_currentPage--;
				AmuletWellAutoCalSearch_displayPage();
			}
			m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_ROW1:
		case PHASE_WELLAUTOCALSEARCH_ROW2:
		case PHASE_WELLAUTOCALSEARCH_ROW3:
		case PHASE_WELLAUTOCALSEARCH_ROW4:
		case PHASE_WELLAUTOCALSEARCH_ROW5:
		case PHASE_WELLAUTOCALSEARCH_ROW6:
		case PHASE_WELLAUTOCALSEARCH_ROW7:
		case PHASE_WELLAUTOCALSEARCH_ROW8:
		case PHASE_WELLAUTOCALSEARCH_ROW9:
		case PHASE_WELLAUTOCALSEARCH_ROW10:
			beep_amulet();
			AmuletWellAutoCalSearch_currentIndex = (10 * AmuletWellAutoCalSearch_currentPage) + (m_iPhase - PHASE_WELLAUTOCALSEARCH_ROW1);
			m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_VIEW:
			beep_amulet();
			AmuletWellAutoCalReportMenu_test.AutoCalID = AmuletWellAutoCalSearch_test[AmuletWellAutoCalSearch_currentIndex].AutoCalID;
			DB_RetrieveAutoCalTest(&AmuletWellAutoCalReportMenu_test);
			SetAmuletHTML(AmuletHTMLIndex[WELLAUTOCALREPORT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLAUTOCALREPORT_HTM]);
			return;

		case PHASE_WELLAUTOCALSEARCH_SEARCH:
			beep_amulet();
			AmuletWellAutoCalSearch_fromSearched = AmuletWellAutoCalSearch_from;
			AmuletWellAutoCalSearch_toSearched = AmuletWellAutoCalSearch_to;
			AmuletWellAutoCalSearch_testCount = DB_SearchAutoCalTests(AmuletWellAutoCalSearch_test, AmuletWellAutoCalSearch_from, AmuletWellAutoCalSearch_to, 100, AmuletWellAutoCalSearch_device);
			AmuletWellAutoCalSearch_currentPage = 0;
			if(AmuletWellAutoCalSearch_testCount == -1){
				AmuletWellAutoCalSearch_testCount = 0;
				Amulet_DisplayError("Search Auto Cal Error", "More than 100 items have been returned\nPlease refine criteria", TRUE);
				return;
			}else if(AmuletWellAutoCalSearch_testCount == 0){
				Amulet_DisplayError("Search Auto Cal Error", "0 items have been returned", TRUE);
				return;
			}
			m_iPhase = PHASE_WELLAUTOCALSEARCH_PRE_INIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_FROM:
			m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_TO:
			m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_CRITERIA:
			m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			break;

		case PHASE_WELLAUTOCALSEARCH_PRINT:
			beep_amulet();
			AmuletWellAutoCalSearch_print();
			//SetAmuletByte(98, 0xFF);
			//m_iPhase = PHASE_WELLAUTOCALSEARCH_WAIT;
			m_iPhase = PHASE_WELLAUTOCALSEARCH_PRE_INIT;
			break;
	}
}

static void AmuletWellAutoCalSearch_displayPage(void){
	int index;
	char message[26];
	char gain[31];
	char detector[31];
	char measuredon[31];
	float combinedgain1, combinedgain2;

	AmuletWellAutoCalSearch_lastPage = AmuletWellAutoCalSearch_testCount / 10;
	if(AmuletWellAutoCalSearch_testCount % 10) AmuletWellAutoCalSearch_lastPage++;
	AmuletWellAutoCalSearch_lastPage--;
	if(AmuletWellAutoCalSearch_lastPage < 0) AmuletWellAutoCalSearch_lastPage = 0;

	if(AmuletWellAutoCalSearch_lastPage == 0){
		SetAmuletByte(91, 0x00); // Hide arrows
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellAutoCalSearch_currentPage == 0){
			SetAmuletByte(91, 0x00); // Show Down Arrow and Hide Up Arrow
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellAutoCalSearch_currentPage == AmuletWellAutoCalSearch_lastPage){
			SetAmuletByte(91, 0xFF); // Show Up Arrow and Hide Down Arrow
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF); // Show Up and Dow Arrows
			SetAmuletByte(92, 0xFF);
		}
	}

	// Populate Grid and activate rows
	for(index=0; index<10; index++){
		if((10 * AmuletWellAutoCalSearch_currentPage + index) < AmuletWellAutoCalSearch_testCount){
			SetAmuletByte(101 + index, 0xFF);
			if(AmuletWellAutoCalSearch_test[10 * AmuletWellAutoCalSearch_currentPage + index].Inactive){
				gain[0] = 4;
				gain[1] = 0;
				detector[0] = 4;
				detector[1] = 0;
				measuredon[0] = 4;
				measuredon[1] = 0;
			}else{
				gain[0] = 0;
				detector[0] = 0;
				measuredon[0] = 0;
			}

			combinedgain1 = AmuletWellAutoCalSearch_test[10 * AmuletWellAutoCalSearch_currentPage + index].Gain1;
			combinedgain1 /= 64.0;
			combinedgain2 = AmuletWellAutoCalSearch_test[10 * AmuletWellAutoCalSearch_currentPage + index].Gain2;
			combinedgain2 /= 64.0;
			combinedgain1 *= combinedgain2;
			sprintf(message, "%.2f", combinedgain1);
			strcat(gain, message);
			SetAmuletString(3*index + 100, gain);

			switch(AmuletWellAutoCalSearch_test[10 * AmuletWellAutoCalSearch_currentPage + index].DetectorType){
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
			}
			SetAmuletString(3*index + 101, detector);

			GetExtendedTimeInfoSec(&AmuletWellAutoCalSearch_test[10 * AmuletWellAutoCalSearch_currentPage + index].MeasuredOn, message);
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
	if(AmuletWellAutoCalSearch_lastPage > 0){
		sprintf(message, "%d of %d", AmuletWellAutoCalSearch_currentPage + 1, AmuletWellAutoCalSearch_lastPage + 1);
	}
	SetAmuletString(130, message);

	SetAmuletByte(100, 0xFF);
	SetAmuletByte(111, 0xFF);

	AmuletWellAutoCalSearch_currentIndex = -1;
}

static void AmuletWellAutoCalSearch_print(void){
	char prtype;
	char strng[90];
	int index;
	float combinedgain1, combinedgain2;

	prtype = current.printer;
	if(start_printer(prtype, 1, FALSE, PAPER)){
		rawheadersec(prtype, "Auto Calibration Search", clock_time);
		pr_set_linecnt(5);

		lininit(strng, TRUE, prtype);

		insertconst(strng, 4, 1, 0, "From:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellAutoCalSearch_fromSearched, TRUE);
		pr_write2sec(strng, "Auto Calibration Search");

		insertconst(strng, 4, 1, 0, "To:");
		inserttime_tsec(strng, 6, 0, 0, AmuletWellAutoCalSearch_toSearched, TRUE);
		pr_write2sec(strng, "Auto Calibration Search");

		insertconst(strng, 4, 1, 0, "Det:");
		switch(AmuletWellAutoCalSearch_device){
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
		pr_write2sec(strng, "Auto Calibration Search");


		for(index=0; index<AmuletWellAutoCalSearch_testCount; index++){
			if(prtype == ROLL_PRINTER) delayloop(100);
			AmuletWellAutoCalReportMenu_test.AutoCalID = AmuletWellAutoCalSearch_test[index].AutoCalID;
			DB_RetrieveAutoCalTest(&AmuletWellAutoCalReportMenu_test);

			insertconst(strng, 0, 0, 0, "==================================");
			pr_write2sec(strng, "Auto Calibration Search");

			inserttime_tsec(strng, 30, 1, 0, AmuletWellAutoCalReportMenu_test.caltstamp, FALSE);
			pr_write2sec(strng, "Auto Calibration Search");

			insertconst(strng, 4, 1, 0, "Det:");
			switch(AmuletWellAutoCalReportMenu_test.detectortype){
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
			}
			pr_write2sec(strng, "Auto Calibration Search");

			insertconst(strng, 4, 1, 0, "Ch:");
			insertint(strng, 6, 0, 0, "%d", AmuletWellAutoCalReportMenu_test.num_of_channels);
			pr_write2sec(strng, "Auto Calibration Search");

			combinedgain1 = AmuletWellAutoCalReportMenu_test.gain1;
			combinedgain2 = AmuletWellAutoCalReportMenu_test.gain2;
			combinedgain1 /= 64.0;
			combinedgain2 /= 64.0;
			combinedgain1 *= combinedgain2;
			insertconst(strng, 4, 1, 0, "Gain:");
			insertfloat(strng, 6, 0, 0, "%.2f", combinedgain1);
			pr_write2sec(strng, "Auto Calibration Search");

			insertconst(strng, 4, 1, 0, "Zero:");
			insertint(strng, 6, 0, 0, "%d", AmuletWellAutoCalReportMenu_test.zeroopampoffset);
			pr_write2sec(strng, "Auto Calibration Search");

			insertconst(strng, 4, 1, 0, "FWHM:");
			insertfloat(strng, 6, 0, 0, "%.3f %%", GetFwhm(AmuletWellAutoCalReportMenu_test.sigma));
			pr_write2sec(strng, "Auto Calibration Search");

			if(AmuletWellAutoCalReportMenu_test.detectortype == DET_DRILLEDPROBE700){
				insertconst(strng, 8, 1, 0, "FWHM Lip:");
				insertfloat(strng, 10, 0, 0, "%.3f %%", GetFwhm(AmuletWellAutoCalReportMenu_test.sigma_lip));
				pr_write2sec(strng, "Auto Calibration Search");
			}
		}
		formfeed(prtype);
	}
}
