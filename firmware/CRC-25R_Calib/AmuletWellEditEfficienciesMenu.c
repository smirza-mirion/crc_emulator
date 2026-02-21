#define PHASE_WELLEDITEFFICIENCIES_PRE_INIT	0
#define PHASE_WELLEDITEFFICIENCIES_WAIT		1
#define PHASE_WELLEDITEFFICIENCIES_PAINT	2
#define PHASE_WELLEDITEFFICIENCIES_DEFAULT_ENERGY1	3
#define PHASE_WELLEDITEFFICIENCIES_DEFAULT_ENERGY2	4
#define PHASE_WELLEDITEFFICIENCIES_DEFAULT_ENERGY3	5
#define PHASE_WELLEDITEFFICIENCIES_DEFAULT_WELL_EFF	6
#define PHASE_WELLEDITEFFICIENCIES_DEFAULT_PROBE_EFF	7
#define PHASE_WELLEDITEFFICIENCIES_SAVE				8

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern short AmuletWellSetupEfficienies_selectedItem;
extern CURRENT current;

SETUP_EFFICIENCY AmuletWellEditEfficiencies_temp;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void GetExtendedNuclideInfo(unsigned char ucNuclideID, NUCDATA *output_nuc, char *acMsg);
void SetAmuletBackHTML(void);
void GetExtendedTimeInfoSec(time_t *dtmDateTime, char *acMsg);

