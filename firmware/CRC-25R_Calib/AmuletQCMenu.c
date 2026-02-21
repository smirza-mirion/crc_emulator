#define PHASE_QC_PRE_INIT	0
#define PHASE_QC_WAIT		1

#include <stdlib.h>
#include <string.h>
#include "crc.h"
#include "chambfac.h"
#include "amulet.h"
#include "message.h"

extern int m_iPhase;
extern CURRENT current;

void send_to_amulet_string(uchar ucIndex, char message0[]);

void AmuletQC_menu(void){
	char acMsg[25];

	switch(m_iPhase){
		case PHASE_QC_PRE_INIT:

			SetAmuletByte(92,current.language);

			send_amulet_message(L_SINGLE_STRIP_TEST,100);    // "Single Strip Test"
			send_amulet_message(L_TWO_STRIP_TEST,102);    // "Two Strip Test"
			send_amulet_message(L_HMPAO_TEST,104);    // "HMPAO Test"
			send_amulet_message(L_MAG3_TEST,106);    // "MAG3 Test"
			
			if(current.num_chambers>1){
				if(chamber_type(0) == R_CHAMB && chamber_type(1) == R_CHAMB) SetAmuletByte(21, 0xFF);
			}

			if(current.num_chambers>0){
				sprintf(acMsg, "Ch: %d", current.main_chamber + 1);
				switch(chamber_type(current.main_chamber)){
					case R_CHAMB:
						strcat(acMsg, ", R");
						break;
					case P_CHAMB:
						strcat(acMsg, ", PET");
						break;
					case B_CHAMB:
						strcat(acMsg, ", BT");
						break;
					case ONE_DOT_EIGHT_CHAMB:
						strcat(acMsg, ", 1.8 Atm");
						break;
					case C_CHAMB:
						strcat(acMsg, ", HR");
						break;
					case K_CHAMB:
						strcat(acMsg, ", 1K");
						break;
				}
				send_to_amulet_string(9, acMsg);
				SetAmuletByte(20, 0xFF);
			}

			m_iPhase = PHASE_QC_WAIT;
			break;

		case PHASE_QC_WAIT:
			break;
	}
}
