#define PHASE_WELLWIPELIST_PRE_INIT	0
#define PHASE_WELLWIPELIST_WAIT		1
#define PHASE_WELLWIPELIST_LOCATIONPGUP	2
#define PHASE_WELLWIPELIST_LOCATIONPGDN	3
#define PHASE_WELLWIPELIST_MEASUREBKG	4
#define PHASE_WELLWIPELIST_ADDALL		5
#define PHASE_WELLWIPELIST_ADD			6
#define PHASE_WELLWIPELIST_REMOVE		7
#define PHASE_WELLWIPELIST_REMOVEALL	8
#define PHASE_WELLWIPELIST_LISTPGUP		9
#define PHASE_WELLWIPELIST_LISTPGDN		10
#define PHASE_WELLWIPELIST_MEASURE		11
#define PHASE_WELLWIPELIST_LOCATION1	17
#define PHASE_WELLWIPELIST_LOCATION2	18
#define PHASE_WELLWIPELIST_LOCATION3	19
#define PHASE_WELLWIPELIST_LOCATION4	20
#define PHASE_WELLWIPELIST_LOCATION5	21
#define PHASE_WELLWIPELIST_LOCATION6	22
#define PHASE_WELLWIPELIST_LOCATION7	23
#define PHASE_WELLWIPELIST_LOCATION8	24
#define PHASE_WELLWIPELIST_LOCATION9	25
#define PHASE_WELLWIPELIST_LOCATION10	26
#define PHASE_WELLWIPELIST_LIST1		33
#define PHASE_WELLWIPELIST_LIST2		34
#define PHASE_WELLWIPELIST_LIST3		35
#define PHASE_WELLWIPELIST_LIST4		36
#define PHASE_WELLWIPELIST_LIST5		37
#define PHASE_WELLWIPELIST_LIST6		38
#define PHASE_WELLWIPELIST_LIST7		39
#define PHASE_WELLWIPELIST_LIST8		40
#define PHASE_WELLWIPELIST_LIST9		41
#define PHASE_WELLWIPELIST_LIST10		42

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "wipes.h"
#include "mca.h"
#include "nuc.h"
#include "database.h"
#include "wipes.h"
#include "amulet.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;
extern unsigned char m_ucSetWellMeasure;
extern bool AmuletWellMeasurementMenu_WipeBackgroundThreshold;
extern bool AmuletWellMeasurementMenu_unlockDetector;
extern bool AmuletWellMeasurementMenu_autoMeasure;
extern volatile char m_acTitle[51];

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
unsigned char PopPageStack(void);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void Amulet_DisplayErrorLarge(char *title, char *errorstring);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void trim(char *acByte);
void AmuletError_setNewPage(uint value);
void PushPageStack(unsigned char ucPage);

static int AmuletWellWipeList_currentAvailablePage;		// Zero based
static int AmuletWellWipeList_currentAvailableIndex; 	// Zero based
static WELLWIPELOCATION *AmuletWellWipeList_available;

static int AmuletWellWipeList_currentSelectedPage;		// Zero based
static int AmuletWellWipeList_currentSelectedIndex;		// Zero based
static WELLWIPELOCATION *AmuletWellWipeList_selected;

static void AmuletWellSetupLocations_displayAvailable(void);
static void AmuletWellSetupLocations_displaySelected(void);

