#define PHASE_WELLSETUPADDEDITLOCATION_PRE_INIT	0
#define PHASE_WELLSETUPADDEDITLOCATION_WAIT		1
#define PHASE_WELLSETUPADDEDITLOCATION_WORKAREA	2
#define PHASE_WELLSETUPADDEDITLOCATION_UNRESTRCTED 3
#define PHASE_WELLSETUPADDEDITLOCATION_SEALED	4
#define PHASE_WELLSETUPADDEDITLOCATION_PACKAGE	5
#define PHASE_WELLSETUPADDEDITLOCATION_SAVE	6

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
extern int AmuletWellSetupLocations_currentIndex;
extern uchar AmuletGenericItems_config;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
static void AmuletWellSetupAddEditLocationMenu_Display(void);
void ThresholdToStr(char *message, char *TypeName, float threshold);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void trim(char *acByte);
void PushPageStack(unsigned char ucPage);
static char titlestring[25];

WELLWIPELOCATION AmuletWellSetupAddEditLocation_wellWipeLocation;

void AmuletWellSetupAddEditLocation_menu(void){
	int index;
	WELLWIPELOCATION *activeLocation;
	char message[51];

	switch(m_iPhase){
		case PHASE_WELLSETUPADDEDITLOCATION_PRE_INIT:
			if((m_ucClear == 58) || (m_ucClear == 59)){
				if(m_ucClear == 58){
					// Add New Location
					strcpy(titlestring, "Add New Wipe Location");
					AmuletWellSetupAddEditLocation_wellWipeLocation.CountTime = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.CreatedOn = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.CreatedOnValue = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.LastUpdated = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.Name[0] = 0;
					for(index=0; index<10; index++) AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index] = -1;
					AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeLocationGroupID = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeLocationID = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeID = 0;
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeName[0] = 0;
				}else if(m_ucClear == 59){
					// Update Location
					strcpy(titlestring, "Edit Wipe Location");
					activeLocation = ActiveWipeLocations();
					DB_CopyWipeLocation(&AmuletWellSetupAddEditLocation_wellWipeLocation, &(activeLocation[AmuletWellSetupLocations_currentIndex]));
				}
				m_ucClear = 0;
			}

			SetAmuletString(98, titlestring);
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID != 0) SetAmuletByte(99, AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID - 1);
			SetAmuletByte(100, 0xFF);
			AmuletWellSetupAddEditLocationMenu_Display();
			m_iPhase = PHASE_WELLSETUPADDEDITLOCATION_WAIT;
			break;

		case PHASE_WELLSETUPADDEDITLOCATION_WAIT:
			break;

		case PHASE_WELLSETUPADDEDITLOCATION_WORKAREA:
			beep_amulet();
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID == 0){
				AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 2;
				AmuletWellSetupAddEditLocationMenu_Display();
			}else{
				if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID != 2){
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 2;
					AmuletGenericItems_config = 20;
					SetAmuletHTML(AmuletHTMLIndex[GENERICITEMS_HTM]);
					PushPageStack(AmuletHTMLIndex[GENERICITEMS_HTM]);
					return;
				}
				//AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 2;
				//AmuletWellSetupAddEditLocationMenu_DisplayThreshold();
			}
			SetAmuletByte(99, 1);
			m_iPhase = PHASE_WELLSETUPADDEDITLOCATION_WAIT;
			break;

		case PHASE_WELLSETUPADDEDITLOCATION_UNRESTRCTED:
			beep_amulet();
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID == 0){
				AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 3;
				AmuletWellSetupAddEditLocationMenu_Display();
			}else{
				if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID != 3){
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 3;
					AmuletGenericItems_config = 20;
					SetAmuletHTML(AmuletHTMLIndex[GENERICITEMS_HTM]);
					PushPageStack(AmuletHTMLIndex[GENERICITEMS_HTM]);
					return;
				}
				//AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 3;
				//AmuletWellSetupAddEditLocationMenu_DisplayThreshold();
			}
			SetAmuletByte(99, 2);
			m_iPhase = PHASE_WELLSETUPADDEDITLOCATION_WAIT;
			break;

		case PHASE_WELLSETUPADDEDITLOCATION_SEALED:
			beep_amulet();
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID == 0){
				AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 4;
				AmuletWellSetupAddEditLocationMenu_Display();
			}else{
				if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID != 4){
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 4;
					AmuletGenericItems_config = 20;
					SetAmuletHTML(AmuletHTMLIndex[GENERICITEMS_HTM]);
					PushPageStack(AmuletHTMLIndex[GENERICITEMS_HTM]);
					return;
				}
				//AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 4;
				//AmuletWellSetupAddEditLocationMenu_DisplayThreshold();
			}
			SetAmuletByte(99, 3);
			m_iPhase = PHASE_WELLSETUPADDEDITLOCATION_WAIT;
			break;

		case PHASE_WELLSETUPADDEDITLOCATION_PACKAGE:
			beep_amulet();
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID == 0){
				AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 5;
				AmuletWellSetupAddEditLocationMenu_Display();
			}else{
				if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID != 5){
					AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 5;
					AmuletGenericItems_config = 20;
					SetAmuletHTML(AmuletHTMLIndex[GENERICITEMS_HTM]);
					PushPageStack(AmuletHTMLIndex[GENERICITEMS_HTM]);
					return;
				}
				//AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID = 5;
				//AmuletWellSetupAddEditLocationMenu_DisplayThreshold();
			}
			SetAmuletByte(99, 4);
			m_iPhase = PHASE_WELLSETUPADDEDITLOCATION_WAIT;
			break;

		case PHASE_WELLSETUPADDEDITLOCATION_SAVE:
			beep_amulet();
			// Check for Location
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.Name[0] == 0){
				Amulet_DisplayError("Wipe Location", "Missing Location Name", TRUE);
				return;
			}
			// Check for duplicate Location
			if(FindWipeLocation(AmuletWellSetupAddEditLocation_wellWipeLocation.Name, AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeLocationID)){
				Amulet_DisplayError("Wipe Location", "Duplicate Location Name", TRUE);
				return;
			}

			// Check for Threshold
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold <= 0){
				Amulet_DisplayError("Wipe Location", "Missing Trigger Level", TRUE);
				return;
			}

			// Check for Count time
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.CountTime <= 0){
				Amulet_DisplayError("Wipe Location", "Missing Count Time", TRUE);
				return;
			}

			// Check NuclideIDs
			for(index=0; index<10; index++){
				if(AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index] >= 0){
					if(NuclideData_getEffectivePrimary(AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index]) <= 0.0){
						NuclideData_getName(AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index], message);
						trim(message);
						strcat(message, " does not have E1(keV)");
						Amulet_DisplayError("Wipe Location", message, TRUE);
						return;
					}

					if(NuclideData_getEffectiveEffSkewWell(AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index], Mca_installedDetector) <= 0.0){
						NuclideData_getName(AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index], message);
						trim(message);
						strcat(message, " does not have Well Efficiency");
						Amulet_DisplayError("Wipe Location", message, TRUE);
						return;
					}
				}
			}

			UpdateWipeLocation(&AmuletWellSetupAddEditLocation_wellWipeLocation);
			SetAmuletBackHTML();
			break;
	}
}

