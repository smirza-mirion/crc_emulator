#define PHASE_TESTIDENT_PRE_INIT	0
#define PHASE_TESTIDENT_WAIT		1
#define PHASE_TESTIDENT_SAVE		2

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "amulet.h"
#include "mca.h"
#include "printer.h"
#include "database.h"

extern int m_iPhase;
extern unsigned char m_ucClear;
extern WELLSCHILLINGTEST AmuletWellSchillingMenu_test;
extern WELLPLASMATEST AmuletWellPlasmaMenu_test;
extern WELLRBCTEST AmuletWellRBCMenu_test;

uchar AmuletTestIdentMenu_config;
TESTIDENT *AmuletTestIdentMenu_info;

unsigned char PopPageStack(void);
void SetAmuletBackHTML(void);

void AmuletTestIdent_menu(void){
#ifdef TIMES1000
	int index;
#endif
	char message[26];

	switch(m_iPhase){
		case PHASE_TESTIDENT_PRE_INIT:
			if(m_ucClear == 67){
				switch(AmuletTestIdentMenu_config){
					case 1:
						AmuletTestIdentMenu_info = &(AmuletWellSchillingMenu_test.TestIdent);
						break;

					case 2:
						AmuletTestIdentMenu_info = &(AmuletWellPlasmaMenu_test.TestIdent);
						break;

					case 3:
						AmuletTestIdentMenu_info = &(AmuletWellRBCMenu_test.TestIdent);
						break;
				}

				AmuletTestIdentMenu_info->TestIdentID = 0;
				AmuletTestIdentMenu_info->TestIdentGroupID = 0;
				AmuletTestIdentMenu_info->TestID[0] = 0;
				AmuletTestIdentMenu_info->PatientID[0] = 0;
				AmuletTestIdentMenu_info->FirstName[0] = 0;
				AmuletTestIdentMenu_info->LastName[0] = 0;
				AmuletTestIdentMenu_info->DateOfBirth = 0.0;
				AmuletTestIdentMenu_info->Sex[0] = 0;
				AmuletTestIdentMenu_info->Physician[0] = 0;
				AmuletTestIdentMenu_info->TechID[0] = 0;
				AmuletTestIdentMenu_info->CreatedOn = 0;
				AmuletTestIdentMenu_info->LastUpdated = 0;
				AmuletTestIdentMenu_info->Inactive = FALSE;
				m_ucClear = 0;
			}

			SetAmuletString(101, AmuletTestIdentMenu_info->TestID);
			SetAmuletString(102, AmuletTestIdentMenu_info->PatientID);
			SetAmuletString(103, AmuletTestIdentMenu_info->FirstName);
			SetAmuletString(104, AmuletTestIdentMenu_info->LastName);
			if(AmuletTestIdentMenu_info->DateOfBirth != 0.0){
				dateout_julian(message, AmuletTestIdentMenu_info->DateOfBirth);
				SetAmuletString(105, message);
			}
			if(strlen(AmuletTestIdentMenu_info->Sex) > 0){
				if(strcmp(AmuletTestIdentMenu_info->Sex, "M") == 0){
					SetAmuletString(106, "Male");
				}else if(strcmp(AmuletTestIdentMenu_info->Sex, "F") == 0){
					SetAmuletString(106, "Female");
				}
			}
			SetAmuletString(107, AmuletTestIdentMenu_info->Physician);
			SetAmuletString(108, AmuletTestIdentMenu_info->TechID);

			SetAmuletByte(100, 0xFF);
			if(strlen(AmuletTestIdentMenu_info->LastName) > 0) SetAmuletByte(101, 0xFF);

			m_iPhase = PHASE_TESTIDENT_WAIT;
			break;

		case PHASE_TESTIDENT_WAIT:
			break;

		case PHASE_TESTIDENT_SAVE:
			beep_amulet();
#ifdef TIMES1000
			for(index=0; index<TIMES1000; index++){
#endif
				switch(AmuletTestIdentMenu_config){
					case 1:
						DB_CreateSchillingTest(&AmuletWellSchillingMenu_test, TRUE);
#ifdef TIMES1000
						AmuletWellSchillingMenu_test.CreatedOn -= 86400;
#endif
						break;

					case 2:
						DB_CreatePlasmaTest(&AmuletWellPlasmaMenu_test, TRUE);
#ifdef TIMES1000
						AmuletWellPlasmaMenu_test.CreatedOn -= 86400;
#endif
						break;

					case 3:
						DB_CreateRBCTest(&AmuletWellRBCMenu_test, TRUE);
#ifdef TIMES1000
						AmuletWellRBCMenu_test.CreatedOn -= 86400;
#endif
						break;

				}
#ifdef TIMES1000
			}
#endif
			PopPageStack();
			PopPageStack();
			SetAmuletBackHTML();
			break;
	}
}
