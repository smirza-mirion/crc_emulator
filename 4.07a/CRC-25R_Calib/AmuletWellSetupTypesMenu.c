#define PHASE_WELLSETUPTYPES_PRE_INIT	0
#define PHASE_WELLSETUPTYPES_WAIT		1
#define PHASE_WELLSETUPTYPES_ACCEPT		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "cs.h"
#include "wipes.h"
#include "mca.h"
#include "nuc.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern CURRENT current;

char SetAmuletByte(unsigned char ucIndex, unsigned char ucValue);
char SetAmuletString(unsigned char ucIndex, char *pcValue);
void SetAmuletBackHTML(void);

int AmuletWellSetupTypes_type;
WELLWIPETYPE AmuletWellSetupTypes_wellWipeType;

void AmuletWellSetupTypes_menu(void){
	WELLWIPETYPE *currentWellWipeType;
	char message[25];
	float convertedvalue;
	int index;

	switch(m_iPhase){
		case PHASE_WELLSETUPTYPES_PRE_INIT:
			if((m_ucClear == 53) || (m_ucClear == 54) || (m_ucClear == 55) || (m_ucClear == 56) || (m_ucClear == 57)){
				AmuletWellSetupTypes_type = m_ucClear - 52;
				currentWellWipeType = WipesCurrentType(AmuletWellSetupTypes_type);
				DB_CopyWipeType(&AmuletWellSetupTypes_wellWipeType, currentWellWipeType);
				m_ucClear = 0;
			}

			switch(AmuletWellSetupTypes_type){
				case 1:
					SetAmuletString(100, "Edit Background Trigger");
					break;

				case 2:
					SetAmuletString(100, "Edit Default Work Area Settings");
					break;

				case 3:
					SetAmuletString(100, "Edit Default Unrestricted Area Settings");
					break;

				case 4:
					SetAmuletString(100, "Edit Default Sealed Settings");
					break;

				case 5:
					SetAmuletString(100, "Edit Default Package Settings");
					break;
			}


			message[0] = 0;
			if(current.system == CI){
				if(AmuletWellSetupTypes_type == 1) sprintf(message, "%.2f cpm", AmuletWellSetupTypes_wellWipeType.Threshold);
				else if((AmuletWellSetupTypes_type == 2) || (AmuletWellSetupTypes_type == 3) || (AmuletWellSetupTypes_type == 5)) sprintf(message, "%.2f dpm", AmuletWellSetupTypes_wellWipeType.Threshold);
				else if(AmuletWellSetupTypes_type == 4){
					convertedvalue = Mca_convertDpmToCi(AmuletWellSetupTypes_wellWipeType.Threshold);
					convertedvalue *= 1e+9;
					sprintf(message, "%.3f nCi", convertedvalue);
				}
			}else{
				if(AmuletWellSetupTypes_type == 1) sprintf(message, "%.3f cps", Mca_convertDpmToBq(AmuletWellSetupTypes_wellWipeType.Threshold));
				else if((AmuletWellSetupTypes_type == 2) || (AmuletWellSetupTypes_type == 3) || (AmuletWellSetupTypes_type == 4) || (AmuletWellSetupTypes_type == 5)) sprintf(message, "%.3f Bq", Mca_convertDpmToBq(AmuletWellSetupTypes_wellWipeType.Threshold));
			}
			SetAmuletString(102, message);

			sprintf(message, "%d sec", AmuletWellSetupTypes_wellWipeType.CountTime);
			SetAmuletString(103, message);
			SetAmuletByte(100, 0xFF);

			if(AmuletWellSetupTypes_type != 1){
				for(index=0; index<10; index++){
					if(AmuletWellSetupTypes_wellWipeType.NuclideID[index] >= 0){
						NuclideData_getName(AmuletWellSetupTypes_wellWipeType.NuclideID[index], message);
						SetAmuletString(104 + index, message);
					}
				}
				SetAmuletByte(101, 0xFF);
			}

			SetAmuletByte(101 + AmuletWellSetupTypes_type, 0xFF);

			m_iPhase = PHASE_WELLSETUPTYPES_WAIT;
			break;

		case PHASE_WELLSETUPTYPES_WAIT:
			break;

		case PHASE_WELLSETUPTYPES_ACCEPT:
			beep_amulet();
			AmuletWellSetupTypes_wellWipeType.WellWipeTypeID = 0;
			DB_UpdateWellWipeType(&AmuletWellSetupTypes_wellWipeType, TRUE);
			if(AmuletWellSetupTypes_wellWipeType.WellWipeTypeID){
				currentWellWipeType = WipesCurrentType(AmuletWellSetupTypes_type);
				DB_CopyWipeType(currentWellWipeType, &AmuletWellSetupTypes_wellWipeType);
			}
			SetAmuletBackHTML();
			break;
	}
}