static void AmuletWellSetupAddEditLocationMenu_Display(void){
	WELLWIPETYPE *wipeType;
	char message[25];
	int index;

	if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID != 0){
		if(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeID == 0){
			wipeType = WipesCurrentType(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID);
			AmuletWellSetupAddEditLocation_wellWipeLocation.CountTime = wipeType->CountTime;
			for(index=0; index<10; index++) AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index] = wipeType->NuclideID[index];
			AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold = wipeType->Threshold;
			AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeID = wipeType->WellWipeTypeID;
			strcpy(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeName, wipeType->Name);
		}

		SetAmuletString(100, AmuletWellSetupAddEditLocation_wellWipeLocation.Name);

		if(AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold > 0){
			ThresholdToStr(message, AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeName, AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold);
		}else{
			message[0] = 0;
		}
		SetAmuletString(102, message);

		if(AmuletWellSetupAddEditLocation_wellWipeLocation.CountTime > 0){
			sprintf(message, "%d sec", AmuletWellSetupAddEditLocation_wellWipeLocation.CountTime);
		}else{
			message[0] = 0;
		}
		SetAmuletString(103, message);

		for(index=0; index<10; index++){
			if(AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index] >= 0){
				NuclideData_getName(AmuletWellSetupAddEditLocation_wellWipeLocation.NuclideID[index], message);
			}else{
				message[0] = 0;
			}
			SetAmuletString(104 + index, message);
		}

		SetAmuletByte(101, 0xFF);
	}
}

/*static void AmuletWellSetupAddEditLocationMenu_DisplayThreshold(void){
	char message[25];
	WELLWIPETYPE *wipeType;

	wipeType = WipesCurrentType(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeGroupID);
	AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeID = wipeType->WellWipeTypeID;
	strcpy(AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeName, wipeType->Name);

	if(AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold > 0){
		ThresholdToStr(message, AmuletWellSetupAddEditLocation_wellWipeLocation.WellWipeTypeName, AmuletWellSetupAddEditLocation_wellWipeLocation.Threshold);
	}else{
		message[0] = 0;
	}
	SetAmuletString(102, message);
	SetAmuletByte(102, 0xFF);
}*/