void AmuletWellEditEfficiencies_menu(void){
	NUCDATA nuclidedata;
	char message[51];
	char message2[76];
	static char nuclide[51];
	//static float energy1default, energy2default, energy3default, welleffdefault, betaeffdefault, probe700effdefault, well700effdefault;
	static float energy1default, energy2default, energy3default, welleffdefault, probe700effdefault, well700effdefault;

	switch(m_iPhase){
		case PHASE_WELLEDITEFFICIENCIES_PRE_INIT:
			if(m_ucClear == 25){
				if(((AmuletWellSetupEfficienies_selectedItem >= 0) && (AmuletWellSetupEfficienies_selectedItem <= USERNUC-5)) || ((AmuletWellSetupEfficienies_selectedItem >= (USERNUC+20)) && (AmuletWellSetupEfficienies_selectedItem < ALLNUC))){
					GetExtendedNuclideInfo(AmuletWellSetupEfficienies_selectedItem, &nuclidedata, nuclide);
					energy1default = NuclideData_getPrimary(AmuletWellSetupEfficienies_selectedItem);
					energy2default = NuclideData_getSecondary(AmuletWellSetupEfficienies_selectedItem);
					energy3default = NuclideData_getTertiary(AmuletWellSetupEfficienies_selectedItem);
					welleffdefault = NuclideData_getEff(AmuletWellSetupEfficienies_selectedItem, DET_WELL);
					//betaeffdefault = NuclideData_getEff(AmuletWellSetupEfficienies_selectedItem, DET_BETA);
					probe700effdefault = NuclideData_getEff(AmuletWellSetupEfficienies_selectedItem, DET_PROBE700);
					well700effdefault = NuclideData_getEff(AmuletWellSetupEfficienies_selectedItem, DET_WELL700);

					AmuletWellEditEfficiencies_temp.nuclideID = AmuletWellSetupEfficienies_selectedItem;
					AmuletWellEditEfficiencies_temp.userEnergy1 = NuclideData_getUserPrimary(AmuletWellSetupEfficienies_selectedItem);
					AmuletWellEditEfficiencies_temp.userEnergy2 = NuclideData_getUserSecondary(AmuletWellSetupEfficienies_selectedItem);
					AmuletWellEditEfficiencies_temp.userEnergy3 = NuclideData_getUserTertiary(AmuletWellSetupEfficienies_selectedItem);
					AmuletWellEditEfficiencies_temp.userWellEff = NuclideData_getUserEff(AmuletWellSetupEfficienies_selectedItem, DET_WELL);
					AmuletWellEditEfficiencies_temp.userBetaEff = NuclideData_getUserEff(AmuletWellSetupEfficienies_selectedItem, DET_BETA);
					AmuletWellEditEfficiencies_temp.userProbe700Eff = NuclideData_getUserEff(AmuletWellSetupEfficienies_selectedItem, DET_PROBE700);
					AmuletWellEditEfficiencies_temp.userWell700Eff = NuclideData_getUserEff(AmuletWellSetupEfficienies_selectedItem, DET_WELL700);
					AmuletWellEditEfficiencies_temp.wellStamp = NuclideData_getMeasuredOnStamp(AmuletWellSetupEfficienies_selectedItem, DET_WELL);
					AmuletWellEditEfficiencies_temp.betaStamp = NuclideData_getMeasuredOnStamp(AmuletWellSetupEfficienies_selectedItem, DET_BETA);
					AmuletWellEditEfficiencies_temp.probe700Stamp = NuclideData_getMeasuredOnStamp(AmuletWellSetupEfficienies_selectedItem, DET_PROBE700);
					AmuletWellEditEfficiencies_temp.well700Stamp = NuclideData_getMeasuredOnStamp(AmuletWellSetupEfficienies_selectedItem, DET_WELL700);
				}else{
					nuclide[0] = 0;
				}
				m_ucClear = 0;
			}

			if(current.default_keV == 0) SetAmuletByte(99, 0xFF);
			else{
				if(nuclide[0] != 0){
					if(energy1default == 0.0){
						SetAmuletByte(99, 0xFF);
					}else{
						SetAmuletByte(99, 0x00);
					}
				}else{
					SetAmuletByte(99, 0x00);
				}
			}

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(102, 0xFF);
			else if(Mca_installedDetector == DET_DRILLEDPROBE700){
				SetAmuletByte(102, 0xFF);
				SetAmuletByte(103, 0xFF);
			}else{
				if(Mca_well700Installed() == DET_WELL700) SetAmuletByte(102, 0xFF);
				if(Mca_probe700Installed() == DET_PROBE700) SetAmuletByte(103, 0xFF);
			}
			m_iPhase = PHASE_WELLEDITEFFICIENCIES_PAINT;
			break;

		case PHASE_WELLEDITEFFICIENCIES_WAIT:
			break;

		case PHASE_WELLEDITEFFICIENCIES_PAINT:
			SetAmuletString(100, nuclide);

			if(energy1default == 0.0){
				message[0] = 0;
			}else{
				sprintf(message, "%.2f", energy1default);
			}
			SetAmuletString(102, message);

			if(energy2default == 0.0){
				message[0] = 0;
			}else{
				sprintf(message, "%.2f", energy2default);
			}
			SetAmuletString(103, message);

			if(energy3default == 0.0){
				message[0] = 0;
			}else{
				sprintf(message, "%.2f", energy3default);
			}
			SetAmuletString(104, message);

			if(AmuletWellEditEfficiencies_temp.userEnergy1 == 0.0){
				message[0] = 0;
			}else if(AmuletWellEditEfficiencies_temp.userEnergy1 == -1.0){
				strcpy(message, "Default");
			}else{
				sprintf(message, "%.2f", AmuletWellEditEfficiencies_temp.userEnergy1);
			}
			SetAmuletString(107, message);

			if(AmuletWellEditEfficiencies_temp.userEnergy2 == 0.0){
				message[0] = 0;
			}else if(AmuletWellEditEfficiencies_temp.userEnergy2 == -1.0){
				strcpy(message, "Default");
			}else{
				sprintf(message, "%.2f", AmuletWellEditEfficiencies_temp.userEnergy2);
			}
			SetAmuletString(108, message);

			if(AmuletWellEditEfficiencies_temp.userEnergy3 == 0.0){
				message[0] = 0;
			}else if(AmuletWellEditEfficiencies_temp.userEnergy3 == -1.0){
				strcpy(message, "Default");
			}else{
				sprintf(message, "%.2f", AmuletWellEditEfficiencies_temp.userEnergy3);
			}
			SetAmuletString(109, message);

			if(Mca_installedDetector == DET_WELL){
				if(welleffdefault == 0.0){
					message[0] = 0;
				}else{
					sprintf(message, "%.3f%%", welleffdefault);
				}
				SetAmuletString(105, message);

				if(AmuletWellEditEfficiencies_temp.userWellEff == 0.0){
					message[0] = 0;
				}else if(AmuletWellEditEfficiencies_temp.userWellEff == -1.0){
					strcpy(message, "Default");
				}else{
					sprintf(message, "%.3f%%", AmuletWellEditEfficiencies_temp.userWellEff);
				}
				SetAmuletString(110, message);

				if(AmuletWellEditEfficiencies_temp.wellStamp == (time_t)0){
					SetAmuletString(112, "");
				}else{
					strcpy(message2, "Well Efficiency was measured on ");
					GetExtendedTimeInfoSec(&AmuletWellEditEfficiencies_temp.wellStamp, message);
					strcat(message2, message);
					SetAmuletString(112, message2);
				}
				SetAmuletByte(100, 0xFF);
			}else{
				if(well700effdefault == 0.0){
					message[0] = 0;
				}else{
					sprintf(message, "%.3f%%", well700effdefault);
				}
				SetAmuletString(105, message);

				if(AmuletWellEditEfficiencies_temp.userWell700Eff == 0.0){
					message[0] = 0;
				}else if(AmuletWellEditEfficiencies_temp.userWell700Eff == -1.0){
					strcpy(message, "Default");
				}else{
					sprintf(message, "%.3f%%", AmuletWellEditEfficiencies_temp.userWell700Eff);
				}
				SetAmuletString(110, message);

				if(probe700effdefault == 0.0){
					message[0] = 0;
				}else{
					sprintf(message, "%.3f%%", probe700effdefault);
				}
				SetAmuletString(106, message);

				if(AmuletWellEditEfficiencies_temp.userProbe700Eff == 0.0){
					message[0] = 0;
				}else if(AmuletWellEditEfficiencies_temp.userProbe700Eff == -1.0){
					strcpy(message, "Default");
				}else{
					sprintf(message, "%.3f%%", AmuletWellEditEfficiencies_temp.userProbe700Eff);
				}
				SetAmuletString(111, message);

				if(AmuletWellEditEfficiencies_temp.well700Stamp == (time_t)0){
					SetAmuletString(112, "");
				}else{
					strcpy(message2, "Well Efficiency was measured on ");
					GetExtendedTimeInfoSec(&AmuletWellEditEfficiencies_temp.well700Stamp, message);
					strcat(message2, message);
					SetAmuletString(112, message2);
				}

				if(AmuletWellEditEfficiencies_temp.probe700Stamp == (time_t)0){
					SetAmuletString(115, "");
				}else{
					strcpy(message2, "Probe Efficiency was measured on ");
					GetExtendedTimeInfoSec(&AmuletWellEditEfficiencies_temp.probe700Stamp, message);
					strcat(message2, message);
					SetAmuletString(115, message2);
				}
				SetAmuletByte(100, 0xFF);
				SetAmuletByte(101, 0xFF);
			}
			m_iPhase = PHASE_WELLEDITEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLEDITEFFICIENCIES_DEFAULT_ENERGY1:
			beep_amulet();
			AmuletWellEditEfficiencies_temp.userEnergy1 = -1.0;
			m_iPhase = PHASE_WELLEDITEFFICIENCIES_PAINT;
			break;

		case PHASE_WELLEDITEFFICIENCIES_DEFAULT_ENERGY2:
			beep_amulet();
			AmuletWellEditEfficiencies_temp.userEnergy2 = -1.0;
			m_iPhase = PHASE_WELLEDITEFFICIENCIES_PAINT;
			break;

		case PHASE_WELLEDITEFFICIENCIES_DEFAULT_ENERGY3:
			beep_amulet();
			AmuletWellEditEfficiencies_temp.userEnergy3 = -1.0;
			m_iPhase = PHASE_WELLEDITEFFICIENCIES_PAINT;
			break;

		case PHASE_WELLEDITEFFICIENCIES_DEFAULT_WELL_EFF:
			beep_amulet();
			if(Mca_installedDetector == DET_WELL){
				AmuletWellEditEfficiencies_temp.userWellEff = -1.0;
				AmuletWellEditEfficiencies_temp.wellStamp = (time_t) 0;
			}else{
				AmuletWellEditEfficiencies_temp.userWell700Eff = -1.0;
				AmuletWellEditEfficiencies_temp.well700Stamp = (time_t) 0;
			}
			m_iPhase = PHASE_WELLEDITEFFICIENCIES_PAINT;
			break;

		case PHASE_WELLEDITEFFICIENCIES_DEFAULT_PROBE_EFF:
			beep_amulet();
			AmuletWellEditEfficiencies_temp.userProbe700Eff = -1.0;
			AmuletWellEditEfficiencies_temp.probe700Stamp = (time_t) 0;
			m_iPhase = PHASE_WELLEDITEFFICIENCIES_PAINT;
			break;

		case PHASE_WELLEDITEFFICIENCIES_SAVE:
			beep_amulet();
			NuclideData_updateEfficiencyMirror(&AmuletWellEditEfficiencies_temp);
			SetAmuletBackHTML();
			break;
	}
}
