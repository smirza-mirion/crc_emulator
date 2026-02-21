#define PHASE_WELLSETUPEFFICIENCIES_PRE_INIT	0
#define PHASE_WELLSETUPEFFICIENCIES_WAIT		1
#define PHASE_WELLSETUPEFFICIENCIES_FILL_GRID	2
#define PHASE_WELLSETUPEFFICIENCIES_PAGE_UP		3
#define PHASE_WELLSETUPEFFICIENCIES_PAGE_DOWN	4
#define PHASE_WELLSETUPEFFICIENCIES_ROW1		5
#define PHASE_WELLSETUPEFFICIENCIES_ROW2		6
#define PHASE_WELLSETUPEFFICIENCIES_ROW3		7
#define PHASE_WELLSETUPEFFICIENCIES_ROW4		8
#define PHASE_WELLSETUPEFFICIENCIES_ROW5		9
#define PHASE_WELLSETUPEFFICIENCIES_ROW6		10
#define PHASE_WELLSETUPEFFICIENCIES_ROW7		11
#define PHASE_WELLSETUPEFFICIENCIES_ROW8		12
#define PHASE_WELLSETUPEFFICIENCIES_ROW9		13
#define PHASE_WELLSETUPEFFICIENCIES_ROW10		14

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "nuc.h"
#include "mca.h"

short AmuletWellSetupEfficienies_selectedItem;
extern int m_iPhase;
extern unsigned char m_ucClear;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);

