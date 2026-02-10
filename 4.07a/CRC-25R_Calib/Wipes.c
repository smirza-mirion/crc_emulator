#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "crc.h"
#include "mca.h"
#include "wipes.h"
#include "database.h"
#include "nuc.h"

extern bool AmuletWellMeasurementMenu_autoMeasure;
extern char AmuletWellMeasurementMenu_testID[11];
extern char AmuletWellMeasurementMenu_testIDPCComm[11];
extern char AmuletInitialRecoveryMsg[100];

static WELLWIPETYPE currentBackgroundType;
static WELLWIPETYPE currentWorkAreaType;
static WELLWIPETYPE currentUnrestrictedAreaType;
static WELLWIPETYPE currentSealedSourceType;
static WELLWIPETYPE currentPackageType;

static int LocationCount;
static int LocationMax;
static WELLWIPELOCATION *activeWipeLocations;

static int AvailableCount;
static int AvailableMax;
static WELLWIPELOCATION *availableWipeLocations;

static int SelectedCount;
static int SelectedMax;
static int SelectedIndex;
static WELLWIPELOCATION *selectedWipeLocations;

static int WipeNuclideCount;
static int WipeNuclideMax;
static WELLWIPENUCLIDE *activeWipeNuclides;

void trim(char *acByte);
void toupper(char *acByte);

void WipesInitialize(void){
	strcpy(currentBackgroundType.Name, "Background");
	DB_readCurrentWellWipeType(&currentBackgroundType);
	strcpy(currentWorkAreaType.Name, "Work Area");
	DB_readCurrentWellWipeType(&currentWorkAreaType);
	strcpy(currentUnrestrictedAreaType.Name, "Unrestricted Area");
	DB_readCurrentWellWipeType(&currentUnrestrictedAreaType);
	strcpy(currentSealedSourceType.Name, "Sealed Source");
	DB_readCurrentWellWipeType(&currentSealedSourceType);
	strcpy(currentPackageType.Name, "Package");
	DB_readCurrentWellWipeType(&currentPackageType);

	if((currentBackgroundType.WellWipeTypeID == 0) || (currentWorkAreaType.WellWipeTypeID == 0) || (currentUnrestrictedAreaType.WellWipeTypeID == 0) || (currentSealedSourceType.WellWipeTypeID == 0) || (currentPackageType.WellWipeTypeID == 0)){
		if(AmuletInitialRecoveryMsg[0] == 0){
			DB_FixWellWipeType();
			//DB_readCurrentWellWipeType(&currentBackgroundType);
			//DB_readCurrentWellWipeType(&currentWorkAreaType);
			//DB_readCurrentWellWipeType(&currentUnrestrictedAreaType);
			//DB_readCurrentWellWipeType(&currentSealedSourceType);
			//DB_readCurrentWellWipeType(&currentPackageType);
			strcpy(AmuletInitialRecoveryMsg, "Error in Wipe Type Table\nRestoring default settings\nPlease Restart");
		}
	}

	activeWipeLocations = NULL;
	LocationCount = 0;
	LocationMax = 0;
	DB_ReadCurrentWipeLocations(&LocationCount, &LocationMax, &activeWipeLocations, &availableWipeLocations, &selectedWipeLocations);
	AvailableMax = LocationMax;
	SelectedMax = LocationMax;

	activeWipeNuclides = NULL;
	WipeNuclideCount = 0;
	WipeNuclideMax = 0;
	DB_ReadCurrentWipeNuclides(&WipeNuclideCount, &WipeNuclideMax, &activeWipeNuclides);
	AmuletWellMeasurementMenu_autoMeasure = FALSE;
	AmuletWellMeasurementMenu_testID[0] = 0;
	AmuletWellMeasurementMenu_testIDPCComm[0] = 0;
}

WELLWIPETYPE *WipesCurrentType(int type){
	switch(type){
		case 1:
			return &currentBackgroundType;

		case 2:
			return &currentWorkAreaType;

		case 3:
			return &currentUnrestrictedAreaType;

		case 4:
			return &currentSealedSourceType;

		case 5:
			return &currentPackageType;

		default:
			return NULL;
	}
}

int ActiveWipeLocationCount(void){
	return LocationCount;
}

WELLWIPELOCATION *ActiveWipeLocations(void){
	return activeWipeLocations;
}

