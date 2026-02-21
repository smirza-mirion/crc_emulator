#define PHASE_WELLSETUPUSERNUCLIDES_PRE_INIT	0
#define PHASE_WELLSETUPUSERNUCLIDES_WAIT		1
#define PHASE_WELLSETUPUSERNUCLIDES_ROW1		2
#define PHASE_WELLSETUPUSERNUCLIDES_ROW2		3
#define PHASE_WELLSETUPUSERNUCLIDES_ROW3		4
#define PHASE_WELLSETUPUSERNUCLIDES_ROW4		5
#define PHASE_WELLSETUPUSERNUCLIDES_ROW5		6
#define PHASE_WELLSETUPUSERNUCLIDES_ROW6		7
#define PHASE_WELLSETUPUSERNUCLIDES_ROW7		8
#define PHASE_WELLSETUPUSERNUCLIDES_ROW8		9
#define PHASE_WELLSETUPUSERNUCLIDES_ROW9		10
#define PHASE_WELLSETUPUSERNUCLIDES_ROW10		11

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "mca.h"
#include "nuc.h"

short AmuletWellSetupUserNuclides_selectedItem;
short AmuletWellSetupUserNuclides_selectedNuclideIndex;
extern int m_iPhase;
extern unsigned char m_ucClear;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);

void AmuletWellSetupUserNuclides_menu(void){
	int index, jndex;
	char message[26];
	static short NuclideIndex[10];
	float energy, eff;

	switch(m_iPhase){
		case PHASE_WELLSETUPUSERNUCLIDES_PRE_INIT:
			if(m_ucClear == 73) m_ucClear = 0;

			for(index=0; index<10; index++) NuclideIndex[index] = -1;

			AmuletWellSetupUserNuclides_selectedItem = -1;

			if(Mca_installedDetector == DET_WELL) SetAmuletByte(90, 0xFF); // Display Well Column Only
			else SetAmuletByte(91, 0xFF); // Display Well and Probe Column

			if(NuclideData_getDetectorNuclideCount() > 0){
				jndex = 0;
				for(index=USERNUC+10; index<USERNUC+20; index++){
					if(NuclideData_getHalflifeUnit(index) != -1){
						NuclideIndex[jndex] = index;

						NuclideData_getName(index, message);
						SetAmuletString(100 + 7*jndex, message);

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
						SetAmuletString(101 + 7*jndex, message);

						energy = NuclideData_getEffectivePrimary(index);
						message[0] = 0;
						if(energy > 0.0) sprintf(message, "%.2f", energy);
						SetAmuletString(102 + 7*jndex, message);

						energy = NuclideData_getEffectiveSecondary(index);
						message[0] = 0;
						if(energy > 0.0) sprintf(message, "%.2f", energy);
						SetAmuletString(103 + 7*jndex, message);

						energy = NuclideData_getEffectiveTertiary(index);
						message[0] = 0;
						if(energy > 0.0) sprintf(message, "%.2f", energy);
						SetAmuletString(104 + 7*jndex, message);

						if(Mca_installedDetector == DET_WELL){
							eff = NuclideData_getEffectiveEff(index, DET_WELL);
							message[0] = 0;
							if(eff > 0.0) sprintf(message, "%.3f%%", eff);
							SetAmuletString(105 + 7*jndex, message);

							message[0] = 0;
							SetAmuletString(106 + 7*jndex, message);
						}else{
							eff = NuclideData_getEffectiveEff(index, DET_WELL700);
							message[0] = 0;
							if(eff > 0.0) sprintf(message, "%.3f%%", eff);
							SetAmuletString(105 + 7*jndex, message);

							eff = NuclideData_getEffectiveEff(index, DET_PROBE700);
							message[0] = 0;
							if(eff > 0.0) sprintf(message, "%.3f%%", eff);
							SetAmuletString(106 + 7*jndex, message);
						}

						SetAmuletByte(100 + jndex, 0xFF);

						jndex++;
						delayloop(2);
					}
				}
			}

			for(index=0; index<10; index++){
				if(NuclideIndex[index] == -1){
					SetAmuletByte(206, 0xFF);
					break;
				}
			}

			SetAmuletByte(200, 0xFF);
			m_iPhase = PHASE_WELLSETUPUSERNUCLIDES_WAIT;
			break;

		case PHASE_WELLSETUPUSERNUCLIDES_WAIT:
			break;

		case PHASE_WELLSETUPUSERNUCLIDES_ROW1:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW2:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW3:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW4:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW5:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW6:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW7:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW8:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW9:
		case PHASE_WELLSETUPUSERNUCLIDES_ROW10:
			if(AmuletWellSetupUserNuclides_selectedItem == -1) SetAmuletByte(205, 0xFF);
			AmuletWellSetupUserNuclides_selectedItem = m_iPhase - PHASE_WELLSETUPUSERNUCLIDES_ROW1;
			AmuletWellSetupUserNuclides_selectedNuclideIndex = NuclideIndex[AmuletWellSetupUserNuclides_selectedItem];
			m_iPhase = PHASE_WELLSETUPUSERNUCLIDES_WAIT;
			break;
	}
}