void AmuletWellSetupEfficiencies_menu(void){
	static short firstitemdisplayed; // Nuclide number
	//static short selecteditem;		 // 0 = first item, ..., 9 = last item
	static short totalpages;
	short index, lastitemplusone;
	char message[26];
	float energy, eff;
	DETECTOR *well;

	switch(m_iPhase){
		case PHASE_WELLSETUPEFFICIENCIES_PRE_INIT:
			if(m_ucClear == 24){
				totalpages = (USERNUC - 4) / 10;
				if(((USERNUC - 4) % 10) > 0) totalpages++;
				firstitemdisplayed = 0;
				m_ucClear = 0;
			}
			AmuletWellSetupEfficienies_selectedItem = -1;

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(90, 0xFF); // Display Well Column Only
			else SetAmuletByte(91, 0xFF); // Display Well and Probe Column

			if(Mca_installedDetector == DET_WELL){
				SetAmuletByte(92, 0xFF);
			}else if(Mca_installedDetector == DET_DRILLEDPROBE700){
				SetAmuletByte(92, 0xFF);
			}else{
				well = Mca_detectorWell700Mirror();
				if((well->installed) == DET_WELL700){
					SetAmuletByte(92, 0xFF);
				}
			}

			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_FILL_GRID;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_WAIT:
			break;

		case PHASE_WELLSETUPEFFICIENCIES_FILL_GRID:
			// Fill Grid
			sprintf(message, "%d of %d",((firstitemdisplayed / 10) + 1), totalpages);
			SetAmuletString(170, message);

			if(firstitemdisplayed + 9 > USERNUC - 5) lastitemplusone = USERNUC - 4;
			else lastitemplusone = firstitemdisplayed + 10;

			for(index=firstitemdisplayed; index<lastitemplusone; index++){
				NuclideData_getName(index, message);
				SetAmuletString(7 * (index - firstitemdisplayed) + 100, message);

				sprintf(message, "%.2f", NuclideData_getHalflife(index));
				switch(NuclideData_getHalflifeUnit(index)){
					case SEC:
						strcat(message, "s");
						break;

					case MIN:
						strcat(message, "m");
						break;

					case HOUR:
						strcat(message, "hr");
						break;

					case DAY:
						strcat(message, "dy");
						break;

					case YEAR:
						if(NuclideData_getHalflife(index) > 1.0e+6){
							sprintf(message, "%.2f", NuclideData_getHalflife(index) / 1.0e+6);
							strcat(message, " Myr");
						}else{
							strcat(message, "yr");
						}
						break;
				}
				SetAmuletString(7 * (index - firstitemdisplayed) + 101, message);

				energy = NuclideData_getEffectivePrimary(index);
				message[0] = 0;
				if(energy > 0.0) sprintf(message, "%.2f", energy);
				SetAmuletString(7 * (index - firstitemdisplayed) + 102, message);

				energy = NuclideData_getEffectiveSecondary(index);
				message[0] = 0;
				if(energy > 0.0) sprintf(message, "%.2f", energy);
				SetAmuletString(7 * (index - firstitemdisplayed) + 103, message);

				energy = NuclideData_getEffectiveTertiary(index);
				message[0] = 0;
				if(energy > 0.0) sprintf(message, "%.2f", energy);
				SetAmuletString(7 * (index - firstitemdisplayed) + 104, message);

				if(Mca_installedDetector == DET_WELL){
					eff = NuclideData_getEffectiveEff(index, DET_WELL);
					message[0] = 0;
					if(eff > 0.0) sprintf(message, "%.3f%%", eff);
					SetAmuletString(7 * (index - firstitemdisplayed) + 105, message);

					eff = NuclideData_getEffectiveEff(index, DET_BETA);
					message[0] = 0;
					if(eff > 0.0) sprintf(message, "%.3f%%", eff);
					//SetAmuletString(7 * (index - firstitemdisplayed) + 106, message);
					SetAmuletString(7 * (index-firstitemdisplayed) + 106, "");
				}else{
					eff = NuclideData_getEffectiveEff(index, DET_WELL700);
					message[0] = 0;
					if(eff > 0.0) sprintf(message, "%.3f%%", eff);
					SetAmuletString(7 * (index - firstitemdisplayed) + 105, message);

					eff = NuclideData_getEffectiveEff(index, DET_PROBE700);
					message[0] = 0;
					if(eff > 0.0) sprintf(message, "%.3f%%", eff);
					SetAmuletString(7 * (index-firstitemdisplayed) + 106, message);
				}

				SetAmuletByte((index - firstitemdisplayed) + 100, 0xFF);

				delayloop(2);
			}

			if(lastitemplusone != firstitemdisplayed){
				for(index=lastitemplusone; index<firstitemdisplayed+10; index++){
					SetAmuletString(7 * (index-firstitemdisplayed) + 100, "");
					SetAmuletString(7 * (index-firstitemdisplayed) + 101, "");
					SetAmuletString(7 * (index-firstitemdisplayed) + 102, "");
					SetAmuletString(7 * (index-firstitemdisplayed) + 103, "");
					SetAmuletString(7 * (index-firstitemdisplayed) + 104, "");
					SetAmuletString(7 * (index-firstitemdisplayed) + 105, "");
					SetAmuletString(7 * (index-firstitemdisplayed) + 106, "");
					SetAmuletByte((index - firstitemdisplayed) + 100, 0x00);
					delayloop(2);
				}
			}

			SetAmuletByte(200, 0xFF);

			// Enable Page Up
			if(firstitemdisplayed!=0) SetAmuletByte(201, 0xFF);
			else SetAmuletByte(202, 0xFF);

			// Enable Page Down
			if((firstitemdisplayed + 10) <= USERNUC - 5) SetAmuletByte(203, 0xFF);
			else SetAmuletByte(204, 0xFF);

			SetAmuletByte(206, 0xFF);

			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_PAGE_UP:
			beep_amulet();
			if((firstitemdisplayed - 10) >= 0) firstitemdisplayed -= 10;
			AmuletWellSetupEfficienies_selectedItem = -1;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_FILL_GRID;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_PAGE_DOWN:
			beep_amulet();
			if((firstitemdisplayed + 10) <= USERNUC - 5) firstitemdisplayed += 10;
			AmuletWellSetupEfficienies_selectedItem = -1;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_FILL_GRID;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW1:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 0;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW2:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 1;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW3:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 2;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW4:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 3;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW5:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 4;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW6:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 5;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW7:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 6;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW8:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 7;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW9:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 8;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;

		case PHASE_WELLSETUPEFFICIENCIES_ROW10:
			if(AmuletWellSetupEfficienies_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupEfficienies_selectedItem = firstitemdisplayed + 9;
			m_iPhase = PHASE_WELLSETUPEFFICIENCIES_WAIT;
			break;
	}
}
