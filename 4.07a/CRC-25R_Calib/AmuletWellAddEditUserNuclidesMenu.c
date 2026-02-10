#define PHASE_WELLADDEDITUSERNUCLIDES_PRE_INIT	0
#define PHASE_WELLADDEDITUSERNUCLIDES_WAIT		1
#define PHASE_WELLADDEDITUSERNUCLIDES_SAVE		2
#define PHASE_WELLADDEDITUSERNUCLIDES_DELETE	3

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"
#include "wipes.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern short AmuletWellSetupUserNuclides_selectedNuclideIndex;

bool AmuletWellAddEditUserNuclides_addNew;
NUCDATA AmuletWellAddEditUserNuclides_temp;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);
void trim_and_shrink(char *acByte);
void Amulet_DisplayError(char *title, char *errorstring, bool showOK);
void tolower(char *c);

void AmuletWellAddEditUserNuclides_menu(void){
	int index, jndex;
	bool flgFound;
	char nuclide_name[10], nuclide_name2[10], message[76], message2[76];
	WELLWIPETYPE *wipetype;
	int wipelocationcount;
	WELLWIPELOCATION *wipelocation;
	//NUCDATA nuclidedata;
	//char message2[76];
	//static char nuclide[51];
	//static float energy1default, energy2default, energy3default, welleffdefault, betaeffdefault, probe700effdefault, well700effdefault;

	switch(m_iPhase){
		case PHASE_WELLADDEDITUSERNUCLIDES_PRE_INIT:
			if(m_ucClear == 74){
				if(AmuletWellAddEditUserNuclides_addNew){
					AmuletWellAddEditUserNuclides_temp.show = 0;
					AmuletWellAddEditUserNuclides_temp.halflife = 0;
					AmuletWellAddEditUserNuclides_temp.response[0] = 0;
					AmuletWellAddEditUserNuclides_temp.response[1] = 0;
					AmuletWellAddEditUserNuclides_temp.user_response[0] = 0;
					AmuletWellAddEditUserNuclides_temp.user_response[1] = 0;
					AmuletWellAddEditUserNuclides_temp.name[0] = 0;
					AmuletWellAddEditUserNuclides_temp.fullname[0] = 0;
					AmuletWellAddEditUserNuclides_temp.code[0] = 0;
					AmuletWellAddEditUserNuclides_temp.hlunit = -1;
					AmuletWellAddEditUserNuclides_temp.energy1 = 0.0;
					AmuletWellAddEditUserNuclides_temp.energy2 = 0.0;
					AmuletWellAddEditUserNuclides_temp.energy3 = 0.0;
					AmuletWellAddEditUserNuclides_temp.wellefficiency = 0.0;
					AmuletWellAddEditUserNuclides_temp.betaefficiency = 0.0;
					AmuletWellAddEditUserNuclides_temp.probe700efficiency = 0.0;
					AmuletWellAddEditUserNuclides_temp.well700efficiency = 0.0;
					AmuletWellAddEditUserNuclides_temp.userenergy1 = -1;
					AmuletWellAddEditUserNuclides_temp.userenergy2 = -1;
					AmuletWellAddEditUserNuclides_temp.userenergy3 = -1;
					AmuletWellAddEditUserNuclides_temp.userwellefficiency = -1;
					AmuletWellAddEditUserNuclides_temp.userbetaefficiency = -1;
					AmuletWellAddEditUserNuclides_temp.userprobe700efficiency = -1;
					AmuletWellAddEditUserNuclides_temp.userwell700efficiency = -1;
					AmuletWellAddEditUserNuclides_temp.wellmeasuredon = (time_t) 0;
					AmuletWellAddEditUserNuclides_temp.betameasuredon = (time_t) 0;
					AmuletWellAddEditUserNuclides_temp.probe700measuredon = (time_t) 0;
					AmuletWellAddEditUserNuclides_temp.well700measuredon = (time_t) 0;
				}else{
					memcpy(&AmuletWellAddEditUserNuclides_temp, NuclideData_getNuclideDataPtr(AmuletWellSetupUserNuclides_selectedNuclideIndex), sizeof(NUCDATA));
				}
				m_ucClear = 0;
			}

			if(AmuletWellAddEditUserNuclides_addNew){
				SetAmuletString(100, "Add User Nuclide");
			}else{
				SetAmuletString(100, "Edit User Nuclide");
				SetAmuletByte(104, 0xFF);
			}
			SetAmuletString(102, AmuletWellAddEditUserNuclides_temp.name);
			SetAmuletString(103, AmuletWellAddEditUserNuclides_temp.fullname);

			if(AmuletWellAddEditUserNuclides_temp.hlunit == -1) message[0] = 0;
			else{
				sprintf(message, "%.2f", AmuletWellAddEditUserNuclides_temp.halflife);
				switch(AmuletWellAddEditUserNuclides_temp.hlunit){
					case SEC:
						strcat(message, " sec");
						break;

					case MIN:
						strcat(message, " min");
						break;

					case HOUR:
						strcat(message, " hr");
						break;

					case DAY:
						strcat(message, " day");
						break;

					case YEAR:
						strcat(message, " yr");
						break;
				}
			}
			SetAmuletString(104, message);

			if(AmuletWellAddEditUserNuclides_temp.energy1 > 0.0){
				sprintf(message, "%.2f", AmuletWellAddEditUserNuclides_temp.energy1);
			}else message[0] = 0;
			SetAmuletString(105, message);

			if(AmuletWellAddEditUserNuclides_temp.energy2 > 0.0){
				sprintf(message, "%.2f", AmuletWellAddEditUserNuclides_temp.energy2);
			}else message[0] = 0;
			SetAmuletString(106, message);

			if(AmuletWellAddEditUserNuclides_temp.energy3 > 0.0){
				sprintf(message, "%.2f", AmuletWellAddEditUserNuclides_temp.energy3);
			}else message[0] = 0;
			SetAmuletString(107, message);

			if(Mca_installedDetector == DET_WELL){
				if(AmuletWellAddEditUserNuclides_temp.wellefficiency > 0.0){
					sprintf(message, "%.3f%%", AmuletWellAddEditUserNuclides_temp.wellefficiency);
				}else message[0] = 0;
				SetAmuletString(108, message);
				message[0] = 0;
				SetAmuletString(109, message);

				if(AmuletWellAddEditUserNuclides_temp.wellmeasuredon == (time_t) 0) message2[0] = 0;
				else{
					strcpy(message2, "Well Efficiency was measured on ");
					GetExtendedTimeInfoSec(&AmuletWellAddEditUserNuclides_temp.wellmeasuredon, message);
					strcat(message2, message);
				}
				SetAmuletString(110, message2);

				message2[0] = 0;
				SetAmuletString(113, message2);
			}else{
				if(AmuletWellAddEditUserNuclides_temp.well700efficiency > 0.0){
					sprintf(message, "%.3f%%", AmuletWellAddEditUserNuclides_temp.well700efficiency);
				}else message[0] = 0;
				SetAmuletString(108, message);

				if(AmuletWellAddEditUserNuclides_temp.probe700efficiency > 0.0){
					sprintf(message, "%.3f%%", AmuletWellAddEditUserNuclides_temp.probe700efficiency);
				}else message[0] = 0;
				SetAmuletString(109, message);

				if(AmuletWellAddEditUserNuclides_temp.well700measuredon == (time_t) 0) message2[0] = 0;
				else{
					strcpy(message2, "Well Efficiency was measured on ");
					GetExtendedTimeInfoSec(&AmuletWellAddEditUserNuclides_temp.well700measuredon, message);
					strcat(message2, message);
				}
				SetAmuletString(110, message2);

				if(AmuletWellAddEditUserNuclides_temp.probe700measuredon == (time_t) 0) message2[0] = 0;
				else{
					strcpy(message2, "Probe Efficiency was measured on ");
					GetExtendedTimeInfoSec(&AmuletWellAddEditUserNuclides_temp.probe700measuredon, message);
					strcat(message2, message);
				}
				SetAmuletString(113, message2);
			}
			SetAmuletByte(100, 0xFF);

			if(Mca_installedDetector != DET_WELL) SetAmuletByte(101, 0xFF);

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(102, 0xFF);
			else if(Mca_installedDetector == DET_DRILLEDPROBE700){
				SetAmuletByte(102, 0xFF);
				SetAmuletByte(103, 0xFF);
			}else{
				if(Mca_well700Installed() == DET_WELL700) SetAmuletByte(102, 0xFF);
				if(Mca_probe700Installed() == DET_PROBE700) SetAmuletByte(103, 0xFF);
			}
			m_iPhase = PHASE_WELLADDEDITUSERNUCLIDES_WAIT;
			break;

		case PHASE_WELLADDEDITUSERNUCLIDES_WAIT:
			break;

		case PHASE_WELLADDEDITUSERNUCLIDES_SAVE:
			// Check Existence of Nuclide Name
			strcpy(message, AmuletWellAddEditUserNuclides_temp.name);
			trim_and_shrink(message);
			if(strlen(message) == 0){
				Amulet_DisplayError("User Nuclide Error", "Missing Nuclide Name", TRUE);
				return;
			}

			// Check Existence of Element
			strcpy(message, AmuletWellAddEditUserNuclides_temp.fullname);
			trim_and_shrink(message);
			if(strlen(message) == 0){
				Amulet_DisplayError("User Nuclide Error", "Missing Element Name", TRUE);
				return;
			}

			// Check Existence of HalfLife
			if(AmuletWellAddEditUserNuclides_temp.hlunit == -1){
				Amulet_DisplayError("User Nuclide Error", "Missing Halflife", TRUE);
				return;
			}else if(AmuletWellAddEditUserNuclides_temp.halflife == 0.0){
				Amulet_DisplayError("User Nuclide Error", "Halflife should be greater than zero", TRUE);
				return;
			}

			// Check Existence of Energy1
			if(AmuletWellAddEditUserNuclides_temp.energy1 == 0){
				Amulet_DisplayError("User Nuclide Error", "Missing Energy 1", TRUE);
				return;
			}

			// Check Existence of Well Efficiency or Probe Efficiency
			if(Mca_installedDetector == DET_WELL){
				if(AmuletWellAddEditUserNuclides_temp.wellefficiency == 0.0){
					Amulet_DisplayError("User Nuclide Error", "Missing Well Efficiency", TRUE);
					return;
				}
			}else{
				if(AmuletWellAddEditUserNuclides_temp.well700efficiency == 0.0 && AmuletWellAddEditUserNuclides_temp.probe700efficiency == 0.0){
					Amulet_DisplayError("User Nuclide Error", "Missing Well or Probe Efficiency", TRUE);
					return;
				}
			}

			// Check for Duplicate of similar Nuclide Name
			strcpy(nuclide_name, AmuletWellAddEditUserNuclides_temp.name);
			trim_and_shrink(nuclide_name);
			tolower(nuclide_name);

			flgFound = FALSE;
			for(index=0; index<BUILTINNUC; index++){
				NuclideData_getName(SortedBuiltInIndex[index], nuclide_name2);
				trim_and_shrink(nuclide_name2);
				tolower(nuclide_name2);
				if(strcmp(nuclide_name, nuclide_name2) == 0){
					flgFound = TRUE;
					break;
				}
			}
			if(flgFound){
				Amulet_DisplayError("User Nuclide Error", "Duplicate Nuclide Name", TRUE);
				return;
			}

			for(index=USERNUC+10; index<USERNUC+20; index++){
				if(NuclideData_getHalflifeUnit(index) != -1){
					NuclideData_getName(index, nuclide_name2);
					trim_and_shrink(nuclide_name2);
					tolower(nuclide_name2);
					if(strcmp(nuclide_name, nuclide_name2) == 0){
						if(AmuletWellAddEditUserNuclides_addNew){
							flgFound = TRUE;
							break;
						}else{
							if(index != AmuletWellSetupUserNuclides_selectedNuclideIndex){
								flgFound = TRUE;
								break;
							}
						}
					}
				}
			}
			if(flgFound){
				Amulet_DisplayError("User Nuclide Error", "Duplicate Nuclide Name", TRUE);
				return;
			}

			// If Edit Nuclide and Nuclide Name is changed, then Check for Nuclide in Wipe Location and Wipe Type, Remove Nuclide from Wipe Location and Wipe Type
			if(!AmuletWellAddEditUserNuclides_addNew){
				NuclideData_getName(AmuletWellSetupUserNuclides_selectedNuclideIndex, nuclide_name2);
				trim_and_shrink(nuclide_name2);
				tolower(nuclide_name2);
				//if((strcmp(nuclide_name, nuclide_name2) != 0) || ((Mca_installedDetector == DET_WELL) && (AmuletWellAddEditUserNuclides_temp.wellefficiency == 0.0)) || ((Mca_installedDetector != DET_WELL) && (AmuletWellAddEditUserNuclides_temp.well700efficiency == 0.0))){
				if(strcmp(nuclide_name, nuclide_name2) != 0){
					// Nuclide Name has changed
					// Remove Nuclide from Wipe Type
					for(index=2; index<6; index++){
						wipetype = WipesCurrentType(index);
						flgFound = FALSE;
						for(jndex=0; jndex<10; jndex++){
							if(wipetype->NuclideID[jndex] >= 0){
								if(wipetype->NuclideID[jndex] == AmuletWellSetupUserNuclides_selectedNuclideIndex){
									flgFound = TRUE;
									wipetype->NuclideID[jndex] = -1;
								}
							}
						}
						if(flgFound) DB_UpdateWellWipeType(wipetype, TRUE);
					}

					// Remove Nuclide from Wipe Locations
					wipelocationcount = ActiveWipeLocationCount();
					if(wipelocationcount > 0){
						wipelocation = ActiveWipeLocations();
						for(index=0; index<wipelocationcount; index++){
							flgFound = FALSE;
							for(jndex=0; jndex<10; jndex++){
								if(wipelocation->NuclideID[jndex] == AmuletWellSetupUserNuclides_selectedNuclideIndex){
									flgFound = TRUE;
									wipelocation->NuclideID[jndex] = -1;
								}
							}
							if(flgFound) DB_UpdateWellWipeLocation(wipelocation, TRUE);

							wipelocation++;
						}
					}
				}
			}

			AmuletWellAddEditUserNuclides_temp.show = -1;
			// Save to NuclideData, userNuclideMirror, and Database;
			if(AmuletWellAddEditUserNuclides_addNew){
				for(index=USERNUC+10; index<USERNUC+20; index++){
					if(NuclideData_getHalflifeUnit(index) == -1){
						NuclideData_updateMirror2(index, &AmuletWellAddEditUserNuclides_temp);
						break;
					}
				}
			}else{
				NuclideData_updateMirror2(AmuletWellSetupUserNuclides_selectedNuclideIndex, &AmuletWellAddEditUserNuclides_temp);
			}
			beep_amulet();
			SetAmuletBackHTML();
			return;

		case PHASE_WELLADDEDITUSERNUCLIDES_DELETE:
			//Check for Nuclide in Wipe Location and Wipe Type, Remove Nuclide from Wipe Location and Wipe Type
			// Remove Nuclide from Wipe Type
			for(index=2; index<6; index++){
				wipetype = WipesCurrentType(index);
				flgFound = FALSE;
				for(jndex=0; jndex<10; jndex++){
					if(wipetype->NuclideID[jndex] >= 0){
						if(wipetype->NuclideID[jndex] == AmuletWellSetupUserNuclides_selectedNuclideIndex){
							flgFound = TRUE;
							wipetype->NuclideID[jndex] = -1;
						}
					}
				}
				if(flgFound) DB_UpdateWellWipeType(wipetype, TRUE);
			}

			// Remove Nuclide from Wipe Locations
			wipelocationcount = ActiveWipeLocationCount();
			if(wipelocationcount > 0){
				wipelocation = ActiveWipeLocations();
				for(index=0; index<wipelocationcount; index++){
					flgFound = FALSE;
					for(jndex=0; jndex<10; jndex++){
						if(wipelocation->NuclideID[jndex] == AmuletWellSetupUserNuclides_selectedNuclideIndex){
							flgFound = TRUE;
							wipelocation->NuclideID[jndex] = -1;
						}
					}
					if(flgFound) DB_UpdateWellWipeLocation(wipelocation, TRUE);

					wipelocation++;
				}
			}

			// Save to NuclideData, userNuclideMirror, and Database;
			AmuletWellAddEditUserNuclides_temp.show = 0;
			AmuletWellAddEditUserNuclides_temp.halflife = 0;
			AmuletWellAddEditUserNuclides_temp.response[0] = 0;
			AmuletWellAddEditUserNuclides_temp.response[1] = 0;
			AmuletWellAddEditUserNuclides_temp.user_response[0] = 0;
			AmuletWellAddEditUserNuclides_temp.user_response[1] = 0;
			AmuletWellAddEditUserNuclides_temp.name[0] = 0;
			AmuletWellAddEditUserNuclides_temp.fullname[0] = 0;
			AmuletWellAddEditUserNuclides_temp.code[0] = 0;
			AmuletWellAddEditUserNuclides_temp.hlunit = -1;
			AmuletWellAddEditUserNuclides_temp.energy1 = 0.0;
			AmuletWellAddEditUserNuclides_temp.energy2 = 0.0;
			AmuletWellAddEditUserNuclides_temp.energy3 = 0.0;
			AmuletWellAddEditUserNuclides_temp.wellefficiency = 0.0;
			AmuletWellAddEditUserNuclides_temp.betaefficiency = 0.0;
			AmuletWellAddEditUserNuclides_temp.probe700efficiency = 0.0;
			AmuletWellAddEditUserNuclides_temp.well700efficiency = 0.0;
			AmuletWellAddEditUserNuclides_temp.userenergy1 = -1;
			AmuletWellAddEditUserNuclides_temp.userenergy2 = -1;
			AmuletWellAddEditUserNuclides_temp.userenergy3 = -1;
			AmuletWellAddEditUserNuclides_temp.userwellefficiency = -1;
			AmuletWellAddEditUserNuclides_temp.userbetaefficiency = -1;
			AmuletWellAddEditUserNuclides_temp.userprobe700efficiency = -1;
			AmuletWellAddEditUserNuclides_temp.userwell700efficiency = -1;
			AmuletWellAddEditUserNuclides_temp.wellmeasuredon = (time_t) 0;
			AmuletWellAddEditUserNuclides_temp.betameasuredon = (time_t) 0;
			AmuletWellAddEditUserNuclides_temp.probe700measuredon = (time_t) 0;
			AmuletWellAddEditUserNuclides_temp.well700measuredon = (time_t) 0;
			NuclideData_updateMirror2(AmuletWellSetupUserNuclides_selectedNuclideIndex, &AmuletWellAddEditUserNuclides_temp);
			beep_amulet();
			SetAmuletBackHTML();
			return;

	}
}