void DeleteWipeLocation(WELLWIPELOCATION *delItem){
	WELLWIPELOCATION *copy;
	int index, jndex;

	DB_DeleteCurrentWipeLocation(delItem, TRUE);
	if(delItem->WellWipeLocationID){
		copy = malloc((LocationCount) * sizeof(WELLWIPELOCATION));
		jndex = 0;
		for(index=0; index<LocationCount; index++){
			if(activeWipeLocations[index].WellWipeLocationID != delItem->WellWipeLocationID){
				DB_CopyWipeLocation(&(copy[jndex]), &(activeWipeLocations[index]));
				jndex++;
			}
		}

		LocationCount = jndex;
		for(index=0; index<LocationCount; index++) DB_CopyWipeLocation(&(activeWipeLocations[index]), &(copy[index]));

		free(copy);
	}
}

void UpdateWipeLocation(WELLWIPELOCATION *updateItem){
	WELLWIPELOCATION *NewLocations;
	int index;

	if(updateItem->WellWipeLocationID == 0){
		// Create New Location
		DB_CreateWellWipeLocation(updateItem, TRUE);
		if(updateItem->WellWipeLocationID != 0){
			if(LocationCount == LocationMax){
				LocationMax += 50;
				NewLocations = malloc(LocationMax * sizeof(WELLWIPELOCATION));
				free(availableWipeLocations);
				AvailableMax = LocationMax;
				availableWipeLocations = malloc(AvailableMax * sizeof(WELLWIPELOCATION));
				free(selectedWipeLocations);
				SelectedMax = LocationMax;
				selectedWipeLocations = malloc(SelectedMax * sizeof(WELLWIPELOCATION));

				for(index=0; index<LocationCount; index++){
					DB_CopyWipeLocation(&(NewLocations[index]), &(activeWipeLocations[index]));
				}
				free(activeWipeLocations);
				activeWipeLocations = NewLocations;
			}

			DB_CopyWipeLocation(&(activeWipeLocations[LocationCount]), updateItem);
			LocationCount++;
		}
	}else{
		// Update Location
		DB_UpdateWellWipeLocation(updateItem, TRUE);
		if(updateItem->WellWipeLocationID != 0){
			for(index=0; index<LocationCount; index++){
				if(updateItem->WellWipeLocationGroupID == activeWipeLocations[index].WellWipeLocationGroupID){
					DB_CopyWipeLocation(&(activeWipeLocations[index]), updateItem);
				}
			}
		}
	}
}

int FindWipeLocation(char *locationName, long long int WellWipeLocationID){
	char locName[31];
	char activeName[31];
	int index;
	int returnvalue;

	returnvalue = 0;
	if(LocationCount > 0){
		strcpy(locName, locationName);
		trim(locName);
		toupper(locName);

		for(index=0; index<LocationCount; index++){
			strcpy(activeName, activeWipeLocations[index].Name);
			trim(activeName);
			toupper(activeName);
			if(strcmp(locName, activeName) == 0){
				if(activeWipeLocations[index].WellWipeLocationID != WellWipeLocationID){
					returnvalue = 1;
					break;
				}
			}
		}
	}
	return returnvalue;
}

void InitializeAvailableWipeLocations(void){
	int index;

	AvailableCount = LocationCount;
	if(LocationCount > 0){
		for(index=0; index<LocationCount; index++){
			DB_CopyWipeLocation(&(availableWipeLocations[index]), &(activeWipeLocations[index]));
		}
	}
	SelectedCount = 0;
}

void RemoveAllSelectedWipeLocations(void){
	int index;

	if(SelectedCount > 0){
		for(index=0; index<SelectedCount; index++){
			DB_CopyWipeLocation(&(availableWipeLocations[AvailableCount+index]), &(selectedWipeLocations[index]));
		}
		AvailableCount += SelectedCount;
		SelectedCount = 0;
	}
}

void AddAllAvailableWipeLocations(void){
	int index;

	if(AvailableCount > 0){
		for(index=0; index<AvailableCount; index++){
			DB_CopyWipeLocation(&(selectedWipeLocations[SelectedCount+index]), &(availableWipeLocations[index]));
		}
		SelectedCount += AvailableCount;
		AvailableCount = 0;
	}
}

int AvailableWipeLocationCount(void){
	return AvailableCount;
}

WELLWIPELOCATION *AvailableWipeLocations(void){
	return availableWipeLocations;
}

void SelectWipeLocationClearIndex(void){
	SelectedIndex = 0;
}