void AmuletWellWipeList_menu(void){
	time_t backgroundstamp;
	char timestring[25], acMsg[100], errorbuffer[4096], errorline[1024];
	float WipeBackgroundTrigger;
	int index, jndex, availableCount, errorcount;
	int nuclide[10];

	switch(m_iPhase){
		case PHASE_WELLWIPELIST_PRE_INIT:
			AmuletWellMeasurementMenu_autoMeasure = FALSE;

			if(m_ucClear == 61){
				Mca_switchToWell();

				InitializeAvailableWipeLocations();
				AmuletWellWipeList_currentAvailablePage = 0;
				AmuletWellWipeList_available = AvailableWipeLocations();
				AmuletWellWipeList_currentSelectedPage = 0;
				AmuletWellWipeList_selected = SelectedWipeLocations();

				// Test for Empty Location List
				if(AvailableWipeLocationCount() == 0){
					PopPageStack();
					Amulet_DisplayError("Wipe Error", "Unable to find any wipe locations\nPlease setup a wipe location", TRUE);
					return;
				}

				// Test autocal
				if(!Mca_getCalibrationStatus()){
					PopPageStack();
					if(Mca_existsAutoCal()){
						Amulet_DisplayError("Calibration Error", "Well parameters have changed\nPlease perform Auto Calibration", TRUE);
					}else{
						Amulet_DisplayError("Calibration Error", "Well has never been calibrated\nPlease perform Auto Calibration first", TRUE);
					}
					return;
				}else if(!Mca_getCalibratedToday()){
					DISPLAY_WARNING("Warning", "Expired calibration\nPlease run Auto Calibration");
				}

				availableCount = AvailableWipeLocationCount();
				errorcount = 0;
				errorbuffer[0] = 0;
				for(index=0; index<availableCount; index++){
					if(errorcount < 15){
						if(TestWipeLocationNuclideMissingE1(index, nuclide)){
							errorline[0] = 0;
							for(jndex=0; jndex<10; jndex++){
								if(nuclide[jndex] >= 0){
									NuclideData_getName(nuclide[jndex], acMsg);
									trim(acMsg);
									if(errorline[0] != 0) strcat(errorline, ", ");
									strcat(errorline, acMsg);
								}
							}

							if(errorline[0] != 0){
								strcat(errorline, " in ");
								strcat(errorline, AmuletWellWipeList_available[index].Name);
								strcat(errorline, " is missing E1(keV)\n");
								strcat(errorbuffer, errorline);
								errorcount++;
							}
						}
					}
				}

				for(index=0; index<availableCount; index++){
					if(errorcount < 15){
						if(TestWipeLocationNuclideMissingEfficiency(index, nuclide)){
							errorline[0] = 0;
							for(jndex=0; jndex<10; jndex++){
								if(nuclide[jndex] >= 0){
									NuclideData_getName(nuclide[jndex], acMsg);
									trim(acMsg);
									if(errorline[0] != 0) strcat(errorline, ", ");
									strcat(errorline, acMsg);
								}
							}

							if(errorline[0] != 0){
								strcat(errorline, " in ");
								strcat(errorline, AmuletWellWipeList_available[index].Name);
								strcat(errorline, " is missing Efficiency\n");
								strcat(errorbuffer, errorline);
								errorcount++;
							}
						}
					}
				}

				if(errorcount){
					PopPageStack();
					Amulet_DisplayErrorLarge("Wipe Location Error", errorbuffer);
					return;
				}

				RefreshWipeNuclides();

				m_ucClear = 0;
			}

			// Test background exists
			// Test background current
			if(Mca_getBackgroundStatus()){
				if(!Mca_getBackgroundToday()){
					backgroundstamp = Mca_getBackgroundStamp();
					GetExtendedTimeInfoSec(&backgroundstamp, timestring);
					strcpy(acMsg, "Background has expired\nBackground measured on ");
					strcat(acMsg, timestring);
					strcat(acMsg, "\nPlease run Background for today");

					DISPLAY_ERROR("Background Error", acMsg);
				}else{
					// Test if background exceeds wipe background thresholds
					WipeBackgroundTrigger = (WipesCurrentType(1))->Threshold;
					if(Mca_getAverageBackgroundCPM(NULL) > WipeBackgroundTrigger){
						if(current.system == CI){
							sprintf(acMsg, "Current background exceeds threshold of %.1f cpm\nRemove any sources from well and try again", WipeBackgroundTrigger);
						}else{
							WipeBackgroundTrigger /= 60.0;
							sprintf(acMsg, "Current background exceeds threshold of %.1f cps\nRemove any sources from well and try again", WipeBackgroundTrigger);
						}
						DISPLAY_ERROR("Background Error", acMsg);
					}
				}
			}else{
				if(Mca_existsBackground()){
					DISPLAY_ERROR("Background Error", "AutoCalibration has invalidated background\nPlease run Background");
				}else{
					DISPLAY_ERROR("Background Error", "Missing Background\nPlease run Background first");
				}
			}

			AmuletWellSetupLocations_displayAvailable();
			AmuletWellSetupLocations_displaySelected();
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_WAIT:
			break;

		case PHASE_WELLWIPELIST_LOCATIONPGUP:
			beep_amulet();
			if(AmuletWellWipeList_currentAvailablePage > 0){
				AmuletWellWipeList_currentAvailablePage--;
				AmuletWellSetupLocations_displayAvailable();
			}
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_LOCATIONPGDN:
			beep_amulet();
			AmuletWellWipeList_currentAvailablePage++;
			AmuletWellSetupLocations_displayAvailable();
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_MEASUREBKG:
			beep_amulet();
			sprintf((char *) m_acTitle, "Measure Background");
			AmuletError_setNewPage(0);
			m_ucSetWellMeasure = 2;
			AmuletWellMeasurementMenu_unlockDetector = FALSE;
			AmuletWellMeasurementMenu_WipeBackgroundThreshold = TRUE;
			m_ucClear = 18;
			SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
			PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
			return;

		case PHASE_WELLWIPELIST_ADDALL:
			beep_amulet();
			AddAllAvailableWipeLocations();
			AmuletWellSetupLocations_displayAvailable();
			AmuletWellSetupLocations_displaySelected();
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_ADD:
			beep_amulet();
			if(AmuletWellWipeList_currentAvailableIndex >= 0){
				AddAvailableWipeLocation(AmuletWellWipeList_currentAvailableIndex);
				AmuletWellSetupLocations_displayAvailable();
				AmuletWellSetupLocations_displaySelected();
			}
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_REMOVE:
			beep_amulet();
			if(AmuletWellWipeList_currentSelectedIndex >= 0){
				RemoveSelectedWipeLocation(AmuletWellWipeList_currentSelectedIndex);
				AmuletWellSetupLocations_displayAvailable();
				AmuletWellSetupLocations_displaySelected();
			}
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_REMOVEALL:
			beep_amulet();
			RemoveAllSelectedWipeLocations();
			AmuletWellSetupLocations_displayAvailable();
			AmuletWellSetupLocations_displaySelected();
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_LISTPGUP:
			beep_amulet();
			if(AmuletWellWipeList_currentSelectedPage > 0){
				AmuletWellWipeList_currentSelectedPage--;
				AmuletWellSetupLocations_displaySelected();
			}
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_LISTPGDN:
			beep_amulet();
			AmuletWellWipeList_currentSelectedPage++;
			AmuletWellSetupLocations_displaySelected();
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_MEASURE:
			beep_amulet();
			AmuletError_setNewPage(0);

			if(Mca_getBackgroundStatus()){
				if(!Mca_getBackgroundToday()){
					backgroundstamp = Mca_getBackgroundStamp();
					GetExtendedTimeInfoSec(&backgroundstamp, timestring);
					strcpy(acMsg, "Background has expired\nBackground measured on ");
					strcat(acMsg, timestring);
					strcat(acMsg, "\nPlease run Background for today");

					DISPLAY_ERROR("Background Error", acMsg);
				}else{
					// Test if background exceeds wipe background thresholds
					WipeBackgroundTrigger = (WipesCurrentType(1))->Threshold;
					if(Mca_getAverageBackgroundCPM(NULL) > WipeBackgroundTrigger){
						if(current.system == CI){
							sprintf(acMsg, "Current background exceeds threshold of %.1f cpm\nRemove any sources from well and try again", WipeBackgroundTrigger);
						}else{
							WipeBackgroundTrigger /= 60.0;
							sprintf(acMsg, "Current background exceeds threshold of %.1f cps\nRemove any sources from well and try again", WipeBackgroundTrigger);
						}
						DISPLAY_ERROR("Background Error", acMsg);
					}else{
						PopPageStack();
						SelectWipeLocationClearIndex();
						m_ucClear = 18;
						m_ucSetWellMeasure = 4;
						AmuletWellMeasurementMenu_unlockDetector = FALSE;
						SetAmuletHTML(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
						PushPageStack(AmuletHTMLIndex[WELLMEASUREMENT_HTM]);
						return;
					}
				}
			}else{
				DISPLAY_ERROR("Background Error", "Missing Background\nPlease run Background first");
			}

			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_LOCATION1:
		case PHASE_WELLWIPELIST_LOCATION2:
		case PHASE_WELLWIPELIST_LOCATION3:
		case PHASE_WELLWIPELIST_LOCATION4:
		case PHASE_WELLWIPELIST_LOCATION5:
		case PHASE_WELLWIPELIST_LOCATION6:
		case PHASE_WELLWIPELIST_LOCATION7:
		case PHASE_WELLWIPELIST_LOCATION8:
		case PHASE_WELLWIPELIST_LOCATION9:
		case PHASE_WELLWIPELIST_LOCATION10:
			beep_amulet();
			AmuletWellWipeList_currentAvailableIndex = (10 * AmuletWellWipeList_currentAvailablePage) + (m_iPhase - PHASE_WELLWIPELIST_LOCATION1);
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;

		case PHASE_WELLWIPELIST_LIST1:
		case PHASE_WELLWIPELIST_LIST2:
		case PHASE_WELLWIPELIST_LIST3:
		case PHASE_WELLWIPELIST_LIST4:
		case PHASE_WELLWIPELIST_LIST5:
		case PHASE_WELLWIPELIST_LIST6:
		case PHASE_WELLWIPELIST_LIST7:
		case PHASE_WELLWIPELIST_LIST8:
		case PHASE_WELLWIPELIST_LIST9:
		case PHASE_WELLWIPELIST_LIST10:
			beep_amulet();
			AmuletWellWipeList_currentSelectedIndex = (10 * AmuletWellWipeList_currentSelectedPage) + (m_iPhase - PHASE_WELLWIPELIST_LIST1);
			m_iPhase = PHASE_WELLWIPELIST_WAIT;
			break;
	}
}

static void AmuletWellSetupLocations_displayAvailable(void){
	int index;
	int availableCount, availableLastPage;
	char message[25];

	availableCount = AvailableWipeLocationCount();
	availableLastPage = availableCount / 10;
	if(availableCount % 10) availableLastPage++;
	availableLastPage--;
	if(availableLastPage < 0) availableLastPage = 0;

	if(AmuletWellWipeList_currentAvailablePage > availableLastPage) AmuletWellWipeList_currentAvailablePage = availableLastPage;

	// Display Up Down Arrows
	if(availableLastPage == 0){
		SetAmuletByte(109, 0xFF);
	}else{
		if(AmuletWellWipeList_currentAvailablePage == 0){
			SetAmuletByte(110, 0xFF);
		}else if(AmuletWellWipeList_currentAvailablePage == availableLastPage){
			SetAmuletByte(112, 0xFF);
		}else{
			SetAmuletByte(111, 0xFF);
		}
	}

	// Display Grid
	if(availableCount == 0) SetAmuletByte(106, 0xFF);
	else SetAmuletByte(105, 0xFF);

	for(index=0; index<10; index++){
		if((10*AmuletWellWipeList_currentAvailablePage + index)< availableCount){
			SetAmuletByte(81 + index, 0xFF);
			SetAmuletString(101 + (2*index), AmuletWellWipeList_available[10*AmuletWellWipeList_currentAvailablePage + index].Name);
		}else{
			SetAmuletByte(81 + index, 0x00);
			SetAmuletString(101 + (2*index), "");
		}
	}

	// Display Page Info
	message[0] = 0;
	if(availableLastPage > 0){
		sprintf(message, "%d of %d", AmuletWellWipeList_currentAvailablePage + 1, availableLastPage + 1);
	}
	SetAmuletString(100, message);

	SetAmuletByte(101, 0xFF);
	SetAmuletByte(103, 0xFF);
	AmuletWellWipeList_currentAvailableIndex = -1;
}

static void AmuletWellSetupLocations_displaySelected(void){
	int index;
	int selectedCount, selectedLastPage;
	char message[25];

	selectedCount = SelectedWipeLocationCount();
	selectedLastPage = selectedCount / 10;
	if(selectedCount % 10) selectedLastPage++;
	selectedLastPage--;
	if(selectedLastPage < 0) selectedLastPage = 0;

	if(AmuletWellWipeList_currentSelectedPage > selectedLastPage) AmuletWellWipeList_currentSelectedPage = selectedLastPage;

	// Display Up Down Arrows
	if(selectedLastPage == 0){
		SetAmuletByte(113, 0xFF);
	}else{
		if(AmuletWellWipeList_currentSelectedPage == 0){
			SetAmuletByte(114, 0xFF);
		}else if(AmuletWellWipeList_currentSelectedPage == selectedLastPage){
			SetAmuletByte(116, 0xFF);
		}else{
			SetAmuletByte(115, 0xFF);
		}
	}

	// Display Grid
	if(selectedCount == 0) SetAmuletByte(108, 0xFF);
	else SetAmuletByte(107, 0xFF);

	for(index=0; index<10; index++){
		if((10*AmuletWellWipeList_currentSelectedPage + index) < selectedCount){
			SetAmuletByte(91 + index, 0xFF);
			SetAmuletString(122 + (2*index), AmuletWellWipeList_selected[10*AmuletWellWipeList_currentSelectedPage + index].Name);
		}else{
			SetAmuletByte(91 + index, 0x00);
			SetAmuletString(122 + (2*index), "");
		}
	}

	// Display Page Info
	message[0] = 0;
	if(selectedLastPage > 0){
		sprintf(message, "%d of %d", AmuletWellWipeList_currentSelectedPage + 1, selectedLastPage + 1);
	}
	SetAmuletString(121, message);
	SetAmuletByte(102, 0xFF);
	SetAmuletByte(104, 0xFF);

	AmuletWellWipeList_currentSelectedIndex = -1;
}
