#define PHASE_WELLSETUPLOCATIONS_PRE_INIT	0
#define PHASE_WELLSETUPLOCATIONS_WAIT		1
#define PHASE_WELLSETUPLOCATIONS_PGDOWN		2
#define PHASE_WELLSETUPLOCATIONS_PGUP		3
#define PHASE_WELLSETUPLOCATIONS_ROW1		4
#define PHASE_WELLSETUPLOCATIONS_ROW2		5
#define PHASE_WELLSETUPLOCATIONS_ROW3		6
#define PHASE_WELLSETUPLOCATIONS_ROW4		7
#define PHASE_WELLSETUPLOCATIONS_ROW5		8
#define PHASE_WELLSETUPLOCATIONS_ROW6		9
#define PHASE_WELLSETUPLOCATIONS_ROW7		10
#define PHASE_WELLSETUPLOCATIONS_ROW8		11
#define PHASE_WELLSETUPLOCATIONS_ROW9		12
#define PHASE_WELLSETUPLOCATIONS_ROW10		13

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "wipes.h"
#include "mca.h"
#include "nuc.h"
#include "database.h"
#include "wipes.h"

extern int m_iPhase;
extern CURRENT current;

static int AmuletWellSetupLocations_count;
WELLWIPELOCATION *AmuletWellSetupLocations;
static int AmuletWellSetupLocations_currentPage;	// Zero based
static int AmuletWellSetupLocations_lastPage;		// Zero based
int AmuletWellSetupLocations_currentIndex;	// Zero based

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);

static void AmuletWellSetupLocations_displayPage(void);
void ThresholdToStr(char *message, char *TypeName, float threshold);

void AmuletWellSetupLocations_menu(void){
	switch(m_iPhase){
		case PHASE_WELLSETUPLOCATIONS_PRE_INIT:
			AmuletWellSetupLocations_count = ActiveWipeLocationCount();
			if(AmuletWellSetupLocations_count == 0) AmuletWellSetupLocations_lastPage = 0;
			else{
				AmuletWellSetupLocations_lastPage = AmuletWellSetupLocations_count / 10;
				if(AmuletWellSetupLocations_count % 10) AmuletWellSetupLocations_lastPage++;
				AmuletWellSetupLocations_lastPage--;
			}
			AmuletWellSetupLocations = ActiveWipeLocations();
			AmuletWellSetupLocations_currentPage = 0;
			AmuletWellSetupLocations_displayPage();
			m_iPhase = PHASE_WELLSETUPLOCATIONS_WAIT;
			break;

		case PHASE_WELLSETUPLOCATIONS_WAIT:
			break;

		case PHASE_WELLSETUPLOCATIONS_PGDOWN:
			if(AmuletWellSetupLocations_currentPage < AmuletWellSetupLocations_lastPage){
				beep_amulet();
				AmuletWellSetupLocations_currentPage++;
				AmuletWellSetupLocations_displayPage();
			}
			m_iPhase = PHASE_WELLSETUPLOCATIONS_WAIT;
			break;

		case PHASE_WELLSETUPLOCATIONS_PGUP:
			if(AmuletWellSetupLocations_currentPage > 0){
				beep_amulet();
				AmuletWellSetupLocations_currentPage--;
				AmuletWellSetupLocations_displayPage();
			}
			m_iPhase = PHASE_WELLSETUPLOCATIONS_WAIT;
			break;

		case PHASE_WELLSETUPLOCATIONS_ROW1:
		case PHASE_WELLSETUPLOCATIONS_ROW2:
		case PHASE_WELLSETUPLOCATIONS_ROW3:
		case PHASE_WELLSETUPLOCATIONS_ROW4:
		case PHASE_WELLSETUPLOCATIONS_ROW5:
		case PHASE_WELLSETUPLOCATIONS_ROW6:
		case PHASE_WELLSETUPLOCATIONS_ROW7:
		case PHASE_WELLSETUPLOCATIONS_ROW8:
		case PHASE_WELLSETUPLOCATIONS_ROW9:
		case PHASE_WELLSETUPLOCATIONS_ROW10:
			beep_amulet();
			AmuletWellSetupLocations_currentIndex = 10 * AmuletWellSetupLocations_currentPage + (m_iPhase - PHASE_WELLSETUPLOCATIONS_ROW1);
			m_iPhase = PHASE_WELLSETUPLOCATIONS_WAIT;
			break;
	}
}

static void AmuletWellSetupLocations_displayPage(void){
	int index, indexbegin;
	char message[51];

	if(AmuletWellSetupLocations_lastPage == 0){
		SetAmuletByte(91, 0x00);
		SetAmuletByte(92, 0x00);
	}else{
		if(AmuletWellSetupLocations_currentPage == 0){
			SetAmuletByte(91, 0x00);
			SetAmuletByte(92, 0xFF);
		}else if(AmuletWellSetupLocations_currentPage == AmuletWellSetupLocations_lastPage){
			SetAmuletByte(91, 0xFF);
			SetAmuletByte(92, 0x00);
		}else{
			SetAmuletByte(91, 0xFF);
			SetAmuletByte(92, 0xFF);
		}
	}

	if(AmuletWellSetupLocations_count > 0){
		indexbegin = 10 * AmuletWellSetupLocations_currentPage;
		for(index=indexbegin; index<indexbegin+10; index++){
			if(index < AmuletWellSetupLocations_count){
				SetAmuletString(100 + 5*(index-indexbegin), AmuletWellSetupLocations[index].Name);
				SetAmuletString(100 + 5*(index-indexbegin) + 2, AmuletWellSetupLocations[index].WellWipeTypeName);
				ThresholdToStr(message, AmuletWellSetupLocations[index].WellWipeTypeName, AmuletWellSetupLocations[index].Threshold);
				SetAmuletString(100 + 5*(index-indexbegin) + 3, message);
				SetAmuletByte(101 + (index-indexbegin), 0xFF);
			}else{
				SetAmuletString(100 + 5 * (index-indexbegin), "");
				SetAmuletString(100 + 5 * (index-indexbegin) + 2, "");
				SetAmuletString(100 + 5 * (index-indexbegin) + 3, "");
				SetAmuletByte(101 + (index-indexbegin), 0);
			}
		}

		message[0] = 0;
		if(AmuletWellSetupLocations_lastPage > 0){
			sprintf(message, "%d of %d", AmuletWellSetupLocations_currentPage + 1, AmuletWellSetupLocations_lastPage + 1);
		}
		SetAmuletString(150, message);

		SetAmuletByte(100, 0xFF);
		SetAmuletByte(111, 0xFF);
	}
	AmuletWellSetupLocations_currentIndex = -1;
}

void ThresholdToStr(char *message, char *TypeName, float threshold){
	float convertedvalue;

	if(current.system == CI){
		if(strcmp(TypeName, "Background") == 0){
			sprintf(message, "%.2f cpm", threshold);
		}else if(strcmp(TypeName, "Sealed Source") == 0){
			convertedvalue = Mca_convertDpmToCi(threshold);
			convertedvalue *= 1e+9;
			sprintf(message, "%.3f nCi", convertedvalue);
		}else{
			sprintf(message, "%.2f dpm", threshold);
		}
	}else{
		if(strcmp(TypeName, "Background") == 0){
			sprintf(message, "%.3f cps", Mca_convertDpmToBq(threshold));
		}else{
			sprintf(message, "%.3f Bq", Mca_convertDpmToBq(threshold));
		}
	}
}