int GetSelectWipeLocationIndex(void){
	return SelectedIndex;
}

void IncSelectWipeLocationIndex(void){
	SelectedIndex++;
}

void EndSelectWipeLocationIndex(void){
	SelectedIndex = SelectedCount;
}

int SelectedWipeLocationCount(void){
	return SelectedCount;
}

WELLWIPELOCATION *SelectedWipeLocations(void){
	return selectedWipeLocations;
}

static void RemoveAndCompress(WELLWIPELOCATION *array, int remove_index, int arraysize){
	int index;

	if(remove_index < (arraysize - 1)){
		for(index=remove_index+1; index<arraysize; index++){
			DB_CopyWipeLocation(&(array[index-1]), &(array[index]));
		}
	}
}

void AddAvailableWipeLocation(int index){
	DB_CopyWipeLocation(&(selectedWipeLocations[SelectedCount]), &(availableWipeLocations[index]));
	RemoveAndCompress(availableWipeLocations, index, AvailableCount);
	AvailableCount--;
	SelectedCount++;
}

void RemoveSelectedWipeLocation(int index){
	DB_CopyWipeLocation(&(availableWipeLocations[AvailableCount]), &(selectedWipeLocations[index]));
	RemoveAndCompress(selectedWipeLocations, index, SelectedCount);
	SelectedCount--;
	AvailableCount++;
}

bool TestWipeLocationNuclideMissingE1(int locationIndex, int nuclide[10]){
	int index, nuclideindex;

	for(index=0; index<10; index++) nuclide[index] = -1;

	nuclideindex = 0;
	for(index=0; index<10; index++){
		if(availableWipeLocations[locationIndex].NuclideID[index] >= 0){
			if(NuclideData_getEffectivePrimary(availableWipeLocations[locationIndex].NuclideID[index]) <= 0.0){
				nuclide[nuclideindex] = availableWipeLocations[locationIndex].NuclideID[index];
				nuclideindex++;
			}
		}
	}

	if(nuclideindex == 0) return FALSE;
	else return TRUE;
}

bool TestWipeLocationNuclideMissingEfficiency(int locationIndex, int nuclide[10]){
	int index, nuclideindex;

	for(index=0; index<10; index++) nuclide[index] = -1;

	nuclideindex = 0;
	for(index=0; index<10; index++){
		if(availableWipeLocations[locationIndex].NuclideID[index] >= 0){
			if(NuclideData_getEffectiveEffSkewWell(availableWipeLocations[locationIndex].NuclideID[index], Mca_installedDetector) <= 0.0){
				nuclide[nuclideindex] = availableWipeLocations[locationIndex].NuclideID[index];
				nuclideindex++;
			}
		}
	}

	if(nuclideindex == 0) return FALSE;
	else return TRUE;
}

void RefreshWipeNuclides(void){
	int NuclideID[200], index, jndex, kndex, lndex;
	int NuclideCount;
	bool flgFound;

	NuclideCount = 0;

	if(AvailableCount > 0){
		for(index=0; index<200; index++) NuclideID[index] = -1;
		// Get Unique NuclideID
		lndex = 0;
		for(index=0; index<AvailableCount; index++){
			for(jndex=0; jndex<10; jndex++){
				if(availableWipeLocations[index].NuclideID[jndex]>=0){
					flgFound = FALSE;
					for(kndex=0; kndex<200; kndex++){
						if(availableWipeLocations[index].NuclideID[jndex] == NuclideID[kndex]){
							flgFound = TRUE;
							break;
						}
					}

					if(!flgFound){
						NuclideID[lndex] = availableWipeLocations[index].NuclideID[jndex];
						lndex++;
					}
				}
			}
		}

		NuclideCount = lndex;
	}

	if(NuclideCount > 0){
		for(index=0; index<NuclideCount; index++){
			DB_RefreshCurrentWipeNuclides(&WipeNuclideCount, &WipeNuclideMax, &activeWipeNuclides, NuclideID[index]);
		}
	}
}

WELLWIPENUCLIDE *GetWipeNuclide(int NuclideID){
	int index;
	WELLWIPENUCLIDE *returnvalue;

	returnvalue = NULL;
	for(index=0; index<WipeNuclideCount; index++){
		if(activeWipeNuclides[index].NuclideID == NuclideID){
			returnvalue = &(activeWipeNuclides[index]);
			break;
		}
	}
	return returnvalue;
}
